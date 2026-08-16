#include "Engine_Define.h"
#include "RenderTargetManager.h"

#include "GameInstance.h"
#include "RenderTarget.h"

#include "GameObject.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::RenderTargetManager::RenderTargetManager()
{
}

Engine::RenderTargetManager::RenderTargetManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice(pDevice), m_pContext(pContext), m_pGameInstance(GameInstance::GetInstance())
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

Engine::RenderTargetManager::~RenderTargetManager()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::RenderTargetManager::Initialize()
{
    m_iNumRenderTargets = _UINT(RenderTargets::END);
    m_iNumMultiRenderTargets = _UINT(MRT::END);

    // 벡터 크기 미리 할당 (nullptr로 초기화됨)
    m_vecRenderTargets.resize(m_iNumRenderTargets, nullptr);
    m_vecMultiRenderTargets.resize(m_iNumMultiRenderTargets);

    return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 컨테이너 관리 함수 ////////////////////////////////////////////////////////
HRESULT Engine::RenderTargetManager::Add_RenderTarget(RenderTargets _targetName, _float _width, _float _height, DXGI_FORMAT _textureformat, const _float4& _clearcolor)
{
    if (_UINT(_targetName) >= m_vecRenderTargets.size())
        return E_FAIL;

    if (m_vecRenderTargets[_UINT(_targetName)] != nullptr)
    {
        MSG_ON((L"추가하려는 렌더 타겟이 이미 존재합니다"), L"추가 실패!!!");
        BREAK;
        return E_FAIL;
    }

    RenderTarget* rtv = RenderTarget::Create(m_pDevice, m_pContext, _width, _height, _textureformat, _clearcolor);
    CHECK_NULLPTR(rtv);

    m_vecRenderTargets[_UINT(_targetName)] = rtv;

    return S_OK;
}

HRESULT Engine::RenderTargetManager::Add_RenderTarget_Shadow(RenderTargets _targetName, _float _width, _float _height, DXGI_FORMAT _textureformat, const _float4& _clearcolor)
{
    if (_UINT(_targetName) >= m_vecRenderTargets.size())
        return E_FAIL;

    if (m_vecRenderTargets[_UINT(_targetName)] != nullptr)
    {
        MSG_ON((L"추가하려는 렌더 타겟이 이미 존재합니다"), L"추가 실패!!!");
        BREAK;
        return E_FAIL;
    }

    RenderTarget* rtv = RenderTarget::Create_Shadow(m_pDevice, m_pContext, _width, _height, _textureformat, _clearcolor);
    CHECK_NULLPTR(rtv);

    m_vecRenderTargets[_UINT(_targetName)] = rtv;

    return S_OK;
}

HRESULT Engine::RenderTargetManager::Add_MultiRenderTarget(MRT _mrtName, RenderTargets _rtvName)
{
    if (_UINT(_rtvName) >= m_vecRenderTargets.size() || m_vecRenderTargets[_UINT(_rtvName)] == nullptr)
    {
        MSG_ON((L"MRT에 추가하려는 렌더 타겟이 존재하지 않습니다"), L"추가 실패!!!");
        BREAK;
        return E_FAIL;
    }

    RenderTarget* rtv = m_vecRenderTargets[_UINT(_rtvName)];

    // [Modified] Vector Indexing
    if (_UINT(_mrtName) < m_vecMultiRenderTargets.size())
    {
        m_vecMultiRenderTargets[_UINT(_mrtName)].push_back(rtv);
        Safe_AddRef(rtv);
    }

    return S_OK;
}

HRESULT Engine::RenderTargetManager::Bind_ShaderResource_FullSlot(RenderTargets _rtvName, Shader* _shader, _uint _slotNum, _uint _stageMask)
{
    if (_UINT(_rtvName) >= m_vecRenderTargets.size() || m_vecRenderTargets[_UINT(_rtvName)] == nullptr)
        return E_FAIL;

    return m_vecRenderTargets[_UINT(_rtvName)]->Bind_ShaderResource_FullSlot(_shader, _slotNum, _stageMask);
}
/******************************************************* 컨테이너 관리 함수 *******************************************************/



//////////////////////////////////////////////////////// MultiRenderTarget 관리 함수 ////////////////////////////////////////////////////////
HRESULT Engine::RenderTargetManager::Begin_MRT(MRT _mrtName, _bool DSV, _bool _Clear)
{
    _uint iIndex = _UINT(_mrtName);

    // [Modified] Map Contains -> Vector Index & Empty Check
    if (iIndex >= m_vecMultiRenderTargets.size())
    {
        MSG_ON((L"교체하려는 MRT 인덱스가 범위를 벗어났습니다"), L"RTV 교체 실패!!!");
        BREAK;
        return E_FAIL;
    }

    if (m_vecMultiRenderTargets[iIndex].empty())
    {
        MSG_ON((L"MRT에 등록된 RTV가 없습니다"), L"RTV 교체 실패!!!");
        BREAK;
        return E_FAIL;
    }

    // 원본 렌더타겟인 백버퍼 RTV, DSV를 저장
    m_pContext->OMGetRenderTargets(1, &m_pBackBufferRTV, &m_pBackBufferDSV);

    // VS와 PS의 SRV를 0 ~ 127번까지 nullptr로 채워준다
    m_pContext->VSSetShaderResources(0, 128, m_pNullSRVs);
    m_pContext->PSSetShaderResources(0, 128, m_pNullSRVs);

    ID3D11RenderTargetView* RTVs[8] = { nullptr };
    _uint numRTVs = { 0 };

    for (auto& rtv : m_vecMultiRenderTargets[iIndex])
    {
        if (numRTVs >= 8)
        {
            MSG_ON((L"MRT에 등록된 RTV가 8개가 넘습니다"), L"이럼 좆된거임");
            BREAK;
            return E_FAIL;
        }
        if (_Clear)
        {
            rtv->ClearRTV();
        }

        RTVs[numRTVs++] = rtv->Get_RTV();
    }

    if (numRTVs >= 8)
        numRTVs = 8;

    //if (_DSV != nullptr)
    //{
    //    m_pContext->ClearDepthStencilView(_DSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
    //}

    m_pContext->OMSetRenderTargets(numRTVs, RTVs, DSV == false ? m_pBackBufferDSV : nullptr);

    return S_OK;
}

HRESULT Engine::RenderTargetManager::Begin_MRT_HDR(MRT _mrtName, ID3D11DepthStencilView* _DSV, _bool _Clear)
{
    _uint iIndex = _UINT(_mrtName);

    // [Modified] Map Contains -> Vector Index & Empty Check
    if (iIndex >= m_vecMultiRenderTargets.size())
    {
        MSG_ON((L"교체하려는 MRT 인덱스가 범위를 벗어났습니다"), L"RTV 교체 실패!!!");
        BREAK;
        return E_FAIL;
    }

    if (m_vecMultiRenderTargets[iIndex].empty())
    {
        MSG_ON((L"MRT에 등록된 RTV가 없습니다"), L"RTV 교체 실패!!!");
        BREAK;
        return E_FAIL;
    }

    // 원본 렌더타겟인 백버퍼 RTV, DSV를 저장
    m_pContext->OMGetRenderTargets(1, &m_pBackBufferRTV, &m_pBackBufferDSV);

    // VS와 PS의 SRV를 0 ~ 127번까지 nullptr로 채워준다
    m_pContext->VSSetShaderResources(0, 128, m_pNullSRVs);
    m_pContext->PSSetShaderResources(0, 128, m_pNullSRVs);

    ID3D11RenderTargetView* RTVs[8] = { nullptr };
    _uint numRTVs = { 0 };

    for (auto& rtv : m_vecMultiRenderTargets[iIndex])
    {
        if (numRTVs >= 8)
        {
            MSG_ON((L"MRT에 등록된 RTV가 8개가 넘습니다"), L"이럼 좆된거임");
            BREAK;
            return E_FAIL;
        }

        if (_Clear)
        {
            rtv->ClearRTV();
        }
        
        RTVs[numRTVs++] = rtv->Get_RTV();
    }

    if (numRTVs >= 8)
        numRTVs = 8;

    if (_DSV != nullptr && _Clear)
    {
        m_pContext->ClearDepthStencilView(_DSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
    }

    m_pContext->OMSetRenderTargets(numRTVs, RTVs, _DSV);

    return S_OK;
}

HRESULT Engine::RenderTargetManager::Begin_RT(RenderTargets _targetName, ID3D11DepthStencilView* _DSV)
{
    // 원본 렌더타겟인 백버퍼 RTV, DSV를 저장
    m_pContext->OMGetRenderTargets(1, &m_pBackBufferRTV, &m_pBackBufferDSV);

    // VS와 PS의 SRV를 0 ~ 127번까지 nullptr로 채워준다
    m_pContext->VSSetShaderResources(0, 128, m_pNullSRVs);
    m_pContext->PSSetShaderResources(0, 128, m_pNullSRVs);

    ID3D11RenderTargetView* RTVs[8] = { nullptr };
    _uint numRTVs = { 1 };

    m_vecRenderTargets[_UINT(_targetName)]->ClearRTV();
    ID3D11RenderTargetView* rtv = m_vecRenderTargets[_UINT(_targetName)]->Get_RTV();
    RTVs[0] = rtv;

    if (_DSV != nullptr)
    {
        m_pContext->ClearDepthStencilView(_DSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
    }

    m_pContext->OMSetRenderTargets(numRTVs, RTVs, _DSV == nullptr ? m_pBackBufferDSV : _DSV);

    return S_OK;
}

HRESULT Engine::RenderTargetManager::Begin_RT_HDR(RenderTargets _targetName, ID3D11DepthStencilView* _DSV, _bool _Clear)
{
    // 원본 렌더타겟인 백버퍼 RTV, DSV를 저장
    m_pContext->OMGetRenderTargets(1, &m_pBackBufferRTV, &m_pBackBufferDSV);

    // VS와 PS의 SRV를 0 ~ 127번까지 nullptr로 채워준다
    m_pContext->VSSetShaderResources(0, 128, m_pNullSRVs);
    m_pContext->PSSetShaderResources(0, 128, m_pNullSRVs);

    ID3D11RenderTargetView* RTVs[8] = { nullptr };
    _uint numRTVs = { 1 };

    if(_Clear)
        m_vecRenderTargets[_UINT(_targetName)]->ClearRTV();
    
    ID3D11RenderTargetView* rtv = m_vecRenderTargets[_UINT(_targetName)]->Get_RTV();
    RTVs[0] = rtv;

    if (_DSV != nullptr)
    {
        m_pContext->ClearDepthStencilView(_DSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
    }

    m_pContext->OMSetRenderTargets(numRTVs, RTVs, _DSV == nullptr ? m_pBackBufferDSV : _DSV);

    return S_OK;
}

ID3D11DepthStencilView** Engine::RenderTargetManager::Begin_MRT_ShadowCascade(MRT _mrtName, vector<GameObject*> m_vecObjects[], _float _fTimeDelta, _int iNumDSV, RENDER_GROUP _eRGroup)
{
    _uint iIndex = _UINT(_mrtName);

    if (iIndex >= m_vecMultiRenderTargets.size() || m_vecMultiRenderTargets[iIndex].empty())
    {
        MSG_ON((L"교체하려는 MRT가 비어있거나 존재하지 않습니다"), L"RTV 교체 실패!!!");
        BREAK;
        return nullptr;
    }

    // 원본 렌더타겟인 백버퍼 RTV, DSV를 저장
    m_pContext->OMGetRenderTargets(1, &m_pBackBufferRTV, &m_pBackBufferDSV);

    // VS와 PS의 SRV를 0 ~ 127번까지 nullptr로 채워준다
    m_pContext->VSSetShaderResources(0, 128, m_pNullSRVs);
    m_pContext->PSSetShaderResources(0, 128, m_pNullSRVs);

    _uint numDSVs = { 0 };
    memset(m_pDSVShadows, 0, sizeof(m_pDSVShadows));
    for (auto& rtv : m_vecMultiRenderTargets[iIndex])
    {
        if (numDSVs >= 8)
        {
            MSG_ON((L"등록된 RTV가 8개가 넘습니다"), L"이럼 좆된거임");
            BREAK;
            return nullptr;
        }

        m_pDSVShadows[numDSVs++] = rtv->Get_DSV();
    }

    if (numDSVs >= 8)
        numDSVs = 8;
    if (m_vecObjects == nullptr)
        return nullptr;

    return m_pDSVShadows;
}

HRESULT Engine::RenderTargetManager::End_MRT()
{
    한번이라도END_MRT호출했는가 = true;

    // VS와 PS의 SRV를 0 ~ 127번까지 nullptr로 채워준다
    m_pContext->VSSetShaderResources(0, 128, m_pNullSRVs);
    m_pContext->PSSetShaderResources(0, 128, m_pNullSRVs);

    ID3D11RenderTargetView* pRenderTargets[8] = {
        m_pBackBufferRTV
    };

    // 렌더타겟을 0 ~ 7번까지 기존에 백버퍼로 다시 채워준다 ( 뎁스 스텐실 뷰도 같이 해줘야함 )
    m_pContext->OMSetRenderTargets(8, pRenderTargets, m_pBackBufferDSV);

    Safe_Release(m_pBackBufferRTV);
    Safe_Release(m_pBackBufferDSV);

    return S_OK;
}

HRESULT Engine::RenderTargetManager::Reset_MRT()
{
    m_pContext->VSSetShaderResources(0, 128, m_pNullSRVs);
    m_pContext->PSSetShaderResources(0, 128, m_pNullSRVs);

    ID3D11RenderTargetView* pRenderTargets[8] = {
        m_pBackBufferRTV
    };

    m_pContext->OMSetRenderTargets(8, pRenderTargets, m_pBackBufferDSV);

    return S_OK;
}
/******************************************************* MultiRenderTarget 관리 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더타겟 크기 변경 함수 ////////////////////////////////////////////////////////
HRESULT Engine::RenderTargetManager::ResizeBuffers(_float _width, _float _height)
{
    // [Modified] Map Iteration -> Vector Iteration
    for (auto& pRT : m_vecRenderTargets)
    {
        if (pRT != nullptr)
        {
            CHECK_FAILED(pRT->ResizeBuffer(_width, _height), E_FAIL);
        }
    }

    return S_OK;
}
/******************************************************* 렌더타겟 크기 변경 함수 *******************************************************/



//////////////////////////////////////////////////////// Texture2D 복사를 위한 함수 ////////////////////////////////////////////////////////
void Engine::RenderTargetManager::Copy_Resource(RenderTargets _rtvName, ID3D11Resource* pDest)
{
    if (_UINT(_rtvName) < m_vecRenderTargets.size() && m_vecRenderTargets[_UINT(_rtvName)] != nullptr)
    {
        m_vecRenderTargets[_UINT(_rtvName)]->Copy_Resource(pDest);
    }
}

void Engine::RenderTargetManager::Clear_RTV(MRT _mrtName)
{
    _uint iIndex = _UINT(_mrtName);

    for (auto& rtv : m_vecMultiRenderTargets[iIndex])
    {
        rtv->ClearRTV();
    }
}
/******************************************************* Texture2D 복사를 위한 함수 *******************************************************/



//////////////////////////////////////////////////////// 디버그용 렌더 함수 ////////////////////////////////////////////////////////
#ifdef _DEBUG
HRESULT Engine::RenderTargetManager::Ready_Debug(RenderTargets _RTName, _float _fX, _float _fY, _float _sizeX, _float _sizeY)
{
    if (_UINT(_RTName) >= m_vecRenderTargets.size() || m_vecRenderTargets[_UINT(_RTName)] == nullptr)
    {
        MSG_ON((L"렌더타겟의 디버그창 생성에 실패했습니다"), L"생성 실패!!!");
        BREAK;
        return E_FAIL;
    }

    m_vecRenderTargets[_UINT(_RTName)]->Ready_Debug(_fX, _fY, _sizeX, _sizeY);

    return S_OK;
}

HRESULT Engine::RenderTargetManager::Render(MRT _MRTName, Shader* _shader, VIBuffer_Rect* _buffer, const _float fTimeDelta)
{
    if (_UINT(_MRTName) >= m_vecMultiRenderTargets.size())
    {
        MSG_ON((L"렌더를 위한 MRT가 존재하지 않습니다"), L"렌더링 실패!");
        BREAK;
        return E_FAIL;
    }

    for (auto& RenderTarget : m_vecMultiRenderTargets[_UINT(_MRTName)])
    {
        if (RenderTarget != nullptr)
            RenderTarget->Render(_shader, _buffer, fTimeDelta);
    }

    return S_OK;
}

HRESULT Engine::RenderTargetManager::Render(RenderTargets _targetName, Shader* _shader, VIBuffer_Rect* _buffer, const _float fTimeDelta)
{
    m_vecRenderTargets[_UINT(_targetName)]->Render(_shader, _buffer, fTimeDelta);

    return S_OK;
}
#endif
/******************************************************* 디버그용 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::RenderTargetManager::Update_Priority(const _float fTimeDelta)
{
    return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::RenderTargetManager::Update(const _float fTimeDelta)
{
    return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::RenderTargetManager::Update_Late(const _float fTimeDelta)
{
    return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Engine::RenderTargetManager::Render(const _float fTimeDelta)
{
    return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// Getter 함수 ////////////////////////////////////////////////////////
ID3D11ShaderResourceView* Engine::RenderTargetManager::Get_ShaderResourceView(RenderTargets _target)
{
    return m_vecRenderTargets[_UINT(_target)]->Get_SRV();
}

ID3D11RenderTargetView* Engine::RenderTargetManager::Get_RenderTargetView(RenderTargets _target)
{
    return m_vecRenderTargets[_UINT(_target)]->Get_RTV();
}
/******************************************************* Getter 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
RenderTargetManager* Engine::RenderTargetManager::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    RenderTargetManager* pInstance = new RenderTargetManager(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize(), L"RenderTargetManager 생성 실패", L"Caution!!!", nullptr);

    return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::RenderTargetManager::Free()
{
    __super::Free();

    if (한번이라도END_MRT호출했는가 == false)
    {
        Safe_Release(m_pBackBufferRTV);
        Safe_Release(m_pBackBufferDSV);
    }

    // Map Loop -> Vector Loop
    for (auto& vecRTV : m_vecMultiRenderTargets)
    {
        for (auto& rtv : vecRTV)
        {
            Safe_Release(rtv); // MRT 추가할 때 AddRef 했으므로 여기서 Release
        }
        vecRTV.clear();
    }
    m_vecMultiRenderTargets.clear();

    for (auto& rtv : m_vecRenderTargets)
    {
        Safe_Release(rtv);
    }
    m_vecRenderTargets.clear();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pGameInstance);
}
/******************************************************* 객체 반환 함수 *******************************************************/