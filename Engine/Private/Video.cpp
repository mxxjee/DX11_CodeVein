#include "Engine_Define.h"
#include "Video.h"

// Media Foundation 헤더
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <mferror.h>

#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Engine::Video::Video()
{
}

Engine::Video::Video(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : Component(pDevice, pContext)
{
}

Engine::Video::Video(const Video& original)
    : Component(original)
    , m_strPath(original.m_strPath)
    , m_pBatch(original.m_pBatch)
{
}

Engine::Video::~Video()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Engine::Video::Initialize_Prototype(const _wstring& _videoPath)
{
    // Media Foundation 초기화
    MSG_FAIL(MFStartup(MF_VERSION), L"Media Foundation 초기화에 실패했습니다.", L"생성 실패", E_FAIL);

    m_strPath = _videoPath;
    m_pBatch = new SpriteBatch(m_pContext);

    return S_OK;
}

HRESULT Engine::Video::Initialize(void* _arg)
{
    // SourceReader 생성
    MSG_FAIL(Create_SourceReader(m_strPath), L"SourceReader 생성에 실패했습니다.", L"생성 실패", E_FAIL);

    // 텍스처 생성
    MSG_FAIL(Create_Texture(), L"Video 텍스처 생성에 실패했습니다.", L"생성 실패", E_FAIL);

    Set_FullScreen(720, 410);

    return S_OK;
}

void Engine::Video::Set_FitHeight(_uint _winSizeX, _uint _winSizeY)
{
    // 세로를 윈도우 높이에 맞추고 비율 유지
    _float aspectRatio = (_float)m_iWidth / (_float)m_iHeight;

    _float newHeight = (_float)_winSizeY;
    _float newWidth = newHeight * aspectRatio;

    // 가로 중앙 정렬 (윈도우보다 크면 음수가 됨 = 양옆 잘림)
    _float posX = ((_float)_winSizeX - newWidth) * 0.5f;

    m_vPosition = { posX, 0.f };
    m_vSize = { newWidth, newHeight };
}

HRESULT Engine::Video::Create_SourceReader(const _wstring& _videoPath)
{
    // SourceReader 생성
    IMFAttributes* pAttributes = nullptr;
    MFCreateAttributes(&pAttributes, 1);
    pAttributes->SetUINT32(MF_SOURCE_READER_ENABLE_VIDEO_PROCESSING, TRUE);

    HRESULT hr = MFCreateSourceReaderFromURL(_videoPath.c_str(), pAttributes, &m_pSourceReader);
    Safe_Release(pAttributes);

    if (FAILED(hr))
        return E_FAIL;

    // 출력 포맷을 RGB32로 설정
    IMFMediaType* pMediaType = nullptr;
    MFCreateMediaType(&pMediaType);
    pMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
    pMediaType->SetGUID(MF_MT_SUBTYPE, MFVideoFormat_RGB32);

    hr = m_pSourceReader->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, nullptr, pMediaType);
    Safe_Release(pMediaType);

    if (FAILED(hr))
        return E_FAIL;

    // 비디오 정보 가져오기
    IMFMediaType* pCurrentType = nullptr;
    m_pSourceReader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM, &pCurrentType);

    if (pCurrentType)
    {
        // 해상도 가져오기
        UINT32 width = 0, height = 0;
        MFGetAttributeSize(pCurrentType, MF_MT_FRAME_SIZE, &width, &height);
        m_iWidth = width;
        m_iHeight = height;

        // FPS 가져오기
        UINT32 numerator = 0, denominator = 1;
        MFGetAttributeRatio(pCurrentType, MF_MT_FRAME_RATE, &numerator, &denominator);
        m_fFPS = (_float)numerator / (_float)denominator;
        m_fFrameTime = 1.f / m_fFPS;

        Safe_Release(pCurrentType);
    }

    return S_OK;
}

