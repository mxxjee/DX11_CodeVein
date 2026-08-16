#include "Client_Define.h"
#include "InventoryManager.h"
#include "UIObj_SlotGrid.h"
#include "GameInstance.h"
#include "UIObj_Slot.h"
#include "UIObj_QuickSlot.h"
#include "UIObj_SubWindow.h"
#include "UIObj_ShortCutQuickSlot.h"
#include "UIObj_PopUp_ItemGet.h"
#include "ItemManager.h"

#include "UIObj_Haze.h"
#include "UIObj_Text.h"

#include "SkillManager.h"

 


IMPLEMENT_SINGLETON(InventoryManager);


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////

Client::InventoryManager::InventoryManager()
{
}

Client::InventoryManager::~InventoryManager()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/

//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::InventoryManager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pGameInstance = GameInstance::GetInstance();

	//전체 인벤토리 개수잡아주기


	//각 카테고리별 슬롯갯수잡기
	//이 슬롯의 의미는 전체인벤토리 슬롯개수임 (게임보면 메인 ui누르면 각 타입별 전체슬롯뜸)
	//무기
	int EquipSize = MAX_SLOT;
	m_vecCategorizedItem[_UINT(ITEM_CATEGORY::EQUIP_WEAPON)].reserve(EquipSize);
	m_CategorySize.emplace(ITEM_CATEGORY::EQUIP_WEAPON, EquipSize);


	//갑옷
	int ArmorSize = MAX_SLOT;
	m_vecCategorizedItem[_UINT(ITEM_CATEGORY::EQUIP_ARMOR)].reserve(ArmorSize);
	m_CategorySize.emplace(ITEM_CATEGORY::EQUIP_ARMOR, ArmorSize);

	//패시브
	int PassiveSize = MAX_SLOT;
	m_vecCategorizedItem[_UINT(ITEM_CATEGORY::PASSIVE)].reserve(PassiveSize);
	m_CategorySize.emplace(ITEM_CATEGORY::PASSIVE, PassiveSize);


	//스킬
	int SkillSize = MAX_SLOT;
	m_vecCategorizedItem[_UINT(ITEM_CATEGORY::SKILL)].reserve(SkillSize);
	m_CategorySize.emplace(ITEM_CATEGORY::SKILL, SkillSize);

	
	//소비 아이템
	int ItemSize = MAX_SLOT;
	m_vecCategorizedItem[_UINT(ITEM_CATEGORY::ITEM)].reserve(ItemSize);
	m_CategorySize.emplace(ITEM_CATEGORY::ITEM, ItemSize);

	//블러드 코드
	int BloodCodeSize = MAX_SLOT;
	m_vecCategorizedItem[_UINT(ITEM_CATEGORY::BLOODCODE)].reserve(BloodCodeSize);
	m_CategorySize.emplace(ITEM_CATEGORY::BLOODCODE, BloodCodeSize);

	m_iInventoryMaxSize = EquipSize + PassiveSize + SkillSize + ItemSize + BloodCodeSize;

	m_AllItems.reserve(m_iInventoryMaxSize);

	//메인 UI슬롯 개수지정
	m_MainSlotInfo[_UINT(MAINUISLOT::EQUIP_WEAPON)].resize(2);
	m_MainSlotInfo[_UINT(MAINUISLOT::EQUIP_ARMOR)].resize(1);
	m_MainSlotInfo[_UINT(MAINUISLOT::SKILL_1)].resize(4);
	m_MainSlotInfo[_UINT(MAINUISLOT::SKILL_2)].resize(4);
	m_MainSlotInfo[_UINT(MAINUISLOT::PASSIVE)].resize(4);
	m_MainSlotInfo[_UINT(MAINUISLOT::ITEM)].resize(6);
	m_MainSlotInfo[_UINT(MAINUISLOT::BLOODCODE)].resize(1);


	//숏컷 슬롯 참조할 벡터
	m_DisplayQuickSlots.resize(6);


	m_SkillAlaram.Off();
	return S_OK;

}
/******************************************************* 객체 준비 함수 *******************************************************/

_int Client::InventoryManager::Update(const _float fTimeDelta)
{
	m_SkillAlaram.Update(fTimeDelta);


	if (!m_ItemGrantQueue.empty())
	{
		m_fGrantTimer += fTimeDelta;

		// 현재 큐의 맨 앞에 있는 아이템의 지연 시간과 비교
		if (m_fGrantTimer >= m_ItemGrantQueue.front().fDelay)
		{
			ITEM_GRANT_DATA& tData = m_ItemGrantQueue.front();

			ItemInfo* pItemInfo = ItemManager::GetInstance()->Get_ItemInfo(tData.iItemID);
			if (pItemInfo)
			{
				Add_Item(pItemInfo, tData.bPopUp);
				Safe_Delete(pItemInfo);
			}

			// 초기화 후 다음 아이템 준비
			m_fGrantTimer = 0.f;
			m_ItemGrantQueue.pop();
		}
	}
	return 0;
}

