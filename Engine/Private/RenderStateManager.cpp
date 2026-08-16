#include "Engine_Define.h"
#include "RenderStateManager.h"
#include "GameInstance.h"


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::RenderStateManager::RenderStateManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice(pDevice), m_pContext(pContext)
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

Engine::RenderStateManager::~RenderStateManager()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 이니셜라이즈 ////////////////////////////////////////////////////////
HRESULT Engine::RenderStateManager::Initialize()
{

#pragma region 래스터라이저 스테이트
    //쉐이더 래스터라이저, 뎁스스텐실,블렌드 여기서 설정
    //처음꺼 Solid or Wire설정 
    //어느면 컬링할지
    //TRUE면 CCW(뒷면)이 앞면 FALSE면 CW이 앞면
    //near~far 밖이면 자를지
    //멀티샘플링사용(MSAA)) //렌더타겟/스왑체인이 MSAA 설정돼 있어야 실제로 효과 있음
    
    // 기본 RS
    {
        D3D11_RASTERIZER_DESC desc{};
        desc.FillMode = D3D11_FILL_SOLID;
        desc.CullMode = D3D11_CULL_BACK;
        desc.FrontCounterClockwise = FALSE;
        desc.DepthClipEnable = TRUE;
        desc.MultisampleEnable = TRUE;
        ID3D11RasterizerState* pRS = nullptr;
        CHECK_FAILED(m_pDevice->CreateRasterizerState(&desc, &pRS), E_FAIL);
        m_umapRS.try_emplace("RS_Default", pRS);
        m_vecRS.push_back(pRS);
    }

    // 채우기 CCW
    {
        D3D11_RASTERIZER_DESC desc{};
        desc.FillMode = D3D11_FILL_SOLID;
        desc.CullMode = D3D11_CULL_FRONT;
        desc.FrontCounterClockwise = FALSE;
        desc.DepthClipEnable = TRUE;

        ID3D11RasterizerState* pRS = nullptr;
        CHECK_FAILED(m_pDevice->CreateRasterizerState(&desc, &pRS), E_FAIL);
        m_umapRS.try_emplace("RS_SolidCCW", pRS);
        m_vecRS.push_back(pRS);
    }

    // 채우기 Scissor
    {
        D3D11_RASTERIZER_DESC desc = {};
        desc.FillMode = D3D11_FILL_SOLID;
        desc.CullMode = D3D11_CULL_NONE;
        desc.ScissorEnable = TRUE;
        desc.DepthClipEnable = TRUE;

        ID3D11RasterizerState* pRS = nullptr;
        m_pDevice->CreateRasterizerState(&desc, &pRS);

        m_umapRS.try_emplace("RS_SolidScissor", pRS);
        m_vecRS.push_back(pRS);
    }

    // 채우기 NONE
    {
        D3D11_RASTERIZER_DESC desc{};
        desc.FillMode = D3D11_FILL_SOLID;
        desc.CullMode = D3D11_CULL_NONE;
        desc.FrontCounterClockwise = FALSE;
        desc.DepthClipEnable = TRUE;

        ID3D11RasterizerState* pRS = nullptr;
        CHECK_FAILED(m_pDevice->CreateRasterizerState(&desc, &pRS), E_FAIL);
        m_umapRS.try_emplace("RS_SolidNone", pRS);
        m_vecRS.push_back(pRS);
    }

    // 와이어프레임 CW
    {
        D3D11_RASTERIZER_DESC desc{};
        desc.FillMode = D3D11_FILL_WIREFRAME;
        desc.CullMode = D3D11_CULL_BACK;
        desc.FrontCounterClockwise = FALSE;
        desc.DepthClipEnable = TRUE;

        ID3D11RasterizerState* pRS = nullptr;
        CHECK_FAILED(m_pDevice->CreateRasterizerState(&desc, &pRS), E_FAIL);
        m_umapRS.try_emplace("RS_WireframeCW", pRS);
        m_vecRS.push_back(pRS);
    }

    // 와이어프레임 CCW
    {
        D3D11_RASTERIZER_DESC desc{};
        desc.FillMode = D3D11_FILL_WIREFRAME;
        desc.CullMode = D3D11_CULL_FRONT;
        desc.FrontCounterClockwise = FALSE;
        desc.DepthClipEnable = TRUE;

        ID3D11RasterizerState* pRS = nullptr;
        CHECK_FAILED(m_pDevice->CreateRasterizerState(&desc, &pRS), E_FAIL);
        m_umapRS.try_emplace("RS_WireframeCCW", pRS);
        m_vecRS.push_back(pRS);
    }

    // 와이어프레임 NONE
    {
        D3D11_RASTERIZER_DESC desc{};
        desc.FillMode = D3D11_FILL_WIREFRAME;
        desc.CullMode = D3D11_CULL_NONE;
        desc.FrontCounterClockwise = FALSE;
        desc.DepthClipEnable = TRUE;

        ID3D11RasterizerState* pRS = nullptr;
        CHECK_FAILED(m_pDevice->CreateRasterizerState(&desc, &pRS), E_FAIL);
        m_umapRS.try_emplace("RS_WireframeNone", pRS);
        m_vecRS.push_back(pRS);
    }


    // 폰트자르기(scissor enable)
    {
        D3D11_RASTERIZER_DESC desc{};
        desc.FillMode = D3D11_FILL_SOLID;
        desc.CullMode = D3D11_CULL_NONE;
        desc.ScissorEnable = TRUE; // 이게 핵심!

        ID3D11RasterizerState* pRS = nullptr;
        CHECK_FAILED(m_pDevice->CreateRasterizerState(&desc, &pRS), E_FAIL);
        m_umapRS.try_emplace("RS_ScissorEnable", pRS);
        m_vecRS.push_back(pRS);
    }

    // 그림자용 RS
    {
        D3D11_RASTERIZER_DESC desc{};
        desc.FillMode = D3D11_FILL_SOLID;
        desc.CullMode = D3D11_CULL_BACK;
        desc.FrontCounterClockwise = FALSE;
        desc.DepthClipEnable = TRUE;
        desc.MultisampleEnable = TRUE;
        desc.DepthBias = 500;          // 이거 추가
        desc.SlopeScaledDepthBias = 1.5f; // 이거 추가
        desc.DepthBiasClamp = 0.01f;
        ID3D11RasterizerState* pRS = nullptr;
        CHECK_FAILED(m_pDevice->CreateRasterizerState(&desc, &pRS), E_FAIL);
        m_umapRS.try_emplace("RS_Shadow", pRS);
        m_vecRS.push_back(pRS);
    }

#pragma endregion


#pragma region 뎁스 스텐실 스테이트
    //깊이테스트(Z버퍼검사) TRUE면 깊이검사 FALSE면 안함
    //깊이버퍼에 기록할지(보통 투명에 ZERO줌) 안하면 제일 먼저 그려진건 가려진다
    //깊이비교를 어떻게할지
    //스텐실은 거울효과라는데 안써본듯
    
    //깊이 테스트를 통과해야 깊이를 기록한다
    // 기본 (깊이 테스트 + 쓰기 ON)
    {
        D3D11_DEPTH_STENCIL_DESC desc{};
        desc.DepthEnable = TRUE;
        desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
        desc.StencilEnable = FALSE;

        ID3D11DepthStencilState* pDSS = nullptr;
        CHECK_FAILED(m_pDevice->CreateDepthStencilState(&desc, &pDSS), E_FAIL);
        m_umapDSS.try_emplace("DSS_Default", pDSS);
        m_vecDSS.push_back(pDSS);
    }

    // DSS_Sky: 스카이박스용 (깊이 테스트만, 쓰기 OFF)
    {
        D3D11_DEPTH_STENCIL_DESC desc{};
        desc.DepthEnable = TRUE;
        desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
        desc.StencilEnable = FALSE;

        ID3D11DepthStencilState* pDSS = nullptr;
        CHECK_FAILED(m_pDevice->CreateDepthStencilState(&desc, &pDSS), E_FAIL);
        m_umapDSS.try_emplace("DSS_Sky", pDSS);
        m_vecDSS.push_back(pDSS);
    }

    // DSS_NoDepth: 깊이 테스트 완전 OFF (UI 등)
    {
        D3D11_DEPTH_STENCIL_DESC desc{};
        desc.DepthEnable = FALSE;
        desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        desc.StencilEnable = FALSE;

        ID3D11DepthStencilState* pDSS = nullptr;
        CHECK_FAILED(m_pDevice->CreateDepthStencilState(&desc, &pDSS), E_FAIL);
        m_umapDSS.try_emplace("DSS_NoDepth", pDSS);
        m_vecDSS.push_back(pDSS);
    }

    // DSS_StencilWrite: 맵 오브젝트용 (깊이 쓰기 + 스텐실 1 기록)
    {
        D3D11_DEPTH_STENCIL_DESC desc{};
        desc.DepthEnable = TRUE;
        desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
        desc.StencilEnable = TRUE;
        desc.StencilReadMask = 0xFF;
        desc.StencilWriteMask = 0xFF;
        desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
        desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
        desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        desc.BackFace = desc.FrontFace;

        ID3D11DepthStencilState* pDSS = nullptr;
        CHECK_FAILED(m_pDevice->CreateDepthStencilState(&desc, &pDSS), E_FAIL);
        m_umapDSS.try_emplace("DSS_StencilWrite", pDSS);
        m_vecDSS.push_back(pDSS);
    }

    // DSS_DecalRead: 데칼용 (깊이 읽기만 + 스텐실 == 1인 곳만 통과)
    {
        D3D11_DEPTH_STENCIL_DESC desc{};
        desc.DepthEnable = TRUE;
        desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
        desc.StencilEnable = TRUE;
        desc.StencilReadMask = 0xFF;
        desc.StencilWriteMask = 0x00;   // 읽기만
        desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
        desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        desc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;        // Ref가 스텐실과 같을 때만 통과
        desc.BackFace = desc.FrontFace;

        ID3D11DepthStencilState* pDSS = nullptr;
        CHECK_FAILED(m_pDevice->CreateDepthStencilState(&desc, &pDSS), E_FAIL);
        m_umapDSS.try_emplace("DSS_DecalRead", pDSS);
        m_vecDSS.push_back(pDSS);
    }
    //DDS_PointLight 포인트라이트용(테스트)
    {
        D3D11_DEPTH_STENCIL_DESC desc = {};
        desc.DepthEnable = true;
        desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;  // 쓰기 끄기
        desc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;     // 핵심
        desc.StencilEnable = FALSE;

        ID3D11DepthStencilState* pDSS = nullptr;
        CHECK_FAILED(m_pDevice->CreateDepthStencilState(&desc, &pDSS), E_FAIL);
        m_umapDSS.try_emplace("DSS_PointLight", pDSS);
        m_vecDSS.push_back(pDSS);
    }
#pragma endregion


#pragma region 블렌드 스테이트
    //첫번째는 MSAA 쓸때만 의미 있음
    //
    // 블렌딩 OFF
    {
        D3D11_BLEND_DESC desc{};
        desc.AlphaToCoverageEnable = FALSE;
        desc.IndependentBlendEnable = FALSE;
        desc.RenderTarget[0].BlendEnable = FALSE;
        desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        ID3D11BlendState* pBS = nullptr;
        CHECK_FAILED(m_pDevice->CreateBlendState(&desc, &pBS), E_FAIL);
        m_umapBS.try_emplace("BS_Default", pBS);
        m_vecBS.push_back(pBS);
    }

    // 알파 블렌딩
    {
        D3D11_BLEND_DESC desc{};
        desc.AlphaToCoverageEnable = FALSE; //MSAA 쓸때만 유효
        desc.IndependentBlendEnable = FALSE; //TRUE하면 RTV마다 다르게 블렌드가능
        desc.RenderTarget[0].BlendEnable = TRUE;  //블렌드하겠다
        desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
        desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        desc.RenderTarget[1].BlendEnable = TRUE;
        desc.RenderTarget[1].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        desc.RenderTarget[1].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        desc.RenderTarget[1].BlendOp = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[1].SrcBlendAlpha = D3D11_BLEND_ONE;
        desc.RenderTarget[1].DestBlendAlpha = D3D11_BLEND_ZERO;
        desc.RenderTarget[1].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[1].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        ID3D11BlendState* pBS = nullptr;
        CHECK_FAILED(m_pDevice->CreateBlendState(&desc, &pBS), E_FAIL);
        m_umapBS.try_emplace("BS_AlphaBlend", pBS);
        m_vecBS.push_back(pBS);
    }

    // 가산 블렌딩(Additive Blending) 빛나는애들 쓸때 사용
    {
        D3D11_BLEND_DESC desc{};
        desc.AlphaToCoverageEnable = FALSE;
        desc.IndependentBlendEnable = FALSE;
        desc.RenderTarget[0].BlendEnable = TRUE;
        desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
        desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
        desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        desc.RenderTarget[1].BlendEnable = TRUE;
        desc.RenderTarget[1].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        desc.RenderTarget[1].DestBlend = D3D11_BLEND_ONE;
        desc.RenderTarget[1].BlendOp = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[1].SrcBlendAlpha = D3D11_BLEND_ONE;
        desc.RenderTarget[1].DestBlendAlpha = D3D11_BLEND_ONE;
        desc.RenderTarget[1].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[1].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        ID3D11BlendState* pBS = nullptr;
        CHECK_FAILED(m_pDevice->CreateBlendState(&desc, &pBS), E_FAIL);
        m_umapBS.try_emplace("BS_Additive", pBS);
        m_vecBS.push_back(pBS);
    }


    {
        //폰트용 블렌드설정
        D3D11_BLEND_DESC desc{};
        desc.AlphaToCoverageEnable = FALSE;
        desc.IndependentBlendEnable = FALSE;

        // RenderTarget[0] 설정 (Premultiplied Alpha 핵심 로직)
        desc.RenderTarget[0].BlendEnable = TRUE;
        // 소스 색상에 이미 알파가 곱해져 있으므로 다시 곱하지 않고 그대로(ONE) 더합니다.
        desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
        desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

        desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
        desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        // (필요하다면 RenderTarget[1]도 동일하게 복사)

        ID3D11BlendState* pBS = nullptr;
        CHECK_FAILED(m_pDevice->CreateBlendState(&desc, &pBS), E_FAIL);
        m_umapBS.try_emplace("BS_PremultipliedAlpha", pBS);
        m_vecBS.push_back(pBS);
    }
#pragma endregion

    return S_OK;
}
/******************************************************* 이니셜라이즈 *******************************************************/



