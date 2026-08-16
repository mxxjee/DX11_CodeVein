#pragma once
#include "UIObj_ProgressBar.h"

NS_BEGIN(Client)
class UIObj_Stamina :
    public UIObj_ProgressBar
{

protected:
    explicit UIObj_Stamina();
    explicit UIObj_Stamina(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_Stamina(const UIObj_Stamina& original);
    virtual ~UIObj_Stamina();


public:
    HRESULT Initialize_Prototype(_uint iLevel);
    HRESULT Initialize(void* arg);

    virtual _int	Update_Priority(const _float fTimeDelta) override;
    virtual _int	Update(const _float fTimeDelta) override;
    virtual _int	Update_Late(const _float fTimeDelta) override;
    HRESULT         Render(const _float fTimeDelta) override;
    HRESULT         Render_UI(const _float fTimeDelta);

public:
    //EventÇÔ¼ö
    virtual void        Execute_By_Event(const string& strActionName, void* pArg);



public:
    virtual void        After_ApplyData();

public:
    static UIObj_Stamina* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;

public:
    void Free() override;

public:
    void        Set_TargetValue(float* pTarget);
    void        Set_MaxValue(float* pMax);

private:
    UIObject* pRedBlinkImage = nullptr;
    unordered_map<string, UIOwnerEventInfo> m_Actions;


};
NS_END

