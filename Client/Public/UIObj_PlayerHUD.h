#pragma once
#include "UIObject.h"

NS_BEGIN(Client)
class UIObj_ProgressBar;
class UIObj_SlotGrid;
class UIObj_ShortCutMenu;


class UIObj_PlayerHUD :
    public UIObject
{
public:

protected:
    explicit UIObj_PlayerHUD();
    explicit UIObj_PlayerHUD(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_PlayerHUD(const UIObj_PlayerHUD& original);
    virtual ~UIObj_PlayerHUD();

public:
    HRESULT Initialize_Prototype(_uint iLevel);
    HRESULT Initialize(void* arg);


    //virtual HRESULT Ready_Components(void* pArg);


    virtual _int	Update_Priority(const _float fTimeDelta) override;
    virtual _int	Update(const _float fTimeDelta) override;
    virtual _int	Update_Late(const _float fTimeDelta) override;
    HRESULT         Render(const _float fTimeDelta) override;
    HRESULT         Render_UI(const _float fTimeDelta);

    virtual HRESULT         Add_Child_OnLoad(UIObject* pObj); 
    virtual HRESULT         Add_Child_OnLoad(GameObject* pObj);


public:
    virtual void        After_ApplyData();
    virtual void        Set_Active(_bool _isActive);

public:
    static UIObj_PlayerHUD* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;

public:
    void Free() override;

private:
    vector<UIObj_ProgressBar*>     m_ProgressBars;
    UIObject* m_pGuardBar;
    vector< UIObj_SlotGrid*>       m_QuickSlotGrid;
   
};
NS_END

