#pragma once
#include "Base.h"

namespace Engine
{
    class GameInstance;
}
NS_BEGIN(Client)
class InventoryManager;

class ShopManager :
    public Base
{
    DECLARE_SINGLETON(ShopManager);

private:
    explicit ShopManager();
    virtual ~ShopManager();
public:
    HRESULT Initialize();   //각 카테고리당 상점아이템 데이터들 세팅
    


public:
    void Free() override final;

public:
    ITEM_CATEGORY Get_FocusCategory() { return m_eFocusCategory; }
    void        Set_FocusCategory(ITEM_CATEGORY eCategory,bool bForce=false);

                //상점에 올릴아이템 정보를 실제 슬롯에게 적용시킨다.(단 1회  호출함)
                //뭐팔건지 진짜로 ui에 띄우는 함수
    void        Set_AllItemInfo_To_UI();


    void          Set_FocusItem(ItemInfo* pInfo);
                    //FocusItem을 산다.
    void          Buy_Item();

    ItemInfo* Get_Item_InShop(ITEM_CATEGORY eCategory, _uint idx);
    void        Update(const _float fTimeDelta);
private:
    void        Show_Warning(wstring Text);
private:
    unordered_map< ITEM_CATEGORY, vector<ItemInfo*>>     m_ShopItems;       //사면 사본을가져온디.
    GameInstance* m_pGameInstance = nullptr;

private:
    ITEM_CATEGORY       m_eFocusCategory = ITEM_CATEGORY::END; //현재 슬롯그디르가 표시되는 카테고리
    ITEM_CATEGORY       m_iPreFocusCategory = ITEM_CATEGORY::END;       //이전값이랑 비교해서 다를때만 이벤트전송하기위해서

    ItemInfo* m_pFocusItem = nullptr;       //호버 시 갱신

    Alarm       m_HazewarningAlarm;

private:
    InventoryManager*   m_pInventoryManager = nullptr;


};
NS_END

