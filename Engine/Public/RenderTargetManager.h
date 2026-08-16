#pragma once

#include "Base.h"

NS_BEGIN(Engine)

class RenderTarget;

enum class RenderTargets {
    Target_Diffuse, Target_Normal, Target_Depth, Target_Pick, Target_U, Target_Roughness, Target_MtrlSpecular, Target_Shade, Target_Specular, Target_Shadow1, Target_Shadow2, Target_Shadow3, Target_Shadow4, Target_Shadow_Bake,
    Target_HDR, Target_Bright, Target_DownSample, Target_BlurHorizontal, Target_BlurVertical, Target_Effect,
    Target_UI_BlurHorizontal, Target_UI_BlurVertical, Target_Emissive, Target_SSAO, Target_SSAOBLur, Target_GodRayCopy, Target_GodRay, Target_Text, Target_CamVelocity, Target_CamMotionBlur, Target_DownSample1, Target_DownSample2, Target_DownSample3,
    Target_BlurHorizontal1, Target_BlurHorizontal2, Target_BlurHorizontal3, Target_BlurVertical1, Target_BlurVertical2, Target_BlurVertical3, Target_HDR_EFFECT_EMISSIVE, Target_VelocityOut,

    Target_Minimap_Acuumulate,//발자국이 남는 누적용
    Target_Minimap_Final,       //지형+발자국+아이콘 모두 합쳐진 최종본RTV
    Target_Minimap_Center,
    END
};

enum class MRT {
    GAMEOBJECT, LIGHTACC, SHADOW, HDR, HDR_BRIGHT, HDR_DOWNSAMPLE, HDR_BLURHORIZONTAL, HDR_BLURVERTICAL, SSAO, SSAO_Blur, SHADOW_BAKE,
    UI_BLURHORIZONTAL, UI_BLURVERTICAL, GODRAYCOPY, GODRAY,TEXT, CAMVELOCITY, CAMMOTIONBLUR, HDR_DOWNSAMPLE1, HDR_DOWNSAMPLE2, HDR_DOWNSAMPLE3,
    HDR_BLURHORIZONTAL1, HDR_BLURHORIZONTAL2, HDR_BLURHORIZONTAL3, HDR_BLURVERTICAL1, HDR_BLURVERTICAL2, HDR_BLURVERTICAL3, VELOCITYOUT,

    MINIMAP_FINAL,END
};

class RenderTargetManager final : public Base
{
private:
    explicit RenderTargetManager();
    explicit RenderTargetManager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual ~RenderTargetManager();

public:
    HRESULT Initialize();
    _int    Update_Priority(const _float fTimeDelta);
    _int    Update(const _float fTimeDelta);
    _int    Update_Late(const _float fTimeDelta);
    HRESULT Render(const _float fTimeDelta);

public:
    // 인자 타입을 Enum으로 변경
    HRESULT Add_RenderTarget(RenderTargets _targetName, _float _width, _float _height, DXGI_FORMAT _textureformat, const _float4& _clearcolor);
    HRESULT Add_RenderTarget_Shadow(RenderTargets _targetName, _float _width, _float _height, DXGI_FORMAT _textureformat, const _float4& _clearcolor);
    HRESULT Add_MultiRenderTarget(MRT _mrtName, RenderTargets _rtvName);

    // RTV클래스가 가지고 있는 SRV를 쉐이더에 바인딩하는 함수
    HRESULT Bind_ShaderResource_FullSlot(RenderTargets _rtvName, class Shader* _shader, _uint _slotNum, _uint _stageMask);

    // 받아온 이름의 Multi Render Target으로 렌더타겟을 교체하는 함수
    // 인자 타입을 MRT Enum으로 변경
    HRESULT Begin_MRT(MRT _mrtName, _bool DSV, _bool _Clear);
    HRESULT Begin_MRT_HDR(MRT _mrtName, ID3D11DepthStencilView* _DSV, _bool _Clear);
    HRESULT Begin_RT(RenderTargets _targetName, ID3D11DepthStencilView* _DSV);
    HRESULT Begin_RT_HDR(RenderTargets _targetName, ID3D11DepthStencilView* _DSV, _bool _Clear);
    ID3D11DepthStencilView** Begin_MRT_ShadowCascade(MRT _mrtName, vector<GameObject*> m_vecObjects[], _float _fTimeDelta, _int iNumDSV, RENDER_GROUP _eRGroup);
    HRESULT End_MRT();
    HRESULT Reset_MRT();

    // 렌더타겟의 크기를 변경하기 위한 함수
    HRESULT ResizeBuffers(_float _width, _float _height);

    // Texture2D 복사를 위한 함수
    // 인자 타입을 RenderTargets Enum으로 변경
    void Copy_Resource(RenderTargets _rtvName, ID3D11Resource* pDest);

    void Clear_RTV(MRT _mrtName);

#pragma region Getter 함수
    ID3D11RenderTargetView* Get_RenderTargetView(RenderTargets _target);

    ID3D11ShaderResourceView* Get_ShaderResourceView(RenderTargets _target);
    
    DXGI_FORMAT Get_Format() const;
    _float4 Get_ClearColor() const;
#pragma endregion Getter 함수


#ifdef _DEBUG
    // 디버그 함수들도 Enum을 받도록 수정
    HRESULT Ready_Debug(RenderTargets _RTName, _float _fX, _float _fY, _float _sizeX, _float _sizeY);
    HRESULT Render(MRT _MRTName, class Shader* _shader, class VIBuffer_Rect* _buffer, const _float fTimeDelta);
    HRESULT Render(RenderTargets _targetName, class Shader* _shader, class VIBuffer_Rect* _buffer, const _float fTimeDelta);
#endif

private:
    ID3D11RenderTargetView* m_pBackBufferRTV = { nullptr };
    ID3D11DepthStencilView* m_pBackBufferDSV = { nullptr };

    ID3D11ShaderResourceView* m_pNullSRVs[128]{}; // SRV는 최대 128개까지 GPU에 등록 가능

    // Unordered_map 제거함

    // UMAP 렌더타겟들 이쪽(vector)으로 옮김
    vector<RenderTarget*> m_vecRenderTargets;
    vector<vector<RenderTarget*>> m_vecMultiRenderTargets;
    _int m_iNumRenderTargets = {};
    _int m_iNumMultiRenderTargets = {};

    _bool 한번이라도END_MRT호출했는가 = false;

private:
    ID3D11Device* m_pDevice = { nullptr };
    ID3D11DeviceContext* m_pContext = { nullptr };
    class GameInstance* m_pGameInstance = { nullptr };

    //그림자용
    ID3D11DepthStencilView* m_pDSVShadows[8] = {};

public:
    static RenderTargetManager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
    void Free() override final;
};

NS_END