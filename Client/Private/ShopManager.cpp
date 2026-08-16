#include "Client_Define.h"
#include "ShopManager.h"
#include "GameInstance.h"
#include "ItemManager.h"
#include "UIObj_ShopSlotGrid.h"
#include "UIObj_Window_Shop.h"
#include "UIObj_Window_ShopDesc.h"

#include "UIObj_Window_ShopSubDesc.h"
#include "UIObj_Window_NeedInfo.h"
#include "UIObj_Text.h"


#include "InventoryManager.h"


IMPLEMENT_SINGLETON(ShopManager)

Client::ShopManager::ShopManager()
{
}

Client::ShopManager::~ShopManager()
{
}
HRESULT Client::ShopManager::Initialize()
{
	m_pGameInstance = GameInstance::GetInstance();
	m_pInventoryManager = InventoryManager::GetInstance();

	//상점아이템목록


	//일단 임시.. (안살것들도 다 채워넣기)
	//////////////상점에 올릴 무기 아이템정보들/////////////////////////////////////////////
	vector<ItemInfo*>		m_WeaponItems;
	for (int i = 1; i < 7; ++i)
	{
		_uint ItemID = 1000 + i;
		m_WeaponItems.push_back(ItemManager::GetInstance()->Get_ItemInfo(ItemID));//무기 아이템들(1000~1008)
	}
	m_ShopItems.emplace(ITEM_CATEGORY::EQUIP_WEAPON, m_WeaponItems);

	///////////////////////////////////////////아장 정보들////////////////////////////////////
	vector<ItemInfo*>		m_ArmorItems;
	for (int i = 1; i < 7; ++i)
	{
		_uint ItemID = 2000 + i;
		m_ArmorItems.push_back(ItemManager::GetInstance()->Get_ItemInfo(ItemID));//무기 아이템들(1000~1008)
	}
	m_ShopItems.emplace(ITEM_CATEGORY::EQUIP_ARMOR, m_ArmorItems);
	/////////////////////////////////////////////////////////////////////////////////////////


	////////////////////////////////////////아이템정보들//////////////////////////////////////
	vector<ItemInfo*>		m_Items;
	for (int i = 0; i < 9; ++i)
	{
		if (i == 1)
			continue;//5001:재생력)팔지않는 아이템임.

		_uint ItemID = 5000 + i;
		m_Items.push_back(ItemManager::GetInstance()->Get_ItemInfo(ItemID));//무기 아이템들(1000~1008)
	}
	m_ShopItems.emplace(ITEM_CATEGORY::ITEM, m_Items);
	
	////////////////////////////////////////////////////////////////////////////////////////
	

	m_HazewarningAlarm.Limit= 2.f;
	m_HazewarningAlarm.m_AlarmFunc = [this]()
		{
			UI_MasterEvent Event;
			Event.m_bPersistent = true;
			Event.m_ActionName = "ActiveUI";
			Event.m_Text = L"Warning_Haze";
			Event.m_bFlag = false;
			Event.m_EventTarget = UI_EVENT_TARGET::SYSTEM;
			m_pGameInstance->Publish(Event);
			m_HazewarningAlarm.Off();
		};





	m_HazewarningAlarm.Off();

	return S_OK;
}

void Client::ShopManager::Update(const _float fTimeDelta)
{
	m_HazewarningAlarm.Update(fTimeDelta);
}
void Client::ShopManager::Free()
{
	__super::Free();

	for (auto& pair : m_ShopItems)
	{
		for (auto& pInfo : pair.second)
		{
			Safe_Delete(pInfo);

		}

		pair.second.clear();
	}
	
}

