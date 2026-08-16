#pragma once
#include "UIObject.h"

NS_BEGIN(Client)
class UIObj_BitmapFont;
class UIObj_Window_PlayerMenu;
class UIObj_SlotGrid;
class InventoryManager;

class UIObj_Slot :
    public UIObject
{
public:
        //슬롯이 점유하고있는 data에 따라서 visible 체크유무판단하기 위한ㅇ ㅣ벤트
    struct SLOT_STATE_EVENT
    {
        ItemInfo* m_pEquipInfo = nullptr;     //보낼 iteminfo구조체
        SLOT_TYPE m_eSlotType = SLOT_TYPE::END;
        _uint m_eGridType = 0;
        
        string m_ActionName = "";
        size_t ActionHash = 0;
        
    };
protected:
    explicit UIObj_Slot();
    explicit UIObj_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_Slot(const UIObj_Slot& original);
    virtual ~UIObj_Slot();

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
  
    virtual void Set_Visible(_bool _isVisible);
    virtual void OnClick();
    
        //슬롯자식들 비활성화( 단지 슬롯아이콘만 보이게하는용도)
    virtual void Reset_Slot();

public:
    virtual void OnHoverEnter();
    virtual void OnHoverExit();

public:
    _uint       Get_SlotType() {return m_SlotType;}

public:
    void Free() override;

public:
    static UIObj_Slot* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;


public:
        //슬롯에 장착, 슬롯의 texture 바꿔주는역할
    virtual HRESULT        Equip(ItemInfo* pInfo);

            //해제, 아예 슬롯의 texture를 비운다.
    virtual void        UnEquip();

                    //m_bEquip=false면 true를반환, m_bEquip=true면 false를반환(이미 존재한다면 equip 불가한상태임)
    bool        Can_Equip() { return !m_bEquip; }

    ItemInfo*       Get_ItemInfo() { return m_pEquipItemInfo; }

    void        Refresh_Count(int itemCount);

    void        Set_ParentGrid(UIObj_SlotGrid* pGrid) { m_pParentGrid = pGrid; }

    void        Set_Idx(_uint i) { m_iSlotIdx = i; }


#ifdef _DEBUG
private:
    void Print_UISlotType();
    void Print_UIParentGridType();
#endif // _DEBUG



public:
    //부모 그리드 설정이후 해야할일
    virtual     void    Ready_Slot() {}
    void        Setting_TotalSlot(ItemInfo* pInfo);
    void        Set_VisibleColor();
    void        Setting_MainSlot();

    bool        Get_UserHoverOutline() { return m_bHoverOutline; }
    void        Set_UseHoverOutline(bool b) { m_bHoverOutline = b; }
private:
    wstring         Get_SlotName();
protected:

        //아이템 바뀔때 texture바꿔야하므로, 아이콘캐싱
    UIObject* m_pIcon = nullptr;
    UIObj_BitmapFont* m_pCountText = nullptr;

    //EnumClass-SlotType을 의미
    _uint       m_SlotType = 0;
    bool        m_bEquip = false;
    
    
        //점유하고있는 iteminfo
    ItemInfo* m_pEquipItemInfo = nullptr;

protected:
            //hover시 이펙트출력하기위해 캐싱.
    UIObj_Window_PlayerMenu*        m_pWindow_PlayerMenu = nullptr;
    UIObj_SlotGrid*             m_pParentGrid = nullptr;
    UIObject*               m_pSelectMark = nullptr;
    _uint                   m_iSlotIdx = 0;
    InventoryManager*       m_pInventoryManager = nullptr;


protected:
    unordered_map<size_t, function<void(const SLOT_STATE_EVENT& Event)>> m_ActionMap;
    bool    m_bHoverOutline = true;
};

NS_END