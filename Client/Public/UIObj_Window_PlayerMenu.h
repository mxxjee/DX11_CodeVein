#pragma once
#include "UIObject.h"

namespace Engine
{
    class UI_Progress;
}

NS_BEGIN(Client)
class UIObj_Window_Inventory;
class UIObj_Slot;
class UIObj_Text;



class UIObj_Window_PlayerMenu :
    public UIObject
{
public:
    struct PlayerMenuEvent
    {
        wstring SlotName = L"";
        wstring ItemName = L"";
    };
protected:
    explicit UIObj_Window_PlayerMenu();
    explicit UIObj_Window_PlayerMenu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_Window_PlayerMenu(const UIObj_Window_PlayerMenu& original);
    virtual ~UIObj_Window_PlayerMenu();
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
    virtual void        Set_Visible(_bool _isVisible);
public:
    void Free() override;
public:
    static UIObj_Window_PlayerMenu* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;

public:
    UIObj_Window_Inventory*        Get_InventoryMenu() { return m_pWindow_Inventory; }
    void                           OnHoverEnter_HoverOutline(const _float2& vPos);
    void                           OnHoverExit_HoverOutline();

private:
    wstring            Get_CategoryName(wstring Name);
private:
    UIObj_Window_Inventory* m_pWindow_Inventory = nullptr;
    UIObject* m_pHoverOutline = nullptr;

    UIObj_Text* m_pSlotName = nullptr;
    UIObj_Text* m_pItemName = nullptr;

    UI_Progress*    m_pProgress = nullptr;

private:
    string      HoverAnimName = "OnHover";
    string      HoverExitAnimName = "OnHoverExit";


private:
    //¾ê°¡ ÄÑÁ³À»¶§, Äü½½·Ô°ú ¼ôÄÆ¸Þ´º´Â ¹Ýµå½Ã²¨Áö´Âµ¥,
    //Äü½½·Ô°ú ¼ôÄÆ¸Þ´º°¡ ¹Ýµå½Ã ÄÑÁö´Â °æ¿ì´Â ¾Æ¹« window°¡¾øÀÌ playerhud°¡ activeÀÎ
    
    UIObject*           m_pShortCutMenu = nullptr;
    vector<UIObject*>   m_pQuickSlots;


};
NS_END

