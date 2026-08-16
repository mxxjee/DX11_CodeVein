#include "Engine_Define.h"
#include "Graphic_Device.h"


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::Graphic_Device::Graphic_Device()
{
}

Engine::Graphic_Device::~Graphic_Device()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 이니셜라이즈 ////////////////////////////////////////////////////////
HRESULT Engine::Graphic_Device::Initialize(HWND _hWnd, WINMODE _isWindowed, _uint _wincx, _uint _wincy, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext)
{
    _uint iFlag = 0;

#ifdef _DEBUG
   // iFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif // DEBUG

    D3D_FEATURE_LEVEL FeatureLV{};

    /* dx9 : 장치 초기화를 하기위한 설정을 쭈욱 하고나서 최종적으로 장치객체를 생성한다. */
    /* dx11 : 우선적으로 장치 객체를 생성하고 장치객체를 통해서 기타 초기화작업 및 설정을 해나간다. */

    /* 그래픽 장치를 초기화한다. */
    MSG_FAIL(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, iFlag, nullptr, 0, D3D11_SDK_VERSION, &m_pDevice, &FeatureLV, &m_pContext),
        L"Graphic Device Create Failed!!", L"Caution!!!", E_FAIL);

    if (FeatureLV < D3D_FEATURE_LEVEL_11_0)
    {
        MSG_ON(L"FeatureLv이 목표값보다 낮습니다", L"경고!!!");
        BREAK;
    }

    /* SwapChain : 더블버퍼링. 전면과 후면버퍼를 번갈아가며 화면에 보여준다.(Present) */

    /* 스왑체인객체를 생성하였고 생성한 스왑체인 객체가 백버퍼를 내장한다. 백버퍼를 생성하기 위한 ID3D11Texture2D 만든거야. */
    /* 스왑체인 객체를 만들면서 백버퍼에 해당하는 ID3D11Texture2D객체를 만들어 스왑체인 객체가 내장한다. */
    MSG_FAIL(Ready_SwapChain(_hWnd, _isWindowed, _wincx, _wincy), L"스왑체인 생성에 실패했습니다!", L"경고!", E_FAIL);

    MSG_FAIL(Ready_BackBufferRenderTargetView(), L"백버퍼 생성에 실패했습니다!", L"경고!", E_FAIL);

    MSG_FAIL(Ready_DepthStencilView(_wincx, _wincy), L"깊이 스텐실뷰 생성에 실패했습니다!", L"경고!", E_FAIL);

    /* 장치에 바인드해놓을 렌더 타겟들과 뎁스스텐실뷰를 세팅한다. */
    /* 장치는 동시에 최대 4->8개의 렌더타겟을 들고 있을 수 있다. */
    ID3D11RenderTargetView* pRTVs[] = {
        m_pBackBufferRTV,
    };

    /* 렌더타겟의 픽셀 수와 깊이스텐실버퍼의 픽셀수가 서로 다르다면 절대 렌더링이 불가능해진다. */
    m_pContext->OMSetRenderTargets(1, pRTVs, m_pDepthStencilView);

    D3D11_VIEWPORT ViewPortDesc{};

    ViewPortDesc.TopLeftX = 0;
    ViewPortDesc.TopLeftY = 0;
    ViewPortDesc.Width	= (_float)_wincx;
    ViewPortDesc.Height = (_float)_wincy;
    ViewPortDesc.MinDepth = 0.f;
    ViewPortDesc.MaxDepth = 1.f;

    m_pContext->RSSetViewports(1, &ViewPortDesc);

    *ppDevice = m_pDevice;
    *ppContext = m_pContext;

    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);

    return S_OK;
}
/******************************************************* 이니셜라이즈 *******************************************************/