void Client::InventoryManager::Set_Default_Inventory()
{
	/*기본아이템 세팅*/
	ItemInfo*	pCaster= ItemManager::GetInstance()->Get_ItemInfo(L"캐스터");
	InventoryManager::GetInstance()->Add_Item(pCaster, false);
	Set_SelectSlotInfo(6,0 , (SLOT_TYPE::BLOODCODE));
	Publish_UpdateTotalSlotGrid();
	Set_SelectItem(Get_Item(pCaster->ItemID));


	for (int i = 0; i < 3; ++i)
	{
		ItemInfo* pHillItemInfo = ItemManager::GetInstance()->Get_ItemInfo(L"재생력");
		InventoryManager::GetInstance()->Add_Item(pHillItemInfo,false);

		//장착의단계
		Set_SelectSlotInfo(5, 0, (SLOT_TYPE::SHORTCUT));
		Publish_UpdateTotalSlotGrid();
		Set_SelectItem(Get_Item(pHillItemInfo->ItemID));

		Safe_Delete(pHillItemInfo);
	}
	


	/*기본아이템 세팅*/
	ItemInfo* pItemInfo = ItemManager::GetInstance()->Get_ItemInfo(L"타락한 바요네트");
	InventoryManager::GetInstance()->Add_Item(pItemInfo, false);

	//장착의단계
	Set_SelectSlotInfo(_UINT(SLOT_TYPE::EQUIP_WEAPON), 0, (SLOT_TYPE::EQUIP_WEAPON));
	Publish_UpdateTotalSlotGrid();
	Set_SelectItem(Get_Item(pItemInfo->ItemID));


	Safe_Delete(pCaster);
	Safe_Delete(pItemInfo);
	

	/*아장입히기*/
	pItemInfo = ItemManager::GetInstance()->Get_ItemInfo(2000);
	InventoryManager::GetInstance()->Add_Item(pItemInfo, false);

	//장착의단계
	Set_SelectSlotInfo(_UINT(SLOT_TYPE::EQUIP_ARMOR), 0, (SLOT_TYPE::EQUIP_ARMOR));
	Publish_UpdateTotalSlotGrid();
	Set_SelectItem(Get_Item(pItemInfo->ItemID));
	Safe_Delete(pItemInfo);

	//스킬장착
	/*pItemInfo = ItemManager::GetInstance()->Get_ItemInfo(L"블러드 샷");
	InventoryManager::GetInstance()->Add_Item(pItemInfo, false);
	Set_SelectSlotInfo(_UINT(SLOT_TYPE::SHORTCUT), 0, (SLOT_TYPE::EQUIP_WEAPON));
	Publish_UpdateTotalSlotGrid();
	Set_SelectItem(Get_Item(pItemInfo->ItemID));*/
	//장착


	//패시브 스킬-체력증가

	pItemInfo = ItemManager::GetInstance()->Get_ItemInfo(3000);
	if (pItemInfo)
	{
		InventoryManager::GetInstance()->Add_Item(pItemInfo, false);

		//장착의단계
		Set_SelectSlotInfo(4, 0, (SLOT_TYPE::PASSIVE));
		Publish_UpdateTotalSlotGrid();
		Set_SelectItem(Get_Item(pItemInfo->ItemID));
		Safe_Delete(pItemInfo);

	}
	
	Set_Skill_Default();

//	Set_Skill_Default();


#pragma region 시연용
#ifdef _DEBUG
	/* 26.03.19 강사님 시연용 */
	pItemInfo = ItemManager::GetInstance()->Get_ItemInfo(L"흑의의 곡도");
	Add_Item(pItemInfo,false);
	Safe_Delete(pItemInfo);
	
	pItemInfo = ItemManager::GetInstance()->Get_ItemInfo(L"임펄스 앵커");
	Add_Item(pItemInfo,false);
	Safe_Delete(pItemInfo);

	pItemInfo = ItemManager::GetInstance()->Get_ItemInfo(L"여왕 토벌대의 도끼창");
	Add_Item(pItemInfo,false);
	Safe_Delete(pItemInfo);

	pItemInfo = ItemManager::GetInstance()->Get_ItemInfo(L"츠바이헨더");
	Add_Item(pItemInfo,false);
	Safe_Delete(pItemInfo);
#endif // _DEBUG
#pragma endregion 시연용
	 



}

