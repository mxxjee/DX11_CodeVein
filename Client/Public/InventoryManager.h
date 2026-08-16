#pragma once
#include "Base.h"

namespace Engine
{
    class GameInstance;

}

NS_BEGIN(Client)
/*일단 테스트용,,*/
//아이템 얻으면 event를 전송
//ui들이 이벤트를 받고 아이템equip함
class InventoryManager final :
    public Base
{
    DECLARE_SINGLETON(InventoryManager);

public:
    /* 메인 ui슬롯눌렀을때  equip을위한 캐싱*/
    struct SelectSlotInfo
    {
        int   GridIdx = -1;
        int   SlotIdx = -1;
        SLOT_TYPE eType;
    };
    
    struct CoolTimeInfo
    {
        float fCurrent = 0.f;
        float fMax = 0.f;

    };

    struct ITEM_GRANT_DATA {
        _uint iItemID;      // 얻을 아이템 ID
        _float fDelay;      // 이번 아이템을 얻기까지 기다릴 시간
        _bool bPopUp;       // 팝업 띄울지 여부
    };
private:
    explicit InventoryManager();
    virtual ~InventoryManager();
public:
    HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
     
    
    _int Update(const _float fTimeDelta);


public:
                //기본적으로 들어가야할 인벤아이템들 세팅(UI에 배치까지)
    void        Set_Default_Inventory();    
public:
        //아이템 획득시 호출
    HRESULT     Add_Item(ItemInfo* pItem,bool bPopUp=true);
  


    ItemInfo*   Get_Item(_uint itemID);
    
                //일반 사용?
    HRESULT     Use_Item(_uint ItemID);
    HRESULT     Use_Skill(ItemInfo* pSkill);
    HRESULT     Use_FocusItem();


    vector<ItemInfo*> Get_CategoryList(ITEM_CATEGORY eCategory);
    int         Get_InventorySize();
    int         Get_Current_Size_By_Category(ITEM_CATEGORY eCategory);
    int         Get_MainSlotSize_By_MainUISlot(MAINUISLOT eSlot);

    SelectSlotInfo      Get_SelectInfo() { return m_SelectSlotInfo; }

            // additem에 성공했을때, ui슬롯들에게 이벤트발송하는 함수
    void        Publish_AddItemEvent(ItemInfo*  pItem);

                //중복된아이템을 넣어서 개수변경시
    void        Refresh_ItemEvent(ItemInfo* pItem);

                    //메인UI에서 선택한 그리드/슬롯 번호 기록(전체 창에서 선택 이후 EQUIP하기위해저장)
    void        Set_SelectSlotInfo(int SlotGridIdx, int SlotIdx,SLOT_TYPE eType);
   
                //실제로 메인UI가 점유하고있는 아이템설정,total 슬롯 클릭시 호출
    HRESULT        Set_SelectItem(ItemInfo* pInfo);

                //실제 mainui에 적용된 아이템정보가져오기
    ItemInfo* Get_MainSlotItemInfo(_uint SlotGridIdx, _uint SlotIdx);//Get_MainSlotItemInfo(_UINT(EQUIP_WEAPON) 0또는1 나는 스왑하게

    void        Publish_UpdateTotalSlotGrid();
    void        Publish_UpdateQuickSlotEvent(ItemInfo* pInfo);

                    //메인슬롯을눌렀을때, subwindow가 켜지는데 얘가 표시할 item을넘겨준다
    void        Publish_UpdateSubWindowEvent(ItemInfo* pInfo);
    void        Publish_UpdateTotalSlotMarkColor();

    bool        Is_Fully_Nullptr_Vector(const vector<ItemInfo*> Vec);

    void        Publish_NewItemInfo(ItemInfo* pNewInfo);


    //없으면 -1을반환
    //eSlotType = MAINUISLOT::SKILL_1(위에있는 스킬4개)
    //MAINUISLOT_SKILL(밑에있는스킬4개)
    //메인스킬에있는지 확인함., 없으면 -1반환
    int         Get_IsInMainSkillSlot(MAINUISLOT eSlotType, _uint SkillID);

public:
    HRESULT         Reset_HillItem();

public:
    /*스킬관련*/
    /*슬롯이 인벤토리 매니저에게 쿨타임얼마나 남았는지 요청하는함수*/
    _float  Get_CoolDown_Ratio(ItemInfo* pItem);
    void    Publish_EndCoolTime(_uint ItemID);
    
public:
    /*숏컷 메뉴관련*/

                //<- , ->화살표를 눌렀을때 호출
    void        Change_Shortcut_Focus(_int iStep);

                //메인슬롯의 숏컷아이템이 아무것도 장착되지 않앗다면, false를리턴하여 숏컷메뉴안띄우게.
    bool        Can_Active_ShortCutMenu();

    void        Update_ShortcutFocusIdx();

    _uint       Get_ShortcutFocusIdx() { return m_iShortcutFocusIdx; }
    ItemInfo*   Get_ShorcutFocusItem();
    void        Refresh_Display_QuickSlots();
    const vector<ItemInfo*>& Get_DisplayQuickSlot() { return m_DisplayQuickSlots; }

public:
    /*InventoryManager*/
    /*헤이즈 관련 - 플레이어 statcomponent와 ui에 각각 이벤트발송해서 갱신*/
                    //Haze강제설정,UI갱신여부
    void            Set_Haze(int iHaze, bool bUpdateUI=true);
    int             Get_Haze() { return m_iHaze; }

                    //Haze 이벤트전송
    void            Publish_Haze();
    void            Publish_NewHaze(int iValue);//변화량던지기


    void            Add_Haze(int iValue);
  
    void            Reduce_Haze(int iValue);

public:
    void Free() override final;


public:
    void        Set_Skill_Debug();  //디버그용 스킬세팅(모든스킬 인벤에유지)
    void        Set_Skill_Default();        //릴리즈때 진짜세팅

    void        Get_SKill_AfterOliver();    //올리버죽고 얻는스킬들

    void        Get_Skill_AfterWhite(); //중간보스이후 얻을 스킬

    void        Get_Skill_ByLevel(_uint iLevel);

public:
    void        Reserve_Items(vector<_uint> vecIDs, _float fInterval, _bool bPopUp);
private:
    /*모든 아이템정보들,실제 저장소/ 마스터리스트 (여기서 포인터 빌려서 쓰기)*/
        //키값 =아이템 아이디
    unordered_map<_uint,ItemInfo* >         m_AllItems;


    //UI카테고리 별 캐싱, 소지한 무기만 표시/ 소지한 옷만 표시 이런거할떄 사용
    vector<ItemInfo*>           m_vecCategorizedItem[_UINT(ITEM_CATEGORY::END)];
    


    //MainInventory에 보여줄 아이템캐싱( 선택된 아이템들)
    unordered_map<_uint, vector<ItemInfo*>>     m_MainSlotInfo;
    //각 카테고리 별 개수기록(칸)
    unordered_map<ITEM_CATEGORY, int>        m_CategorySize;

    _uint       m_iInventoryMaxSize = 0;

    /*아이템 퀵슬롯이 참조할 배열 ( 다른 ui와 다르게 하나의 슬롯이채웢여ㅣㅆ다면 나머지슬롯도 다 이걸로채우는성격이있어서 분리)*/
    vector<ItemInfo*>   m_DisplayQuickSlots;

    /*나중에 바뀔수도있는 데이터*/
    //Key는 아이템등록시 사용했던 itemkey
    //현재 사용중인 스킬을 등록( 여기등록된 애들만 cooltime을 체크하여 줄어들게한다. 업데이트돌아갈애들만)
    unordered_map<_uint, CoolTimeInfo>    m_CooldownMap;


    //현재 포커스되고있는 숏컷 아이템 인덱스번호
    _uint       m_iShortcutFocusIdx=0;

private:
    int          m_iHaze = 0;
    int         m_iMaxHaze = 99999;


private:
    GameInstance* m_pGameInstance = nullptr;
    SelectSlotInfo      m_SelectSlotInfo;


private:
    queue<ITEM_GRANT_DATA> m_ItemGrantQueue;
    _float m_fGrantTimer = 0.f;//한개의 아이템을 여러번 얻고싶을때

    Alarm           m_SkillAlaram;
};

NS_END

