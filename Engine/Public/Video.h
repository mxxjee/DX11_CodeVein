#pragma once
#include "Component.h"

// Media Foundation 전방 선언
struct IMFSourceReader;

NS_BEGIN(Engine)

class ENGINE_DLL Video final : public Component
{
private:
    explicit Video();
    explicit Video(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit Video(const Video& original);
    virtual ~Video();

public:
    HRESULT Initialize_Prototype(const _wstring& _videoPath);
    HRESULT Initialize(void* _arg);
    _int Update(const _float fTimeDelta) override;

    void Begin();
    void Render();
    void End();

    void Play();
    void Pause();
    void Stop();
    void Set_Loop(_bool _loop) { m_bIsLooping = _loop; }

    ID3D11ShaderResourceView* Get_SRV() const { return m_pSRV; }
    void Set_Position(_float2 _pos) { m_vPosition = _pos; }
    void Set_Size(_float2 _size) { m_vSize = _size; }
    void Set_FullScreen(_uint _winSizeX, _uint _winSizeY) {
        m_vPosition = { 0.f, 0.f };
        m_vSize = { (_float)_winSizeX, (_float)_winSizeY };
    }
    _bool Is_Playing() const { return m_bIsPlaying; }
    _bool Is_Finished() const { return m_bIsFinished; }

    void Set_FitHeight(_uint _winSizeX, _uint _winSizeY);

private:
    HRESULT Create_SourceReader(const _wstring& _videoPath);
    HRESULT Create_Texture();
    HRESULT Read_Frame();

private:
    // Media Foundation
    IMFSourceReader* m_pSourceReader = { nullptr };
    _wstring                    m_strPath = {};

    // DX11 리소스
    ID3D11Texture2D* m_pTexture = { nullptr };
    ID3D11ShaderResourceView* m_pSRV = { nullptr };
    SpriteBatch* m_pBatch = { nullptr };

    // 렌더 스테이트 백업
    ID3D11BlendState* m_pOldBlendState = { nullptr };
    ID3D11DepthStencilState* m_pOldDepthState = { nullptr };
    ID3D11RasterizerState* m_pOldRasterState = { nullptr };
    _float                      m_fOldBlendFactor[4] = {};
    _uint                       m_iOldSampleMask = {};
    _uint                       m_iOldStencilRef = {};

    // 비디오 정보
    _uint                       m_iWidth = {};
    _uint                       m_iHeight = {};
    _float                      m_fFPS = {};
    _float                      m_fFrameTime = {};
    _float                      m_fElapsedTime = {};

    // 재생 상태
    _bool                       m_bIsPlaying = { false };
    _bool                       m_bIsLooping = { true };
    _bool                       m_bIsFinished = { false };

    // 렌더링 위치/크기
    _float2                     m_vPosition = { 0.f, 0.f };
    _float2                     m_vSize = { 0.f, 0.f };

public:
    static Video* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _wstring& _videoPath);
    Component* Clone(void* _arg);

public:
    void Free() override final;
};

NS_END