HRESULT Client::InventoryManager::Add_Item(ItemInfo* pItem, bool bPopUp)
{
	//아이템다찼는지검사.
	CHECK_NULLPTR(pItem);

	//넣으려는 아이템의 타입이 알맞은지 확인.
	auto CategoryItemIter = m_CategorySize.find(pItem->m_eCategory);
	if (CategoryItemIter != m_CategorySize.end())
	{
		//지금 현재 이 카테고리에 채워진 슬롯개수
		int		CurrentSize = Get_Current_Size_By_Category(pItem->m_eCategory);

		vector<ItemInfo*>& TargetVector = m_vecCategorizedItem[_UINT(pItem->m_eCategory)];

		//중복아이템 찾기, (이미존재하고, 더넣을수있는애들만)
		auto FindItemIter = find_if(TargetVector.begin(), TargetVector.end(), [pItem](ItemInfo* pTarget)
			{
				return (pItem->ItemID == pTarget->ItemID);
			});

		//케이스 1. 이미존재하는 아이템의 개수를 추가할때
		if (FindItemIter != TargetVector.end())
		{
			//max카운트 계산해서 그전까지만 채우도록

			//남은 공간개수
			_uint iRoom = (pItem)->maxItemSize - (*FindItemIter)->itemCount;
			_uint iAddAmount = min(iRoom, (_uint)pItem->itemCount);
			if (iAddAmount == 0)
			{
				if (bPopUp)
				{
					UIObj_Text::SYSTEMTEXTEVENT TextEvent;
					TextEvent.Text = L"최대 소지 개수에 도달했습니다.";
					m_pGameInstance->Publish(TextEvent);
				}

	
				return E_FAIL;
			}
			
			(*FindItemIter)->itemCount += iAddAmount;
			int itemCount = pItem->itemCount - iAddAmount;

			// 다 넣었으면 종료

			if (itemCount <= 0)
			{

				if (bPopUp)
					Publish_NewItemInfo(pItem);

				Refresh_Display_QuickSlots();
				return S_OK;
			}
		}

		//케이스 2. 존재하지않는 아이템을 새로넣을때
		int maxSize = m_CategorySize[pItem->m_eCategory];
		if (CurrentSize < (size_t)maxSize) // size()가 nullptr 없는 클린 벡터일 때 기준
		{
			ItemInfo* pNewItem = new ItemInfo(*pItem);
			TargetVector.push_back(pNewItem);
		
			m_AllItems.emplace(pNewItem->ItemID, pNewItem);

			Publish_AddItemEvent(pNewItem);
		
		}

	}

	if(bPopUp)
		Publish_NewItemInfo(pItem);

	return S_OK;
}



ItemInfo* Client::InventoryManager::Get_Item(_uint itemID)
{
	auto iter= m_AllItems.find(itemID);
	if (iter != m_AllItems.end())
		return iter->second;

	return nullptr;
}

HRESULT Client::InventoryManager::Use_Item(_uint ItemID)
{
	auto iter = m_AllItems.find(ItemID);

	if (iter != m_AllItems.end())
	{
		if (iter->second->itemCount >= 1)
		{
			--iter->second->itemCount;
			return S_OK;
		}
		else
			return E_FAIL;

	}
	return S_OK;
}

HRESULT Client::InventoryManager::Use_Skill(ItemInfo* pSkill)
{
	//쿨타임관리할 맵에추가
	CoolTimeInfo Info;
	Info.fCurrent = 0.f;
	Info.fMax= pSkill->Get_Value(ITEM_VALUE::COOLDOWN);

	m_CooldownMap.emplace(pSkill->ItemID, Info);
	return S_OK;
}

HRESULT Client::InventoryManager::Use_FocusItem()
{
	CHECK_TRUE_RESULT(Is_Fully_Nullptr_Vector(m_DisplayQuickSlots),E_FAIL);

	HRESULT hr= Use_Item(m_DisplayQuickSlots[m_iShortcutFocusIdx]->ItemID);

	//갱신 이벤트 호출
	UIObj_ShortCutQuickSlot::tagShortCutQuickSlotEvent Event;
	Event.eType = UIObj_ShortCutQuickSlot::EventType::UPDATE_EQUIPDATA;


	m_pGameInstance->Publish(Event);
	return hr;
}

vector<ItemInfo*> Client::InventoryManager::Get_CategoryList(ITEM_CATEGORY eCategory)
{
	assert(_UINT(eCategory) < _UINT(ITEM_CATEGORY::END)); 

	return m_vecCategorizedItem[_UINT(eCategory)];
}

int Client::InventoryManager::Get_InventorySize()
{
	int iTotalCount = 0;
	for (auto& pair : m_AllItems)
	{
		if(pair.second)
			iTotalCount += pair.second->itemCount;
	}
	return iTotalCount;
}

int Client::InventoryManager::Get_Current_Size_By_Category(ITEM_CATEGORY eCategory)
{
	int CurrentSize = 0;
	for (auto& pObj : m_vecCategorizedItem[_UINT(eCategory)])
	{
		if (pObj != nullptr)
			++CurrentSize;
	}

	return CurrentSize;
}

