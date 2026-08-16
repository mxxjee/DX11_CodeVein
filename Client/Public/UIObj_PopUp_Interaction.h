#pragma once
#include "UIObject.h"
namespace Engine
{
    class UI_Progress;

}
NS_BEGIN(Client)
class UIObj_Text;

class UIObj_PopUp_Interaction :
    public UIObject
{
public:
    enum Interaction_Type{ITME_PICKUP,SAVEPOINT,LADDER_UP,CHEST,NPC,END};

    struct PopUp_Interaction_Event
    {
        Interaction_Type    m_eType;
        wstring              m_Text;

    };
protected:
    explicit UIObj_PopUp_Interaction();
    explicit UIObj_PopUp_Interaction(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_PopUp_Interaction(const UIObj_PopUp_Interaction& original);
    virtual ~UIObj_PopUp_Interaction();

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
    static UIObj_PopUp_Interaction* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;

public:
    void Free() override;

private:
    void        Do_Interaction();

private:
    UI_Progress* m_Progress = nullptr;
    UIObject* m_pLeft = nullptr;
    UIObject* m_pRight = nullptr;
    UIObject* m_pKey = nullptr;

    UIObject* m_pPanel_Select = nullptr;
    UIObject* m_pPanel = nullptr;

    UIObj_Text* m_pInteraction_Text = nullptr;

private:
    string              ShowAnimKey = "OnShow";
    Interaction_Type        m_eInteractionType;

};

NS_END
