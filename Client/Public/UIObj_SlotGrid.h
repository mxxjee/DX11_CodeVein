#pragma once
/*Slot형태의 오브젝트들을관리*/
//슬롯이 몇개있는지, 각 스롯의 인덱스 관리
//
#include "UIObject.h"
NS_BEGIN(Client)
class UIObj_Slot;
class InventoryManager;
class UIObj_SlotGrid :
    public UIObject
{
public:
    enum SLOT_GRID_TYPE{NORMAL,TOTAL,END};
public:
    typedef struct tagSlotGridEvent
    {
        string Action = "";
        size_t ActionHash = 0;

        SLOT_GRID_TYPE  eGridType = SLOT_GRID_TYPE::NORMAL;
        void* pArg = nullptr;



    }SLOT_GRID_EVENT;
protected:
    explicit UIObj_SlotGrid();
    explicit UIObj_SlotGrid(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_SlotGrid(const UIObj_SlotGrid& original);
    virtual ~UIObj_SlotGrid();

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
    void        Set_SlotGridType(SLOT_GRID_TYPE eType) { m_SlotGridType = eType; }
    SLOT_GRID_TYPE Get_SlotGridType() { return m_SlotGridType; }
    void        Set_Idx(_uint i) { m_iSlotGridIdx = i; }
    _uint       Get_Idx() { return m_iSlotGridIdx; }

    UIObj_Slot* Get_Slot(int i) { return m_Slots[i]; }

            //mainui에서 누른 단일 슬롯이 가리키는 슬롯타입에 맞는 정보 세팅해주기위해
    void        Update_All_TotalSlots(void *pArg);
    void        Set_Hover_AllSlots(bool bUseHover);
public:
    void Free() override;

public:
    static UIObj_SlotGrid* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;

    
private:
                    //이벤트 검사 시 타입이 허용되는타입인지 bool 반환
    _bool      Is_Compatible(ITEM_CATEGORY eCategory);
protected:
    vector<UIObj_Slot*> m_Slots;

    //Enumclass-SlotType
    SLOT_TYPE       m_SlotType;

    unordered_map<size_t, function<void(const void*)>>      m_Actions;


protected:
    SLOT_GRID_TYPE      m_SlotGridType = SLOT_GRID_TYPE::NORMAL;
    _uint               m_iSlotGridIdx = 0;

    InventoryManager* m_pInventoryManager = nullptr;


};
NS_END