//////////////////////////////////////////////////////// Setter 함수 ////////////////////////////////////////////////////////
void Engine::RenderStateManager::Set_RasterizerState(RSSET _rasterrizerState)
{
    m_pContext->RSSetState(m_vecRS[_UINT(_rasterrizerState)]);
}

void Engine::RenderStateManager::Set_RasterizerState(const _string& _name)
{
    auto it = m_umapRS.find(_name);
    if (it != m_umapRS.end())
        m_pContext->RSSetState(it->second);
}

void Engine::RenderStateManager::Set_DepthStencilState(DSSSET _depthStencilState, _uint _stencilRef)
{
    m_pContext->OMSetDepthStencilState(m_vecDSS[_UINT(_depthStencilState)], _stencilRef);
}

void Engine::RenderStateManager::Set_DepthStencilState(const _string& _name, _uint _stencilRef)
{
    auto it = m_umapDSS.find(_name);
    if (it != m_umapDSS.end())
        m_pContext->OMSetDepthStencilState(it->second, _stencilRef);
}

void Engine::RenderStateManager::Set_BlendState(BSSET _blendState)
{
    m_pContext->OMSetBlendState(m_vecBS[_UINT(_blendState)], nullptr, 0xffffff);
}

void Engine::RenderStateManager::Set_BlendState(const _string& _name)
{
    auto it = m_umapBS.find(_name);
    if (it != m_umapBS.end())
        m_pContext->OMSetBlendState(it->second, nullptr, 0xffffffff);
}
ID3D11RasterizerState* Engine::RenderStateManager::Get_RS(const _string& _name)
{
    auto it = m_umapRS.find(_name);
    if (it != m_umapRS.end())
        return it->second;
    return nullptr;
}

