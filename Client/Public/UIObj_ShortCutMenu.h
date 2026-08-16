#pragma once
#include "UIObject.h"

NS_BEGIN(Client)
class UIObj_SlotGrid_Shortcut;
class UIObj_Text;
class UIObj_BitmapFont;
class InventoryManager;


class UIObj_ShortCutMenu :
    public UIObject
{
public:
    enum class ShortCutMenu_State {EXTRA,DEFAULT};
    enum class ARROWTYPE {LEFT,RIGHT};
protected:
    explicit UIObj_ShortCutMenu();
    explicit UIObj_ShortCutMenu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_ShortCutMenu(const UIObj_ShortCutMenu& original);
    virtual ~UIObj_ShortCutMenu();

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
    static UIObj_ShortCutMenu* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;


public:
    void Free() override;

private:
    void        Publish_ModeEvent(ShortCutMenu_State eMode);
    void        Publish_SlotDataEvent();
private:
    void        Change_State(ShortCutMenu_State State);

    void        Update_State(const _float fTimeDelta);

private:
    UIObj_SlotGrid_Shortcut*         m_pSlotGrid = nullptr;
    vector<UIObject*>       m_Arrows;
    _float vInitPosX[2];


    //Texts
    UIObj_Text*             m_pText_Name = nullptr;
 

    InventoryManager* m_pInventoryManager = nullptr;


private:
    ShortCutMenu_State      m_eState;
    _float              m_fTime = 0.f;  //특정시간이후 다시 Default모드로가기위함

    string ExtraAnimKey = "OnExtra";
    string DefaultAnimKey = "OnDefault";

    ItemInfo*   pFocusInfo = nullptr;
};
NS_END

