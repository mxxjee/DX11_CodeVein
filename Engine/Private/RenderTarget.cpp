#include "Engine_Define.h"
#include "RenderTarget.h"

//#include "GameInstance.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::RenderTarget::RenderTarget()
{
}

Engine::RenderTarget::RenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice(pDevice), m_pContext(pContext)//, m_pGameInstance(GameInstance::GetInstance())
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    //Safe_AddRef(m_pGameInstance);
}

Engine::RenderTarget::~RenderTarget()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::RenderTarget::Initialize(_float _width, _float _height, DXGI_FORMAT _textureformat, const _float4& _clearcolor)
{
    m_vClearColor = _clearcolor;

    D3D11_TEXTURE2D_DESC texturedesc{};
    texturedesc.Width = (UINT)_width;
    texturedesc.Height = (UINT)_height;
    texturedesc.MipLevels = 1;
    texturedesc.ArraySize = 1;
    texturedesc.Format = m_Format = _textureformat;    // 렌더타겟의 픽셀당 크기(DXGI_FORMAT_R16G16B16A16_UNORM)

    texturedesc.SampleDesc.Quality = 0;
    texturedesc.SampleDesc.Count = 1;

    texturedesc.Usage = D3D11_USAGE_DEFAULT;
    texturedesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

    texturedesc.CPUAccessFlags = 0;
    texturedesc.MiscFlags = 0;

    MSG_FAIL(m_pDevice->CreateTexture2D(&texturedesc, nullptr, &m_pTexture2D), L"렌더타겟의 Texture2D생성에 실패했습니다.", L"경고!!!", E_FAIL);
    MSG_FAIL(m_pDevice->CreateRenderTargetView(m_pTexture2D, nullptr, &m_pRTV), L"렌더타겟의 RTV생성에 실패했습니다.", L"경고!!!", E_FAIL);
    MSG_FAIL(m_pDevice->CreateShaderResourceView(m_pTexture2D, nullptr, &m_pSRV), L"렌더타겟의 SRV생성에 실패했습니다.", L"경고!!!", E_FAIL);

    return S_OK;
}