void Client::ShopManager::Set_FocusCategory(ITEM_CATEGORY eCategory, bool bForce)
{
	if (!bForce)
	{
		if (m_eFocusCategory != eCategory)
		{
			m_iPreFocusCategory = m_eFocusCategory;
			m_eFocusCategory = eCategory;
			//이벤트발송..
			//1.해당되는 slotgrid만 켜고,나머지는 다꺼라.
			UIObj_ShopSlotGrid::ShopSlotGridUIEvent Event;
			Event.eventType = UIObj_ShopSlotGrid::ShopSlotGridEventType::UPDATE_FOCUSCATEGORY;
			Event.m_eCategory = eCategory;
			m_pGameInstance->Publish_Stack(Event);


			//2.highlight를 옮겨라
			UIObj_Window_Shop::WINDOWSHOPEVENT ShopEvent;
			ShopEvent.eType = UIObj_Window_Shop::WINDOWSHOPEVNETTPYE::UPDATE_HIGHLIGHT;
			ShopEvent.m_eCategory = eCategory;
			m_pGameInstance->Publish(ShopEvent);

		}
	}
	
	else//무조건갱신
	{
 		m_iPreFocusCategory = m_eFocusCategory;
		m_eFocusCategory = eCategory;
		//이벤트발송..
		//1.해당되는 slotgrid만 켜고,나머지는 다꺼라.
		UIObj_ShopSlotGrid::ShopSlotGridUIEvent Event;
		Event.eventType = UIObj_ShopSlotGrid::ShopSlotGridEventType::UPDATE_FOCUSCATEGORY;
		Event.m_eCategory = eCategory;
		m_pGameInstance->Publish(Event);


		//2.highlight를 옮겨라
		UIObj_Window_Shop::WINDOWSHOPEVENT ShopEvent;
		ShopEvent.eType = UIObj_Window_Shop::WINDOWSHOPEVNETTPYE::UPDATE_HIGHLIGHT;
		ShopEvent.m_eCategory = eCategory;
		m_pGameInstance->Publish(ShopEvent);
	}



}

void Client::ShopManager::Set_AllItemInfo_To_UI()
{
	/*slotgrid에게 자식들 슬롯세팅하라고 알리기*/

	UIObj_ShopSlotGrid::ShopSlotGridUIEvent Event;
	Event.eventType = UIObj_ShopSlotGrid::ShopSlotGridEventType::INITIALIZE_SLOTS;
	Event.m_eCategory = ITEM_CATEGORY::EQUIP_WEAPON;
	Event.m_Items = m_ShopItems.find(ITEM_CATEGORY::EQUIP_WEAPON)->second;
	m_pGameInstance->Publish(Event);

	////////방어구 슬롯 ui세팅//////
	Event.eventType = UIObj_ShopSlotGrid::ShopSlotGridEventType::INITIALIZE_SLOTS;
	Event.m_eCategory = ITEM_CATEGORY::EQUIP_ARMOR;
	Event.m_Items = m_ShopItems.find(ITEM_CATEGORY::EQUIP_ARMOR)->second;
	m_pGameInstance->Publish(Event);

	////////아이템 슬롯 ui세팅//////
	Event.eventType = UIObj_ShopSlotGrid::ShopSlotGridEventType::INITIALIZE_SLOTS;
	Event.m_eCategory = ITEM_CATEGORY::ITEM;
	Event.m_Items = m_ShopItems.find(ITEM_CATEGORY::ITEM)->second;
	m_pGameInstance->Publish(Event);
}

void Client::ShopManager::Set_FocusItem(ItemInfo* pInfo)
{
	CHECK_JUST_NULL(pInfo);
	m_pFocusItem = pInfo;



	//Desc (설명창 갱신)
	//1. 텍스처 갱신
	UIObj_Window_ShopDesc::ShopDescUIEvent ShopDescUIevent;
	ShopDescUIevent.eType = UIObj_Window_ShopDesc::ShopDescUIEventType::SET_ICONTEXTURE;
	ShopDescUIevent.pInfo = pInfo;

	m_pGameInstance->Publish(ShopDescUIevent);


	//2.이름 +설명갱신
	ShopDescUIevent.eType = UIObj_Window_ShopDesc::ShopDescUIEventType::SET_ITEMDESC;
	ShopDescUIevent.pInfo = pInfo;
	m_pGameInstance->Publish(ShopDescUIevent);

	//3.subdesc갱신 (공격력/물리량 이런거 표시하는 ui갱신)
	UIObj_Window_ShopSubDesc::ShopSubDescUIEvent SubDescEvent;
	SubDescEvent.eCategory = pInfo->m_eCategory;
	SubDescEvent.pInfo = pInfo;
	m_pGameInstance->Publish(SubDescEvent);

	//4.NEedInfo갱신(필요한 헤이즈량)
	UIObj_Window_NeedInfo::NeedInfoUIEvent NeedInfoIEvent;
	NeedInfoIEvent.pInfo = pInfo;
	m_pGameInstance->Publish(NeedInfoIEvent);

}

