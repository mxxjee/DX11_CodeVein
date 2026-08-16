#pragma once
#include "UIObject.h"

/*하위메뉴가 존재하고 키보드로 이를 선택할수잇는 ui*/
NS_BEGIN(Client)
class UIObj_Selector;
class UIObj_GlowButton;

class UIObj_FocusMenu :
    public UIObject
{
    
public:
    enum class FOCUSMENUTYPE{SAVEPOINT,OTHER,END};
    struct FocusMenuUpdateEvent
    {
        int         m_iObjID;
        _uint iCurrentIdx = 0;

    };


    struct FocusMenuToParentEvent
    {
        int         m_iObjID;
        wstring         m_Text = L"";
    };
protected:
    explicit UIObj_FocusMenu();
    explicit UIObj_FocusMenu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_FocusMenu(const UIObj_FocusMenu& original);
    virtual ~UIObj_FocusMenu();
public:
    HRESULT Initialize_Prototype(_uint iLevel);
    HRESULT Initialize(void* arg);

    virtual _int	Update_Priority(const _float fTimeDelta) override;
    virtual _int	Update(const _float fTimeDelta) override;
    virtual _int	Update_Late(const _float fTimeDelta) override;
    HRESULT         Render(const _float fTimeDelta) override;
    HRESULT         Render_UI(const _float fTimeDelta);
public:
    void Free() override;
public:
    virtual void        After_ApplyData();
    virtual void        Set_Active(_bool _isActive);
public:
    static UIObj_FocusMenu* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;


protected:
    void        Change_Idx(_uint iNewIdx);

protected:
    int       m_iCurrentIdx = 0;
    _uint       m_iMaxIdx = 0;

    Alarm       To_Update_HoverLine;
    bool        m_bInput = false;
    
    UIObj_Selector*         m_pSelector = nullptr;
    vector<UIObj_GlowButton*>       m_Buttons;

    

    FOCUSMENUTYPE       m_eType;
};
NS_END