HRESULT Engine::Video::Create_Texture()
{
    D3D11_TEXTURE2D_DESC textureDesc = {};
    textureDesc.Width = m_iWidth;
    textureDesc.Height = m_iHeight;
    textureDesc.MipLevels = 1;
    textureDesc.ArraySize = 1;
    textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;  // RGB32는 실제로 BGRA
    textureDesc.SampleDesc.Count = 1;
    textureDesc.Usage = D3D11_USAGE_DYNAMIC;
    textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    MSG_FAIL(m_pDevice->CreateTexture2D(&textureDesc, nullptr, &m_pTexture), L"Video 텍스처 생성 실패", L"생성 실패", E_FAIL);
    MSG_FAIL(m_pDevice->CreateShaderResourceView(m_pTexture, nullptr, &m_pSRV), L"Video SRV 생성 실패", L"생성 실패", E_FAIL);

    return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Engine::Video::Update(const _float fTimeDelta)
{
    if (!m_bIsPlaying || m_bIsFinished)
        return 0;

    m_fElapsedTime += fTimeDelta;

    // 프레임 타이밍 체크
    if (m_fElapsedTime >= m_fFrameTime)
    {
        m_fElapsedTime -= m_fFrameTime;

        if (FAILED(Read_Frame()))
        {
            if (m_bIsLooping)
            {
                // 처음으로 되돌리기
                PROPVARIANT var;
                PropVariantInit(&var);
                var.vt = VT_I8;
                var.hVal.QuadPart = 0;
                m_pSourceReader->SetCurrentPosition(GUID_NULL, var);
                PropVariantClear(&var);

                Read_Frame();
            }
            else
            {
                m_bIsFinished = true;
                m_bIsPlaying = false;
            }
        }
    }

    return 0;
}

HRESULT Engine::Video::Read_Frame()
{
    if (!m_pSourceReader)
        return E_FAIL;

    DWORD streamIndex = 0, flags = 0;
    LONGLONG timestamp = 0;
    IMFSample* pSample = nullptr;

    HRESULT hr = m_pSourceReader->ReadSample(
        (DWORD)MF_SOURCE_READER_FIRST_VIDEO_STREAM,
        0,
        &streamIndex,
        &flags,
        &timestamp,
        &pSample
    );

    if (FAILED(hr) || (flags & MF_SOURCE_READERF_ENDOFSTREAM))
    {
        Safe_Release(pSample);
        return E_FAIL;
    }

    if (pSample)
    {
        IMFMediaBuffer* pBuffer = nullptr;
        pSample->ConvertToContiguousBuffer(&pBuffer);

        if (pBuffer)
        {
            BYTE* pData = nullptr;
            DWORD maxLength = 0, currentLength = 0;
            pBuffer->Lock(&pData, &maxLength, &currentLength);

            D3D11_MAPPED_SUBRESOURCE mapped = {};
            if (SUCCEEDED(m_pContext->Map(m_pTexture, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped)))
            {
                BYTE* dest = (BYTE*)mapped.pData;
                BYTE* src = pData;

                for (_uint row = 0; row < m_iHeight; ++row)
                {
                    DWORD* destPixel = (DWORD*)dest;
                    DWORD* srcPixel = (DWORD*)src;

                    for (_uint col = 0; col < m_iWidth; ++col)
                    {
                        destPixel[col] = srcPixel[col] | 0xFF000000;  // 알파 비트 강제 설정
                    }

                    dest += mapped.RowPitch;
                    src += m_iWidth * 4;
                }

                m_pContext->Unmap(m_pTexture, 0);
            }

            pBuffer->Unlock();
            Safe_Release(pBuffer);
        }

        Safe_Release(pSample);
    }

    return S_OK;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
void Engine::Video::Begin()
{
    m_pContext->OMGetBlendState(&m_pOldBlendState, m_fOldBlendFactor, &m_iOldSampleMask);
    m_pContext->OMGetDepthStencilState(&m_pOldDepthState, &m_iOldStencilRef);
    m_pContext->RSGetState(&m_pOldRasterState);

    m_pBatch->Begin();
}

void Engine::Video::Render()
{
    if (!m_pSRV || !m_pBatch)
        return;

    Begin();

    _float width = (m_vSize.x > 0.f) ? m_vSize.x : (_float)m_iWidth;
    _float height = (m_vSize.y > 0.f) ? m_vSize.y : (_float)m_iHeight;

    RECT destRect = {
        (LONG)m_vPosition.x,
        (LONG)m_vPosition.y,
        (LONG)(m_vPosition.x + width),
        (LONG)(m_vPosition.y + height)
    };

    m_pBatch->Draw(m_pSRV, destRect);

    End();
}

void Engine::Video::End()
{
    m_pBatch->End();

    m_pContext->OMSetBlendState(m_pOldBlendState, m_fOldBlendFactor, m_iOldSampleMask);
    m_pContext->OMSetDepthStencilState(m_pOldDepthState, m_iOldStencilRef);
    m_pContext->RSSetState(m_pOldRasterState);

    Safe_Release(m_pOldBlendState);
    Safe_Release(m_pOldDepthState);
    Safe_Release(m_pOldRasterState);
}
/******************************************************* 렌더 함수 *******************************************************/



//////////////////////////////////////////////////////// 재생 제어 함수 ////////////////////////////////////////////////////////
void Engine::Video::Play()
{
    m_bIsPlaying = true;
    m_bIsFinished = false;
}

void Engine::Video::Pause()
{
    m_bIsPlaying = false;
}

void Engine::Video::Stop()
{
    m_bIsPlaying = false;
    m_bIsFinished = false;
    m_fElapsedTime = 0.f;

    // 처음으로 되돌리기
    if (m_pSourceReader)
    {
        PROPVARIANT var;
        PropVariantInit(&var);
        var.vt = VT_I8;
        var.hVal.QuadPart = 0;
        m_pSourceReader->SetCurrentPosition(GUID_NULL, var);
        PropVariantClear(&var);
    }
}
/******************************************************* 재생 제어 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Video* Engine::Video::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& _videoPath)
{
    Video* pInstance = new Video(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(_videoPath), L"Video 프로토타입 생성 실패!", L"생성 실패", nullptr);

    return pInstance;
}

Component* Engine::Video::Clone(void* _arg)
{
    Video* pInstance = new Video(*this);

    MSG_FAIL(pInstance->Initialize(_arg), L"Video 복사 실패!", L"생성 실패", nullptr);

    return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Engine::Video::Free()
{
    __super::Free();

    Safe_Release(m_pSRV);
    Safe_Release(m_pTexture);
    Safe_Release(m_pSourceReader);

    if (m_bIsClone == false)
    {
        Safe_Delete(m_pBatch);
        MFShutdown();  // Media Foundation 종료
    }
}
/******************************************************* 객체 반환 함수 *******************************************************/
