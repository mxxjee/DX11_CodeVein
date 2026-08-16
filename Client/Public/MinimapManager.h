#pragma once
#include "Base.h"


//맵이 달라질경우에 mapcenter다시수정해줘야한다!

namespace Engine
{
    class Shader;
    class VIBuffer_Rect;
    class GameInstance;
    class Transform;
    class Camera;

}

NS_BEGIN(Client)
struct MINIMAPINFO_DESC
{
    class MinimapRenderComponent* m_pTargetComponent = nullptr;
    class Engine::Transform* m_pTransform = nullptr;
};

struct MINIMAPDATA
{
    class MinimapRenderComponent* m_pTargetComponent = nullptr;
    _float2 UV;

    void clear() { m_pTargetComponent = nullptr; }
};

class MinimapRenderComponent;
class MinimapManager :
    public Base
{
    DECLARE_SINGLETON(MinimapManager);


    struct Camera_Buffer {
        _float4x4 g_ViewMatrix = {};
        _float4x4 g_ProjMatrix = {};
        _float4x4 g_InverseViewMatrix = {};
        _float4x4 g_InverseProjMatrix = {};
        _float4 g_vCamPosition = {};
    };
private:
    explicit MinimapManager();
    virtual ~MinimapManager();

public:
    HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iWidth, _uint iHeight);
    //미니맵RTV생성
    HRESULT Ready_Minimap_Resource(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iWidth, _uint iHeight);

    //MinimapRenderComponent가 tick으로 호출
    void    Add_Minimap_TraceQueue(const MINIMAPINFO_DESC& tDesc);
    void    Add_Minimap_CommonQueue(const MINIMAPINFO_DESC& tDesc);
    void    Add_Minimap_PlayerData(const MINIMAPINFO_DESC& tDesc);


    void    Render_To_Texture(_float fTimeDelta);

    _float2 Get_CurrentTargetUV() { return m_vCurrentTargetUV; }

    //world좌표를 uv저ㅘ표로 바꾸기
    _float2 World_To_MinimapUV(XMVECTOR vPos);


private:
    void        Render_PlayerArrow(bool bFull, _float fZoom, _float fTimeDelta);
    void        Render_Player(_float fZoom, _float  fTimeDelta);
public:
    void            Bind_Resource_By_Buffer();
    void            Render_By_Buffer(const _float fTimeDelta);

    VIBuffer_Rect* Get_Buffer() { return m_pVIBuffer; }



    //center전용 미니맵에서 호출할함수
    void            Render_Centered_Minimap(_float fZoom, Shader* pShader);//, float fTimeDelta);
    void            Render_Full_Minimap(Shader* pShader);


    void        Set_PlayerTransform(Transform* pTransform) { m_pPlayerTransform = pTransform; }

    void        Set_RotateCamera(Camera* pCam) { m_pRotateCamera = pCam; }

    void        Set_Current_Level(LEVEL eLevel);

private:
    HRESULT     Render_Minimap_Accumulate(const _float& fTimeDelta);
    HRESULT     Render_Minimap_Center(const _float& fTimeDelta);
    HRESULT     Render_Minimap_Final(const _float& fTimeDelta);

public:
    void        Render_Icons_On_UI(_float fZoom, _bool bIsCentered, _float fTimeDelta);
    void        Clear_Icons_On_UI()
    {
        m_vecCommonQueue.clear();
    }
    void        Clear_Minimap()
    {
        m_vecCommonQueue.clear();
        m_PlayerData.clear();
        m_vecTraceQueue.clear();


    }
public:
    void Free() override final;

private:
            //UI상의 아이콘으로 변형
    _float2 Get_FinalPosOnUI(_float fZoom, _float2 UV, _float2 PlayerUV, _float fAngle);

private:
    GameInstance* m_pGameInstance = nullptr;

    ID3D11Device* m_pDevice = nullptr;
    ID3D11DeviceContext* m_pContext = nullptr;

    //512x512 RTV
    ID3D11Texture2D*            m_pMiniMapTexture = nullptr;
    ID3D11RenderTargetView*     m_pMiniMapRTV = nullptr;
    ID3D11ShaderResourceView*   m_pMiniMapSRV = nullptr;

    //전용셰이더
    Shader*             m_pShader = nullptr;
    

  
    VIBuffer_Rect*      m_pVIBuffer = nullptr;

    //렌더링데이터
    deque<MINIMAPDATA>        m_vecTraceQueue;//발자국
    vector<MINIMAPDATA>        m_vecCommonQueue;//플레이어 제외한 환경오브젝트, 몬스터들
    
    MINIMAPDATA                 m_PlayerData;



    _float2                         m_vCurrentTargetUV = { 0.f,0.f };

    //월드변환 기준값
                                    //맵이 달라지면이거도수정해야함!
    _float3                         m_vMapCenter = { -229.f, -9.f, 48.f };
    _float2                         m_vMapCenterUV;     //위의값을 uv로바꾼값

    _float                          m_fMapWorldSize = 500.f;        //맵이 커버하는 월드 반경


    //누적용 데이터가 계쏙추가되지 않도록 제어하기위한 변수들
    _float3                         m_vLastPos=_float3(0.f,0.f,0.f);
    _float                          m_fValue = 20.f;

    Transform*                      m_pPlayerTransform=nullptr;
    _float2                         m_vArrowPos;
private:
    Camera*                         m_pRotateCamera = nullptr;  //회전값먹일 대상카메라 
    

private:

    /*카메라 ㅏ보는 방향(라이트 이미지) 바인딩*/
    ID3D11ShaderResourceView* m_pLightArrowImg = nullptr;

    _float          g_fArrowIconScale = 0.4f;
    _float          CameraAngle = 0.f;

private:
    _float4x4 m_matWorldMatrix = {};
    _float4x4 m_matViewMatrix = {};
    _float4x4 m_matProjMatrix = {};

    int            m_bCliping = false;


private:
    unordered_map<LEVEL, _float3>        m_MapCenters;


};
NS_END