ID3D11RasterizerState* Engine::RenderStateManager::Get_RS(RSSET _rasterrizerState)
{
    return m_vecRS[_UINT(_rasterrizerState)];
}

ID3D11DepthStencilState* Engine::RenderStateManager::Get_DSS(const _string& _name)
{
    auto it = m_umapDSS.find(_name);
    if (it != m_umapDSS.end())
        return it->second;

    return nullptr;
}

ID3D11DepthStencilState* Engine::RenderStateManager::Get_DSS(DSSSET _depthStencilState)
{
    return m_vecDSS[_UINT(_depthStencilState)];
}

ID3D11BlendState* Engine::RenderStateManager::Get_BS(const _string& _name)
{
    auto it = m_umapBS.find(_name);
    if (it != m_umapBS.end())
        return it->second;
    return nullptr;
}

ID3D11BlendState* Engine::RenderStateManager::Get_BS(BSSET _blendState)
{
    return m_vecBS[_UINT(_blendState)];
}
/******************************************************* Setter 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
RenderStateManager* Engine::RenderStateManager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    RenderStateManager* pInstance = new RenderStateManager(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize(), L"RenderStateManager 생성 실패", L"Caution!!!", nullptr);

    return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::RenderStateManager::Free()
{
    __super::Free();

    for (auto& pair : m_umapRS)
        Safe_Release(pair.second);
    m_umapRS.clear();

    for (auto& pair : m_umapDSS)
        Safe_Release(pair.second);
    m_umapDSS.clear();

    for (auto& pair : m_umapBS)
        Safe_Release(pair.second);
    m_umapBS.clear();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
/******************************************************* 객체 반환 함수 *******************************************************/