int Client::InventoryManager::Get_MainSlotSize_By_MainUISlot(MAINUISLOT eSlot)
{
	int iSize = 0;

	auto iter = m_MainSlotInfo.find(_UINT(eSlot));
	CHECK_TRUE_RESULT(iter == m_MainSlotInfo.end(), 0);

	for (auto pObj : iter->second)
	{
		if (pObj != nullptr)
			++iSize;


	}
	return iSize;
}

void Client::InventoryManager::Publish_AddItemEvent(ItemInfo* pItem)
{
	//전체스롨에게 명령내리기
	//메인 인벤에 올릴려면 type을 normal로설정
	UIObj_SlotGrid::SLOT_GRID_EVENT Event;
	Event.Action = "Add_Item";
	Event.eGridType = UIObj_SlotGrid::SLOT_GRID_TYPE::TOTAL;
	Event.ActionHash = hash<string>{}(Event.Action);
	Event.pArg = pItem;

	m_pGameInstance->Publish(Event);                                                                                                                                                                                                                                                              
}

void Client::InventoryManager::Refresh_ItemEvent(ItemInfo* pItem)
{
	UIObj_SlotGrid::SLOT_GRID_EVENT Event;
	Event.Action = "Refresh_Item";
	Event.ActionHash = hash<string>{}(Event.Action);
	Event.pArg = pItem;

	m_pGameInstance->Publish(Event);

}

void Client::InventoryManager::Set_SelectSlotInfo(int SlotGridIdx, int SlotIdx, SLOT_TYPE eType)
{
	m_SelectSlotInfo.GridIdx = SlotGridIdx;
	m_SelectSlotInfo.SlotIdx = SlotIdx;
	//현재 선택한 슬롯의 타입. total로 보여줄 타입을 의미
	m_SelectSlotInfo.eType = eType;
}

HRESULT Client::InventoryManager::Set_SelectItem(ItemInfo* pInfo)
{
	CHECK_TRUE_RESULT(m_SelectSlotInfo.GridIdx == -1,E_FAIL);
	CHECK_TRUE_RESULT(m_SelectSlotInfo.SlotIdx == -1,E_FAIL);

	auto iter = m_MainSlotInfo.find(m_SelectSlotInfo.GridIdx);
	CHECK_TRUE_RESULT(iter == m_MainSlotInfo.end(), E_FAIL);

	ItemInfo* pTmp = iter->second[m_SelectSlotInfo.SlotIdx];

	if (!pTmp)
	{
		iter->second[m_SelectSlotInfo.SlotIdx] = pInfo;
		pInfo->m_bMain = true;
	}

	else
	{
		CHECK_TRUE_RESULT(pInfo == pTmp,E_FAIL);
		//다른 슬롯에서쓰고있는지검사
		for(auto pTarget:iter->second)
		{
			if (pTarget)
			{
				if (pTarget == pInfo)
					return E_FAIL;
			}
		}

	
		ItemInfo* pPre = iter->second[m_SelectSlotInfo.SlotIdx];
		if (pPre)
		{
			pPre->m_bMain = false;

			//교체, visible값 변경하기
			//원래있던값을 교체하는거라면, 체크표시풀어야함
			//Evnetbus활용..?
			//타입에맞는 슬롯들에게 이벤트보내서 이전의 선택했던 슬롯과비교해서 맞는거 체크표시풀어주기
			UIObj_Slot::SLOT_STATE_EVENT Event;
			Event.m_eGridType = UIObj_SlotGrid::SLOT_GRID_TYPE::TOTAL;
			Event.m_eSlotType = m_SelectSlotInfo.eType;
			Event.m_pEquipInfo = pPre;
			Event.m_ActionName = "Update_SelectMark";
			Event.ActionHash = hash<string>{}(Event.m_ActionName);
			
		
			m_pGameInstance->Publish(Event);

		}
			

		iter->second[m_SelectSlotInfo.SlotIdx] = pInfo;
		pInfo->m_bMain = true;

	
	}
	//skill인경우 퀵슬롯업데이트 이벤트호출
	if (pInfo->m_eCategory == ITEM_CATEGORY::SKILL)
	{
		Publish_UpdateQuickSlotEvent(pInfo);
		
	}

	//숏컷에 등록한경우, target처리가필요함 ( 첫번째 넣은 슬롯을 focusidx로설정해야함)
	if (pInfo->m_eCategory == ITEM_CATEGORY::ITEM)
	{
		Update_ShortcutFocusIdx();
	}

	Publish_UpdateTotalSlotMarkColor();
	//Publish_UpdateSubWindowEvent(pInfo);
	return S_OK;
}

