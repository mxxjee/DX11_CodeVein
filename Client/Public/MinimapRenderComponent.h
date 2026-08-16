#pragma once
#include "Component.h"
#include "MinimapManager.h"


//객체에 붙어서 매 프레임 minimapmanger에게 데이터전달
namespace Engine
{
    class NewTexture;
    class Shader;
    class GameObject;

}


NS_BEGIN(Client)
class MinimapManager;
class MinimapRenderComponent :
    public Component
{
public:
    enum ICON_TYPE { PLAYER, MONSTER, BOSS,LADDER, ITEM,SAVEPOINT, END };

    typedef struct MinimapCompDesc
    {
        GameObject* pOwner = nullptr;

        ICON_TYPE       eType;
        _bool           m_bIsTrace = false;
        _bool           m_bUseRotation = false;
    }MINIMAPDESC;
  
protected:
	explicit MinimapRenderComponent();
	explicit MinimapRenderComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit MinimapRenderComponent(const MinimapRenderComponent& original);
	virtual ~MinimapRenderComponent();

public:
    HRESULT Initialize_Prototype();
    HRESULT Initialize(void* _arg);
    _int	Update_Late(const _float fTimeDelta);

    HRESULT Render(const _float fTimeDelta) override;

    //manager에서 호출함.
    void    Render_Trace(_float2 UV,Shader* pShader,const _float fTimeDelta);
    void    Render_Icon(_float fCameraYaw,_float2 UV, _float fZoom, Shader* pShader,const _float fTimeDelta);

public:
    static MinimapRenderComponent* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual Component* Clone(void* arg);

public:
    void Free() override;

public:
    ICON_TYPE   Get_IconType() { return m_eIconType; }
    bool        Is_Trace() { return m_bIsTrace; }
    

private:
    ICON_TYPE                       m_eIconType;
    bool                            m_bIsTrace = false;     //발자국을남길건지 유무
    
    ID3D11ShaderResourceView*       m_pTraceTexSRV = nullptr;
    ID3D11ShaderResourceView*       m_pIconTexSRV = nullptr;

    MinimapManager*             m_pMinimapManager = nullptr;

    //미니맵매니저에게 보낼 정보구조체
    MINIMAPINFO_DESC        m_MinimapInfo;

private:
    GameObject*         m_pOwner = nullptr;
    _float              g_fTraceIconScale = 0.08f;
    _float              m_fIconScale = 0.2f;
    bool                m_bUseRotation = false;

};
NS_END