HRESULT Engine::RenderTarget::Initialize_Shadow(_float _width, _float _height, DXGI_FORMAT _textureformat, const _float4& _clearcolor)
{
    m_vClearColor = _clearcolor;

    D3D11_TEXTURE2D_DESC texturedesc{};
    texturedesc.Width = (UINT)_width;
    texturedesc.Height = (UINT)_height;
    texturedesc.MipLevels = 1;
    texturedesc.ArraySize = 1;
    texturedesc.Format = m_Format = _textureformat;    // 렌더타겟의 픽셀당 크기(DXGI_FORMAT_R16G16B16A16_UNORM)

    texturedesc.SampleDesc.Quality = 0;
    texturedesc.SampleDesc.Count = 1;

    texturedesc.Usage = D3D11_USAGE_DEFAULT;
    texturedesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;

    texturedesc.CPUAccessFlags = 0;
    texturedesc.MiscFlags = 0;

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0;

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Texture2D.MostDetailedMip = 0;

    MSG_FAIL(m_pDevice->CreateTexture2D(&texturedesc, nullptr, &m_pTexture2D), L"렌더타겟의 Texture2D생성에 실패했습니다.", L"경고!!!", E_FAIL);
    MSG_FAIL(m_pDevice->CreateDepthStencilView(m_pTexture2D, &dsvDesc, &m_pDSV), L"렌더타겟의 DSV생성에 실패했습니다.", L"경고!!!", E_FAIL);
    MSG_FAIL(m_pDevice->CreateShaderResourceView(m_pTexture2D, &srvDesc, &m_pSRV), L"렌더타겟의 SRV생성에 실패했습니다.", L"경고!!!", E_FAIL);

    return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 쉐이더 바인딩 함수 ////////////////////////////////////////////////////////
HRESULT Engine::RenderTarget::Bind_ShaderResource(Shader* _shader, const _string& _constName)
{
    return _shader->Bind_SRV(_constName, m_pSRV);
}

HRESULT Engine::RenderTarget::Bind_ShaderResource_FullSlot(Shader* _shader, _uint _slotNum, _uint _stageMask)
{
    return _shader->Bind_SRV_FullSlot(_slotNum, m_pSRV, _stageMask);
}
/******************************************************* 쉐이더 바인딩 함수 *******************************************************/



//////////////////////////////////////////////////////// RTV 초기화 함수 ////////////////////////////////////////////////////////
void Engine::RenderTarget::ClearRTV()
{
    m_pContext->ClearRenderTargetView(m_pRTV, reinterpret_cast<_float*>(&m_vClearColor));
}
/******************************************************* RTV 초기화 함수 *******************************************************/

//////////////////////////////////////////////////////// Texture2D 복사 함수 ////////////////////////////////////////////////////////
void Engine::RenderTarget::Copy_Resource(ID3D11Resource* pDest)
{
    if (nullptr == m_pTexture2D)
        return;
    m_pContext->CopyResource(pDest, m_pTexture2D);
}
/******************************************************* Texture2D 복사 함수 *******************************************************/

//////////////////////////////////////////////////////// 렌더타겟 사이즈 변경 함수 ////////////////////////////////////////////////////////
HRESULT Engine::RenderTarget::ResizeBuffer(_float _width, _float _height)
{
    _float fOffset = 1.f;
    //if(m_vClearColor.x == 1.f && m_vClearColor.y == 1.f && m_vClearColor.z == 1.f)
    //    fOffset = 10.f;

    // 기존 리소스 해제
    Safe_Release(m_pRTV);
    Safe_Release(m_pSRV);
    Safe_Release(m_pTexture2D);

    // 기존 포맷, 클리어 컬러로 재생성
    D3D11_TEXTURE2D_DESC texturedesc{};
    texturedesc.Width = (UINT)(_width * fOffset);
    texturedesc.Height = (UINT)(_height * fOffset);
    texturedesc.MipLevels = 1;
    texturedesc.ArraySize = 1;
    texturedesc.Format = m_Format;

    texturedesc.SampleDesc.Quality = 0;
    texturedesc.SampleDesc.Count = 1;

    texturedesc.Usage = D3D11_USAGE_DEFAULT;
    texturedesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    texturedesc.CPUAccessFlags = 0;
    texturedesc.MiscFlags = 0;

    CHECK_FAILED(m_pDevice->CreateTexture2D(&texturedesc, nullptr, &m_pTexture2D), E_FAIL);
    CHECK_FAILED(m_pDevice->CreateRenderTargetView(m_pTexture2D, nullptr, &m_pRTV), E_FAIL);
    CHECK_FAILED(m_pDevice->CreateShaderResourceView(m_pTexture2D, nullptr, &m_pSRV), E_FAIL);


    return S_OK;
}
/******************************************************* 렌더타겟 사이즈 변경 함수 *******************************************************/


#ifdef _DEBUG
//////////////////////////////////////////////////////// 디버그용 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Engine::RenderTarget::Ready_Debug(_float _fX, _float _fY, _float _sizeX, _float _sizeY)
{
    _uint iNumViewPorts = { 1 };
    D3D11_VIEWPORT desc{};

    m_pContext->RSGetViewports(&iNumViewPorts, &desc);

    XMStoreFloat4x4(&m_WorldMatrix,
        XMMatrixScaling(_sizeX, _sizeY, 1.f) *
        XMMatrixTranslation(_fX - desc.Width * 0.5f, -_fY + desc.Height * 0.5f, 0.f));
    
    return S_OK;
}

HRESULT Engine::RenderTarget::Render(Shader* _shader, VIBuffer_Rect* _buffer, const _float fTimeDelta)
{
    HRESULT hr{};
    _shader->Bind_Matrix_FullSlot(1, "g_WorldMatrix", m_WorldMatrix);
    _shader->Bind_SRV("g_DebugTexture", m_pSRV);
    _shader->Begin(0);
    _shader->Bind_Resources(0);
    _buffer->Render(fTimeDelta);

    return S_OK;
}
/******************************************************* 디버그용 렌더 함수 *******************************************************/
#endif


//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
RenderTarget* Engine::RenderTarget::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _float _width, _float _height, DXGI_FORMAT _textureformat, const _float4& _clearcolor)
{
    RenderTarget* pInstance = new RenderTarget(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize(_width, _height, _textureformat, _clearcolor), L"RenderTarget 생성 실패", L"Caution!!!", nullptr);

    return pInstance;
}

RenderTarget* Engine::RenderTarget::Create_Shadow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _float _width, _float _height, DXGI_FORMAT _textureformat, const _float4& _clearcolor)
{
    RenderTarget* pInstance = new RenderTarget(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Shadow(_width, _height, _textureformat, _clearcolor), L"RenderTarget 생성 실패", L"Caution!!!", nullptr);

    return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::RenderTarget::Free()
{
    __super::Free();

    Safe_Release(m_pRTV);
    Safe_Release(m_pSRV);
    Safe_Release(m_pDSV);
    Safe_Release(m_pTexture2D);

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    //Safe_Release(m_pGameInstance);
}
/******************************************************* 객체 반환 함수 *******************************************************/