ItemInfo* Client::InventoryManager::Get_MainSlotItemInfo(_uint SlotGridIdx, _uint SlotIdx)
{
	auto iter = m_MainSlotInfo.find(SlotGridIdx);
	CHECK_TRUE_RESULT(iter == m_MainSlotInfo.end(),nullptr);
	CHECK_TRUE_RESULT(iter->second.size() <= SlotIdx,nullptr);

	ItemInfo* pTmp = iter->second[SlotIdx];
	return pTmp;
}

void Client::InventoryManager::Publish_UpdateTotalSlotGrid()
{
	UIObj_SlotGrid::SLOT_GRID_EVENT Event;
	Event.Action = "Update_All_TotalSlots";
	Event.ActionHash = hash<string>{}("Update_All_TotalSlots");
	Event.eGridType = UIObj_SlotGrid::SLOT_GRID_TYPE::TOTAL;
	Event.pArg = &m_SelectSlotInfo;

	m_pGameInstance->Publish(Event);
}

void Client::InventoryManager::Publish_UpdateQuickSlotEvent(ItemInfo* pInfo)
{
	CHECK_JUST_NULL(pInfo);

	//**추가** Skill값 보내기
	Skill_Base* pSkill = SkillManager::GetInstance()->Find_Skill(pInfo->ItemID);
	CHECK_JUST_NULL(pSkill);

	UIObj_QuickSlot::QuickSlotEvent Event;
	Event.m_eEventType = UIObj_QuickSlot::QuickSlotEventType::EQUIP;
	Event.iIdx = m_SelectSlotInfo.SlotIdx;
	Event.TargetSlotGrid = m_SelectSlotInfo.GridIdx;

	Event.m_pItemInfo = pInfo;
	Event.m_pSkillBase = pSkill;
	m_pGameInstance->Publish(Event);


	//여기가찐짜로 스킬이 메인슬롯에껴지는타이밍임. 
	////여기서 이벤트쏘면됨..근데 여기는 메인슬롯판단이안되므로 이런식으로처리함
	PlayerSkillInfo SkillOnPressedEvent;
	SkillOnPressedEvent.idx = (m_SelectSlotInfo.GridIdx == 2) ? m_SelectSlotInfo.SlotIdx : m_SelectSlotInfo.SlotIdx + 4;
	SkillOnPressedEvent.m_ItemInfo = pInfo;
	m_pGameInstance->Publish(SkillOnPressedEvent);

}

void Client::InventoryManager::Publish_UpdateSubWindowEvent(ItemInfo* pInfo)
{


	UIObj_SubWindow::SubWindowEvent Event;
	Event.m_ActionName = "Update_Subwindow";
	Event.ActionHash = hash<string>{}(Event.m_ActionName);
	Event.m_Info = pInfo;


	switch (m_SelectSlotInfo.eType)
	{
	case SLOT_TYPE::EQUIP_WEAPON:
		Event.m_eCategory = ITEM_CATEGORY::EQUIP_WEAPON;
		break;

	case SLOT_TYPE::EQUIP_ARMOR:
		Event.m_eCategory = ITEM_CATEGORY::EQUIP_ARMOR;
		break;

	case SLOT_TYPE::PASSIVE:
		Event.m_eCategory = ITEM_CATEGORY::PASSIVE;
		break;

	case SLOT_TYPE::SHORTCUT:
		Event.m_eCategory = ITEM_CATEGORY::ITEM;
		break;


	case SLOT_TYPE::SKILL:
		Event.m_eCategory = ITEM_CATEGORY::SKILL;
		break;


	case SLOT_TYPE::BLOODCODE:
		Event.m_eCategory = ITEM_CATEGORY::BLOODCODE;
		break;

	default:
		break;
	}

	m_pGameInstance->Publish(Event);
}

void Client::InventoryManager::Publish_UpdateTotalSlotMarkColor()
{
	UIObj_Slot::SLOT_STATE_EVENT Event;
	Event.m_ActionName = "Update_SlotMarkColor";
	Event.ActionHash = hash<string>{}(Event.m_ActionName);
	Event.m_eSlotType = (SLOT_TYPE)m_SelectSlotInfo.eType;

	m_pGameInstance->Publish(Event);

}

bool Client::InventoryManager::Is_Fully_Nullptr_Vector(const vector<ItemInfo*> Vec)
{
	bool bResult = true;

	for (auto& pObj : Vec)
	{
		if (pObj != nullptr)
			return false;

	}

	return bResult;

}

void Client::InventoryManager::Publish_NewItemInfo(ItemInfo* pNewInfo)
{
	UIObj_PopUp_ItemGet::PopUpEvent Event;
	Event.pInfo = pNewInfo;

	m_pGameInstance->Publish(Event);

}


