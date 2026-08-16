#pragma once
#include "UIObject.h"

NS_BEGIN(Client)
class UIObj_Text;
class UIObj_PopUp_SavePoint :
    public UIObject
{
protected:
    explicit UIObj_PopUp_SavePoint();
    explicit UIObj_PopUp_SavePoint(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_PopUp_SavePoint(const UIObj_PopUp_SavePoint& original);
    virtual ~UIObj_PopUp_SavePoint();

public:
    HRESULT Initialize_Prototype(_uint iLevel);
    HRESULT Initialize(void* arg);

    virtual _int	Update_Priority(const _float fTimeDelta) override;
    virtual _int	Update(const _float fTimeDelta) override;
    virtual _int	Update_Late(const _float fTimeDelta) override;
    HRESULT         Render(const _float fTimeDelta) override;
    HRESULT         Render_UI(const _float fTimeDelta);
public:
    virtual void        After_ApplyData();
    virtual void        Set_Active(_bool _isActive);

public:
    static UIObj_PopUp_SavePoint* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;

public:
    void Free() override;



private:
    UI_Progress* m_Progress = nullptr;
    UIObject* m_pLeft = nullptr;
    UIObject* m_pRight = nullptr;
    UIObject* m_pKey = nullptr;

    UIObject* m_pPanel_Select = nullptr;
    UIObject* m_pPanel = nullptr;

    UIObj_Text* m_pInteraction_Text = nullptr;

private:
    string      ShowAnimKey = "OnShow";
};
NS_END