void Client::ShopManager::Buy_Item()
{
	CHECK_JUST_NULL(m_pFocusItem);

	/*먼저 개수가 초과되었는지 비교.*/
	/*1.소지개수 를 넘었는지 비교.*/
	ItemInfo* InvenItem = m_pInventoryManager->Get_Item(m_pFocusItem->ItemID);
	if (InvenItem)
	{
		if (InvenItem->itemCount + m_pFocusItem->itemCount > InvenItem->maxItemSize)
		{
			Show_Warning(L"소지개수의 한도에 도달했습니다.");
			return;
		}
	}



	/*2.현재 헤이즈양과 비교*/
	_uint iCurrentHaze = InventoryManager::GetInstance()->Get_Haze();
	_uint TargetHaze = m_pFocusItem->m_iHaze;

	if (iCurrentHaze >= TargetHaze)
	{
		//사기 가넝
 		HRESULT hr = InventoryManager::GetInstance()->Add_Item(m_pFocusItem);
		//만약 이랬는데도 못샀으면? 개수문제.
		if (hr == S_OK)
		{
			InventoryManager::GetInstance()->Reduce_Haze(TargetHaze);

			//ui갱신!!
			UIObj_Window_ShopSubDesc::ShopSubDescUIEvent Event;
			Event.eCategory = m_pFocusItem->m_eCategory;
			Event.pInfo = m_pFocusItem;
			m_pGameInstance->Publish(Event);

			//돈갱신!!
			UIObj_Window_NeedInfo::NeedInfoUIEvent NeedInfoIEvent;
			NeedInfoIEvent.pInfo = m_pFocusItem;
			m_pGameInstance->Publish(NeedInfoIEvent);
		}
	}


	else
	{
		//불가쓰..헤이즈 부족 ui띄우기
		Show_Warning(L"헤이즈가 부족합니다.");
	}

}

ItemInfo* Client::ShopManager::Get_Item_InShop(ITEM_CATEGORY eCategory, _uint idx)
{
	auto iter = m_ShopItems.find(eCategory);
	CHECK_TRUE_RESULT(iter == m_ShopItems.end(), nullptr);


	vector<ItemInfo*>	m_Vec = iter->second;
	CHECK_TRUE_RESULT(m_Vec.size()==0, nullptr);
	CHECK_TRUE_RESULT(idx>=m_Vec.size(), nullptr);	//인덱스가 벡터 크기보다큰경우

	return m_Vec[idx];
}

void Client::ShopManager::Show_Warning(wstring Text)
{
	//불가쓰..헤이즈 부족 ui띄우기
	UIObj_Text::SYSTEMTEXTEVENT TextEvent;
	TextEvent.Text = Text;
	TextEvent.eType = UIObj_Text::TEXTTYPE::WARNING;
	m_pGameInstance->Publish(TextEvent);

	UI_MasterEvent Event;
	Event.m_bPersistent = true;
	Event.m_ActionName = "ActiveUI";
	Event.m_Text = L"Warning_Haze";
	Event.m_bFlag = true;
	Event.m_EventTarget = UI_EVENT_TARGET::SYSTEM;
	m_pGameInstance->Publish(Event);

	m_HazewarningAlarm.Elapsed = 0.f;
	m_HazewarningAlarm.On();
}