int Client::InventoryManager::Get_IsInMainSkillSlot(MAINUISLOT eSlotType, _uint SkillID)
{
	auto iter = m_MainSlotInfo.find(_UINT(eSlotType));
	if (iter == m_MainSlotInfo.end())
		return -1;


	//스킬아무것도안겼으면 -1
	vector<ItemInfo*>	m_FindVec = iter->second;
	bool bFullyNullptr = Is_Fully_Nullptr_Vector(m_FindVec);
	CHECK_TRUE_RESULT(bFullyNullptr,-1);

	//그이후에 찾기.
	int iIdx = 0;
	auto Finditer = find_if(m_FindVec.begin(), m_FindVec.end(), [SkillID](ItemInfo* pInfo)
		{
			return pInfo && (pInfo->ItemID == SkillID);
		});

	if (Finditer == m_FindVec.end())
		return -1;

	return (int)std::distance(m_FindVec.begin(), Finditer);
}

HRESULT Client::InventoryManager::Reset_HillItem()
{
	ItemInfo* CurrentItem = Get_Item(5001);

	
	int CurrentCount = 0;
	if (CurrentItem)
	{
		CurrentCount = CurrentItem->itemCount;
	}

	ItemInfo* pHillItem = ItemManager::GetInstance()->Get_ItemInfo(5001);//원본 
	if (pHillItem)
	{
		_uint iMaxCount = pHillItem->maxItemSize - CurrentCount;
		for (int i = 0; i < iMaxCount; ++i)
		{
			Add_Item(pHillItem,false);
		}

		
	}

	Safe_Delete(pHillItem);


	return S_OK;
}

_float Client::InventoryManager::Get_CoolDown_Ratio(ItemInfo* pItem)
{
	auto iter = m_CooldownMap.find(pItem->ItemID);
	if (iter == m_CooldownMap.end())
		return 0;

	return iter->second.fCurrent / iter->second.fMax;
	
}

void Client::InventoryManager::Publish_EndCoolTime(_uint ItemID)
{
	UIObj_QuickSlot::QuickSlotEvent Event;
	Event.m_eEventType = UIObj_QuickSlot::QuickSlotEventType::END_COOLTIME;
	
	auto ItemIter = m_AllItems.find(ItemID);
	if (ItemIter != m_AllItems.end())
		Event.m_pItemInfo = ItemIter->second;

	else
		Event.m_pItemInfo = nullptr;


	m_pGameInstance->Publish(Event);

}

void Client::InventoryManager::Change_Shortcut_Focus(_int iStep)
{
	bool bFullyNullptr = Is_Fully_Nullptr_Vector(m_MainSlotInfo[_UINT(MAINUISLOT::ITEM)]);
	CHECK_TRUE(bFullyNullptr);


	//인덱스 0~5순환
	m_iShortcutFocusIdx = (m_iShortcutFocusIdx + iStep + 6) % 6;
}

bool Client::InventoryManager::Can_Active_ShortCutMenu()
{
	bool bFullyNullptr = Is_Fully_Nullptr_Vector(m_MainSlotInfo[_UINT(MAINUISLOT::ITEM)]);
	
	return !bFullyNullptr;

}

void Client::InventoryManager::Update_ShortcutFocusIdx()
{
	/*만약 처음으로 item숏컷에 등록했다면, 등록한애로 targetidx를 바꾼다.*/
	auto iter = m_MainSlotInfo.find(_UINT(MAINUISLOT::ITEM));
	CHECK_TRUE(iter == m_MainSlotInfo.end());

	if (Get_MainSlotSize_By_MainUISlot(MAINUISLOT::ITEM))
		m_iShortcutFocusIdx = m_SelectSlotInfo.SlotIdx;

}

ItemInfo* Client::InventoryManager::Get_ShorcutFocusItem()
{
	CHECK_TRUE_RESULT(m_DisplayQuickSlots.empty(), nullptr);

	return m_DisplayQuickSlots[m_iShortcutFocusIdx];
}

void Client::InventoryManager::Refresh_Display_QuickSlots()
{
	bool bFullyNullptr = Is_Fully_Nullptr_Vector(m_MainSlotInfo[_UINT(MAINUISLOT::ITEM)]);
	if (bFullyNullptr)
		return;

	//진짜유요한 객체들만 모아두는곳 
	vector<ItemInfo*> validObjects;
	validObjects.reserve(6);

	for (auto& pObj : m_MainSlotInfo[_UINT(MAINUISLOT::ITEM)])
	{
		if (pObj != nullptr)
		{
			validObjects.push_back(pObj);

		}
	}

	if (validObjects.empty())
		return;

	int iCount = (int)validObjects.size();
	for (int i = 0; i < m_MainSlotInfo[_UINT(MAINUISLOT::ITEM)].size(); ++i)
	{
		m_DisplayQuickSlots[i] = validObjects[i%iCount];
	}

	/*템얻었을대도 바로 퀵슬롯디프를에이 바로갱신*/
	UIObj_ShortCutQuickSlot::tagShortCutQuickSlotEvent Event;
	Event.eType = UIObj_ShortCutQuickSlot::EventType::UPDATE_EQUIPDATA;


	m_pGameInstance->Publish(Event);
}

