#pragma once
#include "UIObject.h"

NS_BEGIN(Client)
class ShopManager;
class UIObj_ShopSlot;

class UIObj_ShopSlotGrid :
    public UIObject
{
public:
    /*shopmanager의 focus item_category가 바뀔때마다 필요한 이벤트를 구독해야함.*/
    /*그 이벤트를 통해 자신을 활성화할지 비활성화할지 판단함.*/
    
    //UPDATE_FOCUSCATEGORY - 카테고리버튼을 눌러서 FOCUSCATEGORY가 바꼈으니 스스로판단해서 활성/비활성화 처리해라.
    //INITIALIZE_SLOTS-상점에 올릴 아이템들을 로드한 후 각 슬롯에게 세팅하는 이벤트(게임시작시 1회만불림)

    enum class ShopSlotGridEventType{UPDATE_FOCUSCATEGORY,INITIALIZE_SLOTS,END};
    struct ShopSlotGridUIEvent
    {
        ShopSlotGridEventType       eventType;

        ITEM_CATEGORY               m_eCategory;
        vector<ItemInfo*>           m_Items;
    };
protected:
    explicit UIObj_ShopSlotGrid();
    explicit UIObj_ShopSlotGrid(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    explicit UIObj_ShopSlotGrid(const UIObj_ShopSlotGrid& original);
    virtual ~UIObj_ShopSlotGrid();
public:
    HRESULT Initialize_Prototype(_uint iLevel);
    HRESULT Initialize(void* arg);

    virtual _int	Update_Priority(const _float fTimeDelta) override;
    virtual _int	Update(const _float fTimeDelta) override;
    virtual _int	Update_Late(const _float fTimeDelta) override;
    HRESULT         Render(const _float fTimeDelta) override;
public:
    void Free() override;
public:
    virtual void        After_ApplyData();

private:
    /*Event관련함수*/
    void        Set_Slots(vector<ItemInfo*> Items);

public:
    static UIObj_ShopSlotGrid* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel);
    GameObject* Clone(void* pArg) override;


private:
    ITEM_CATEGORY       m_eItemCategory = ITEM_CATEGORY::END;//이 클래스의 아이템카테고리
    ShopManager*        m_pShopManager = nullptr;


private:
    vector< UIObj_ShopSlot*>        m_ShopSlots;

};
NS_END;