HRESULT Engine::Graphic_Device::Ready_SwapChain(HWND _hWnd, WINMODE _isWindowed, _uint _wincx, _uint _wincy)
{
    IDXGIDevice*			pDevice = nullptr;
    m_pDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDevice);

    IDXGIAdapter*			pAdapter = nullptr;
    pDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&pAdapter);

    IDXGIFactory*			pFactory = nullptr;
    pAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pFactory);

    /* 스왑체인을 생성한다. = 텍스쳐를 생성하는 행위 + 스왑하는 형태  */
    DXGI_SWAP_CHAIN_DESC SwapChain{};
    ZeroMemory(&SwapChain, sizeof(DXGI_SWAP_CHAIN_DESC));

    /* 백버퍼 == 텍스쳐 */
    /* 텍스처(백버퍼 == ID3D11Texture2D)를 생성하는 행위*/
    SwapChain.BufferDesc.Width = _wincx;
    SwapChain.BufferDesc.Height = _wincy;

    SwapChain.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;	/* D3DFMT_A8R8G8B8 */ /* 만든 픽셀하나의 데이터 정보 : 32BIT픽셀생성하되 부호가 없는 정규화된 수를 저장할께 */
    SwapChain.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE; // 기존 DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED
    SwapChain.BufferDesc.Scaling = DXGI_MODE_SCALING_STRETCHED; // 기존 DXGI_MODE_SCALING_UNSPECIFIED

    SwapChain.BufferDesc.RefreshRate.Numerator = 144; // 점멸 빈도
    SwapChain.BufferDesc.RefreshRate.Denominator = 1;

    SwapChain.BufferCount = 1;
    SwapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

    SwapChain.SampleDesc.Count = 1;
    SwapChain.SampleDesc.Quality = 0;

    SwapChain.OutputWindow = _hWnd;
    SwapChain.Windowed = static_cast<_bool>(_isWindowed);
    SwapChain.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;


    /* 백버퍼라는 텍스처(ID3D11Texture2D)를 생성했다. */
    HRESULT hr = (pFactory->CreateSwapChain(m_pDevice, &SwapChain, &m_pSwapChain));

    Safe_Release(pFactory);
    Safe_Release(pAdapter);
    Safe_Release(pDevice);

    if (FAILED(hr))
        return E_FAIL;

    return S_OK;
}

HRESULT Engine::Graphic_Device::Ready_BackBufferRenderTargetView()
{
    MSG_NULL(m_pDevice, L"디바이스가 존재하지 않습니다.", L"경고!!!", E_FAIL);

    /* 내가 앞으로 사용 하기위한 용도의 텍스쳐를 생성하기위한 베이스 데이터를 가지고 있는 객체이다. */
    /* 내가 앞으로 사용 하기위한 용도의 텍스쳐 : ID3D11RenderTargetView, ID3D11ShaderResoureView, ID3D11DepthStencilView */
    ID3D11Texture2D* pBackBufferTexture = nullptr;

    /* 스왑체인이 들고있던 텍스처를 가져와봐. */
    CHECK_FAILED(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBufferTexture), E_FAIL);

    /* 실제 렌더타겟용도로 사용할 수 있는 텍스쳐 타입(ID3D11RenderTargetView)의 객체를 생성한다. */
    CHECK_FAILED(m_pDevice->CreateRenderTargetView(pBackBufferTexture, nullptr, &m_pBackBufferRTV), E_FAIL);

    Safe_Release(pBackBufferTexture);

    return S_OK;
}

HRESULT Engine::Graphic_Device::Ready_DepthStencilView(_uint _wincx, _uint _wincy)
{
    MSG_NULL(m_pDevice, L"디바이스가 존재하지 않습니다.", L"경고!!!", E_FAIL);

    D3D11_TEXTURE2D_DESC TextureDesc{};

    /* 깊이 버퍼의 픽셀은 백버퍼의 픽셀과 갯수가 동일해야만 깊이 테스트가 가능해진다. */
    /* 픽셀의 수가 다르면 아에 렌더링을 못함. */
    TextureDesc.Width = _wincx;
    TextureDesc.Height = _wincy;
    TextureDesc.MipLevels = 1;
    TextureDesc.ArraySize = 1;
    TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

    TextureDesc.SampleDesc.Quality = 0;
    TextureDesc.SampleDesc.Count = 1;


    /* 동적? 정적?  */
    TextureDesc.Usage = D3D11_USAGE_DEFAULT /* 정적 */;
    /* 추후에 어떤 용도로 바인딩 될 수 있는 View타입의 텍스쳐를 만들기위한 Texture2D입니까? */
    TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    /*| D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE*/;
    TextureDesc.CPUAccessFlags = 0;
    TextureDesc.MiscFlags = 0;

    CHECK_FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pDepthStencilTexture), E_FAIL);

    CHECK_FAILED(m_pDevice->CreateDepthStencilView(m_pDepthStencilTexture, nullptr, &m_pDepthStencilView), E_FAIL);

    return S_OK;
}

HRESULT Engine::Graphic_Device::Clear_BackBufferView(const _float4* pClearColor)
{
    CHECK_NULLPTR(m_pContext);

    ID3D11RenderTargetView* pRTVs[] = {
        m_pBackBufferRTV,
    };
    m_pContext->OMSetRenderTargets(1, pRTVs, m_pDepthStencilView);

    /* DX9기준 : Clear함수는 백버퍼, 깊이스텐실버퍼를 한꺼번에 지운다.  */
    // m_pGraphic_Device->Clear(어떤 영역만큼 지울까, 어떤 것들을 지울까? , 뭘로 지울가. );	

    /* 백버퍼를 초기화한다.  */
    m_pContext->ClearRenderTargetView(m_pBackBufferRTV, reinterpret_cast<const _float*>(pClearColor));

    return S_OK;
}