void Client::InventoryManager::Set_Haze(int iHaze, bool bUpdateUI)
{
	m_iHaze = iHaze;
	m_iHaze = clamp<int>(m_iHaze, 0, m_iMaxHaze);

	//플레이어에게 전송(스탯 컴포넌트 업데이트)
	HazeChangeEvent hazeEvent;
	hazeEvent.iHaze = m_iHaze;
	hazeEvent.m_eChangeType = HazeChangeType::SET;
	m_pGameInstance->Publish(hazeEvent);


	//UI Update
	if (bUpdateUI)
		Publish_Haze();

}

void Client::InventoryManager::Publish_Haze()
{
	UIObj_Haze::HazeUIEvent Event;
	Event.eType = UIObj_Haze::HazeUIEventType::UPDATE_CURRENTHAZE;

	Event.pArg = &m_iHaze;

	m_pGameInstance->Publish(Event);
}

void Client::InventoryManager::Publish_NewHaze(int iValue)
{
	UIObj_Haze::HazeUIEvent Event;
	Event.eType = UIObj_Haze::HazeUIEventType::UPDATE_NEWHAZE;
	Event.pArg = &iValue;
	m_pGameInstance->Publish(Event);
}

void Client::InventoryManager::Add_Haze(int iValue)
{
	m_iHaze += iValue;
	m_iHaze = clamp<int>(m_iHaze, 0, m_iMaxHaze);

	//플레이어에게 전송(스탯 컴포넌트 업데이트)
	HazeChangeEvent hazeEvent;
	hazeEvent.iHaze = iValue;
	hazeEvent.m_eChangeType = HazeChangeType::ADD;
	m_pGameInstance->Publish(hazeEvent);

	Publish_Haze();
	Publish_NewHaze(iValue);
}

void Client::InventoryManager::Reduce_Haze(int iValue)
{

	m_iHaze -= iValue;
	m_iHaze = clamp<int>(m_iHaze, 0, m_iMaxHaze);

	//플레이어에게 전송(스탯 컴포넌트 업데이트)
	HazeChangeEvent hazeEvent;
	hazeEvent.iHaze = iValue;
	hazeEvent.m_eChangeType = HazeChangeType::REDUCE;
	m_pGameInstance->Publish(hazeEvent);

	Publish_Haze();
	Publish_NewHaze(-iValue);

}

void Client::InventoryManager::Free()
{
	for (auto& pair : m_AllItems)
		Safe_Delete(pair.second);

	__super::Free();
}

void Client::InventoryManager::Set_Skill_Debug()
{
	ItemInfo* pRadiantBarrelItemInfo = ItemManager::GetInstance()->Get_ItemInfo(L"레이디언트 배럴");
	InventoryManager::GetInstance()->Add_Item(pRadiantBarrelItemInfo, false);
	Safe_Delete(pRadiantBarrelItemInfo);



	ItemInfo* pRoyalHeartItemInfo = ItemManager::GetInstance()->Get_ItemInfo(L"로열 하트");
	InventoryManager::GetInstance()->Add_Item(pRoyalHeartItemInfo, false);
	Safe_Delete(pRoyalHeartItemInfo);




	ItemInfo* pStrikeRiserItemInfo = ItemManager::GetInstance()->Get_ItemInfo(L"스트라이크 라이저");
	InventoryManager::GetInstance()->Add_Item(pStrikeRiserItemInfo, false);
	Safe_Delete(pStrikeRiserItemInfo);

	ItemInfo* ppYeomgeomjeonmuItemInfo = ItemManager::GetInstance()->Get_ItemInfo(L"염검전무");
	InventoryManager::GetInstance()->Add_Item(ppYeomgeomjeonmuItemInfo, false);
	Safe_Delete(ppYeomgeomjeonmuItemInfo);

	ItemInfo* pShadowAssaultItemInfo = ItemManager::GetInstance()->Get_ItemInfo(L"섀도우 어설트");
	InventoryManager::GetInstance()->Add_Item(pShadowAssaultItemInfo, false);
	Safe_Delete(pShadowAssaultItemInfo);

	ItemInfo* pKangRyongSkillItemInfo = ItemManager::GetInstance()->Get_ItemInfo(L"강룡복호");
	InventoryManager::GetInstance()->Add_Item(pKangRyongSkillItemInfo,false);
	Safe_Delete(pKangRyongSkillItemInfo);


	ItemInfo* pIndraCoilItemInfo = ItemManager::GetInstance()->Get_ItemInfo(L"인드라 코일");
	InventoryManager::GetInstance()->Add_Item(pIndraCoilItemInfo, false);
	Safe_Delete(pIndraCoilItemInfo);

	ItemInfo* pEnchantItemInfo = ItemManager::GetInstance()->Get_ItemInfo(L"플레임 웨폰");
	InventoryManager::GetInstance()->Add_Item(pEnchantItemInfo, false);
	Safe_Delete(pEnchantItemInfo);

}

void Client::InventoryManager::Set_Skill_Default()
{
	//기본아이템 - 공격버프(스트라이크 라이저)/ 섀도우어설트/로열하트
	ItemInfo* pFlameWapon = ItemManager::GetInstance()->Get_ItemInfo(4003);
	InventoryManager::GetInstance()->Add_Item(pFlameWapon, false);


	//장착
	Set_SelectSlotInfo(_UINT(SLOT_TYPE::SKILL), 0, (SLOT_TYPE::SKILL));
	Publish_UpdateTotalSlotGrid();
	Set_SelectItem(Get_Item(pFlameWapon->ItemID));


	Safe_Delete(pFlameWapon);


	ItemInfo* pShadowAssert = ItemManager::GetInstance()->Get_ItemInfo(L"섀도우 어설트");
	InventoryManager::GetInstance()->Add_Item(pShadowAssert, false);

	//장착
	Set_SelectSlotInfo(_UINT(SLOT_TYPE::SKILL), 1, (SLOT_TYPE::SKILL));
	Publish_UpdateTotalSlotGrid();
	Set_SelectItem(Get_Item(pShadowAssert->ItemID));

	Safe_Delete(pShadowAssert);

	

	ItemInfo* pRoyalHeart = ItemManager::GetInstance()->Get_ItemInfo(L"로열 하트");
	InventoryManager::GetInstance()->Add_Item(pRoyalHeart, false);
	//장착
	Set_SelectSlotInfo(_UINT(SLOT_TYPE::PASSIVE), 0, (SLOT_TYPE::SKILL));
	Publish_UpdateTotalSlotGrid();
	Set_SelectItem(Get_Item(pRoyalHeart->ItemID));


	Safe_Delete(pRoyalHeart);


}

void Client::InventoryManager::Get_SKill_AfterOliver()
{
	ItemInfo* pKangryong = ItemManager::GetInstance()->Get_ItemInfo(4000);
	InventoryManager::GetInstance()->Add_Item(pKangryong, true);
	Safe_Delete(pKangryong);

	m_SkillAlaram.Off();
}

void Client::InventoryManager::Get_Skill_AfterWhite()
{
	m_SkillAlaram.Off();

	//염검전무
	ItemInfo* pYeomgeom= ItemManager::GetInstance()->Get_ItemInfo(4002);
	InventoryManager::GetInstance()->Add_Item(pYeomgeom, true);
	Safe_Delete(pYeomgeom);

	//플레임웨폰
	
	m_SkillAlaram.m_AlarmFunc = [this]()
		{
			ItemInfo* pFlamWeapon = ItemManager::GetInstance()->Get_ItemInfo(4007);
			InventoryManager::GetInstance()->Add_Item(pFlamWeapon, true);
			Safe_Delete(pFlamWeapon);

			m_SkillAlaram.Off();
		};

	m_SkillAlaram.Limit = 5.f;
	m_SkillAlaram.On();

}

void Client::InventoryManager::Get_Skill_ByLevel(_uint iLevel)
{

	switch (iLevel)
	{
	case 3:
	{
		//레디언트 베럴
		ItemInfo* pRailGun = ItemManager::GetInstance()->Get_ItemInfo(4005);
		InventoryManager::GetInstance()->Add_Item(pRailGun, true);
		Safe_Delete(pRailGun);

	}
		break;

	case 7:
	{
		//인드라 코일
		ItemInfo* pCoil = ItemManager::GetInstance()->Get_ItemInfo(4006);
		InventoryManager::GetInstance()->Add_Item(pCoil, true);
		Safe_Delete(pCoil);

	}
		break;
	}
}

void Client::InventoryManager::Reserve_Items(vector<_uint> vecIDs, _float fInterval, _bool bPopUp)
{
	for (size_t i = 0; i < vecIDs.size(); ++i)
	{
		ITEM_GRANT_DATA tData;
		tData.iItemID = vecIDs[i];
		tData.bPopUp = bPopUp;

		// 첫 번째 아이템은 즉시(혹은 아주 짧게), 이후는 fInterval만큼 간격
		tData.fDelay = (i == 0) ? 0.5f : fInterval;

		m_ItemGrantQueue.push(tData);
	}
}