HRESULT Engine::Graphic_Device::Clear_DepthStencilView()
{
    CHECK_NULLPTR(m_pContext);

    m_pContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

    return S_OK;
}

HRESULT Engine::Graphic_Device::Present()
{
    CHECK_NULLPTR(m_pSwapChain);

    /* 전면 버퍼와 후면 버퍼를 교체하여 후면 버퍼를 전면으로 보여주는 역할을 한다. */
    /* 후면 버퍼를 직접 화면에 보여줄게. */
#ifdef _DEBUG
    return m_pSwapChain->Present(0, 0);
#endif
    return m_pSwapChain->Present(1, 0);
}

HRESULT Engine::Graphic_Device::ResizeBuffers(_int _width, _int _height, D3D11_VIEWPORT& _vp)
{
    // 1. 기존 리소스 완전 해제
    Safe_Release(m_pBackBufferRTV);
    Safe_Release(m_pDepthStencilView);
    Safe_Release(m_pDepthStencilTexture);

    ID3D11RenderTargetView* nullRTVs[] = { nullptr };
    m_pContext->OMSetRenderTargets(1, nullRTVs, nullptr);

    // 2. SwapChain 리사이즈
    HRESULT hr = m_pSwapChain->ResizeBuffers(0, _width, _height, DXGI_FORMAT_UNKNOWN, 0);
    if (FAILED(hr)) return E_FAIL;

    // 3. 백버퍼 재생성
    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBuffer);
    if (FAILED(hr)) return E_FAIL;
    hr = m_pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &m_pBackBufferRTV);
    Safe_Release(pBackBuffer);
    if (FAILED(hr)) return E_FAIL;

    // 4. 깊이 스텐실 재생성
    D3D11_TEXTURE2D_DESC depthDesc = {};
    depthDesc.Width = _width;
    depthDesc.Height = _height;
    depthDesc.MipLevels = 1;
    depthDesc.ArraySize = 1;
    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.Usage = D3D11_USAGE_DEFAULT;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    hr = m_pDevice->CreateTexture2D(&depthDesc, nullptr, &m_pDepthStencilTexture);
    if (FAILED(hr)) return E_FAIL;

    hr = m_pDevice->CreateDepthStencilView(m_pDepthStencilTexture, nullptr, &m_pDepthStencilView);
    if (FAILED(hr)) return E_FAIL;

    // 5. 뷰포트 설정
    D3D11_VIEWPORT vp = {};
    vp.Width = (_float)_width;
    vp.Height = (_float)_height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    m_pContext->RSSetViewports(1, &vp);
    _vp = vp;

    // 6. RTV/DSV 바인딩 복구
    ID3D11RenderTargetView* pRTVs[] = { m_pBackBufferRTV };
    m_pContext->OMSetRenderTargets(1, pRTVs, m_pDepthStencilView);

    return S_OK;
}

HRESULT Engine::Graphic_Device::Swap_OriginalBackbuffer()
{
    ID3D11RenderTargetView* pRenderTargets[8] = {
        m_pBackBufferRTV
    };

    m_pContext->OMSetRenderTargets(8, pRenderTargets, m_pDepthStencilView);

    return S_OK;
}


Graphic_Device* Engine::Graphic_Device::Create(HWND _hWnd, WINMODE _isWindowed, _uint _wincx, _uint _wincy, ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext)
{
    Graphic_Device* pInstance = new Graphic_Device();

    if (FAILED(pInstance->Initialize(_hWnd, _isWindowed, _wincx, _wincy, ppDevice, ppContext)))
    {
        MSG_ON(L"그래픽 디바이스 생성 실패!", L"경고!!!");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void Engine::Graphic_Device::Free()
{
    __super::Free();

    Safe_Release(m_pBackBufferRTV);
    Safe_Release(m_pDepthStencilView);
    Safe_Release(m_pDepthStencilTexture);

    Safe_Release(m_pContext);


#if defined(DEBUG) || defined(_DEBUG)
    ID3D11Debug* d3dDebug = { nullptr };
    HRESULT hr = m_pDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&d3dDebug));
    if (SUCCEEDED(hr))
    {
        OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
        OutputDebugStringW(L"                                                                    D3D11 Live Object ref Count Checker \r ");
        OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");

        hr = d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);

        OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
        OutputDebugStringW(L"                                                                    D3D11 Live Object ref Count Checker END \r ");
        OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
    }
    if (d3dDebug != nullptr)            d3dDebug->Release();
#endif

    Safe_Release(m_pSwapChain);

    Safe_Release(m_pDevice);

}
