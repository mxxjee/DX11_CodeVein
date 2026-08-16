#include "Client_Define.h"
#include "UIObj_Slot.h"
#include "UI_Image.h"
#include "UIObj_BitmapFont.h"
#include "UIObj_Window_PlayerMenu.h"
#include "UIObj_SlotGrid.h"
#include "InventoryManager.h"
#include "UIObj_GlowButton.h"
#include "UISoundUtil.h"


UIObj_Slot::UIObj_Slot()
{
}

UIObj_Slot::UIObj_Slot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:UIObject(pDevice, pContext)
{
}

UIObj_Slot::UIObj_Slot(const UIObj_Slot& original)
	:UIObject(original), m_pInventoryManager(original.m_pInventoryManager)
{
}

UIObj_Slot::~UIObj_Slot()
{
}

//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_Slot::Initialize_Prototype(_uint iLevel)
{
	m_iLevel = iLevel;
	m_pInventoryManager = InventoryManager::GetInstance();
	return S_OK;
}

HRESULT Client::UIObj_Slot::Initialize(void* arg)
{
	CHECK_FAILED(__super::Initialize(arg), E_FAIL);

	size_t ActiveSelectMark = hash<string>{}("Update_SelectMark");
	m_ActionMap.emplace(ActiveSelectMark, [this](const SLOT_STATE_EVENT& Event)
		{
			CHECK_JUST_NULL(Event.m_pEquipInfo);

			//현재 내가 속한 그리드와 맞는지 확인
			if (m_pParentGrid)
			{
				if (m_pParentGrid->Get_SlotGridType() != Event.m_eGridType)
					return;
			}

			if (m_pEquipItemInfo != nullptr)
			{
				if (m_pEquipItemInfo == Event.m_pEquipInfo)
				{
					if (m_pEquipItemInfo->m_bMain)
					{
						if (m_pSelectMark)
							m_pSelectMark->Set_Visible(true);
					}


					else
						if (m_pSelectMark)
							m_pSelectMark->Set_Visible(false);
				}
			}


		});

	size_t SelectMarkColor = hash<string>{}("Update_TotalSlot");
	m_ActionMap.emplace(SelectMarkColor, [this](const SLOT_STATE_EVENT& Event)
		{
			//구독한 모든Total grid가 다받아야함
			if (m_pParentGrid)
			{
				UIObj_SlotGrid::SLOT_GRID_TYPE eType = m_pParentGrid->Get_SlotGridType();
				if (eType == UIObj_SlotGrid::SLOT_GRID_TYPE::TOTAL)
				{
					Set_VisibleColor();

				}
			}


		});

	//////////////////////////////////////
	m_pGameInstance->Subscribe<UIObj_Slot::SLOT_STATE_EVENT>([this](const UIObj_Slot::SLOT_STATE_EVENT& Event)
		{
		
			
			for (auto& pair : m_ActionMap)
			{
				if (pair.first == Event.ActionHash)
					pair.second(Event);
			}
			

		});

	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////



_int UIObj_Slot::Update_Priority(const _float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);
	return 0;
}

_int UIObj_Slot::Update(const _float fTimeDelta)
{
	__super::Update(fTimeDelta);
	return 0;
}

_int UIObj_Slot::Update_Late(const _float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);
	return 0;
}

HRESULT UIObj_Slot::Render(const _float fTimeDelta)
{
	__super::Render(fTimeDelta);
	return 0;
}

void UIObj_Slot::Free()
{
	__super::Free();
}

UIObj_Slot* Client::UIObj_Slot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	UIObj_Slot* pInstance = new UIObj_Slot(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_Slot 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* UIObj_Slot::Clone(void* pArg)
{
	UIObj_Slot* pInstance = new UIObj_Slot(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_SlotGrid 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}

HRESULT Client::UIObj_Slot::Equip(ItemInfo* pInfo)
{
	CHECK_NULLPTR(pInfo);
	CHECK_NULLPTR(m_pIcon);

	m_pIcon->Set_Visible(true);
	m_pIcon->Set_Active(true);

	UI_Render* pRender = m_pIcon->Get_UIRenderComponent();
	if (pRender)
	{
		UI_Image* pImage = dynamic_cast<UI_Image*>(pRender);
		if (pImage)
			pImage->Change_Texture(pInfo->ItemTexKey);

	}


	m_bEquip = true;
	m_pEquipItemInfo = pInfo;

	if (m_pCountText)
	{
		m_pCountText->Set_Text(to_wstring(m_pEquipItemInfo->itemCount));

	}

	return S_OK;
}

void Client::UIObj_Slot::UnEquip()
{

	CHECK_JUST_NULL(m_pIcon);

	m_pIcon->Set_Visible(false);
	m_bEquip = false;
	m_pEquipItemInfo = nullptr;
}

void Client::UIObj_Slot::Refresh_Count(int itemCount)
{
	if (m_pCountText)
		m_pCountText->Set_Text(to_wstring(itemCount));
}


#ifdef _DEBUG

void Client::UIObj_Slot::Print_UISlotType()
{
	switch (m_SlotType)
	{
	case _UINT(SLOT_TYPE::BLOODCODE):
		COUT("SlotType:BLOODCODE");
		break;

	case _UINT(SLOT_TYPE::SHORTCUT):
		COUT("SlotType:SHORTCUT");
		break;

	case _UINT(SLOT_TYPE::PASSIVE):
		COUT("SlotType:PASSIVE");
		break;

	case _UINT(SLOT_TYPE::EQUIP_ARMOR):
		COUT("SlotType:EQUIP_ARMOR");
		break;

	case _UINT(SLOT_TYPE::EQUIP_WEAPON):
		COUT("SlotType:EQUIP_WEAPON");
		break;

	}
}
void Client::UIObj_Slot::Print_UIParentGridType()
{

	UIObj_SlotGrid::SLOT_GRID_TYPE	SlotGridType = m_pParentGrid->Get_SlotGridType();
	switch (SlotGridType)
	{
	case Client::UIObj_SlotGrid::NORMAL:
		COUT("SlotGridType=NORMAL");
		break;
	case Client::UIObj_SlotGrid::TOTAL:
		COUT("SlotGridType=TOTAL");
		break;
	case Client::UIObj_SlotGrid::END:
		break;
	default:
		break;
	}
}

#endif // _DEBUG

void Client::UIObj_Slot::Setting_TotalSlot(ItemInfo* pInfo)
{
	
	if (m_pSelectMark)
	{
		if (pInfo)
		{
			if (pInfo->m_bMain)
			{
				m_pSelectMark->Set_Visible(true);

			}
			else
				m_pSelectMark->Set_Visible(false);
		}
	
		else
		{

			if (m_pIcon)
				m_pIcon->Set_Visible(false);

			if (m_pCountText)
				m_pCountText->Set_Visible(false);


			if (m_pSelectMark)
				m_pSelectMark->Set_Visible(false);
		}
	}
}

void Client::UIObj_Slot::Set_VisibleColor()
{
	CHECK_JUST_NULL(m_pInventoryManager);

	InventoryManager::SelectSlotInfo Info = m_pInventoryManager->Get_SelectInfo();
	ItemInfo* pTargetInfo = m_pInventoryManager->Get_MainSlotItemInfo(Info.GridIdx, Info.SlotIdx);

	CHECK_JUST_NULL(m_pSelectMark);
	UI_Render* pRender = m_pSelectMark->Get_UIRenderComponent();
	CHECK_JUST_NULL(pRender);

	UI_Image* pImage = dynamic_cast<UI_Image*>(pRender);
	CHECK_JUST_NULL(pImage);

	//지금 선택한 슬롯이 점유하고있는 슬롯과 , 현재 this클래스의 equipinfo가 같으면 color를 제대로.
	if (pTargetInfo!=nullptr && pTargetInfo==m_pEquipItemInfo)
	{

		pImage->Set_Color(_float4(1.f, 1.f, 1.f, 1.f));

	}

	else
	{
		pImage->Set_Color(_float4(0.5f, 0.5f, 0.5f, 1.f));
	}


}

void Client::UIObj_Slot::Setting_MainSlot()
{
	ItemInfo* pMainInfo = m_pInventoryManager->Get_MainSlotItemInfo(m_pParentGrid->Get_Idx(), m_iSlotIdx);
	if (pMainInfo)
	{
		Equip(pMainInfo);
		UI_Image* pImage = dynamic_cast<UI_Image*>(m_pIcon->Get_UIRenderComponent());
		
		if (pImage)
		{
			if (pMainInfo->itemCount <= 0)
				pImage->Set_Color(_float4(0.5f, 0.5f, 0.5f, 1.f));
			else
				pImage->Set_Color(_float4(1.f, 1.f, 1.f, 1.f));

		}
		

		

	}
	else
	{
		//메인으로 등록된게없거나 장착된게없는 경우 표시슬롯다꺼주기
		if (!m_pEquipItemInfo)
		{
			if (m_pIcon)
				m_pIcon->Set_Visible(false);

			if (m_pCountText)
				m_pCountText->Set_Visible(false);


			if (m_pSelectMark)
				m_pSelectMark->Set_Visible(false);
		}
	}
}

wstring Client::UIObj_Slot::Get_SlotName()
{
	switch ((SLOT_TYPE)m_SlotType)
	{
	case SLOT_TYPE::EQUIP_WEAPON:
	{
		if (m_iSlotIdx == 0)
			return L"무기1";
		else
			return L"무기2";
	}
		break;
	
	case SLOT_TYPE::EQUIP_ARMOR:
	{
		if (m_iSlotIdx == 0)
			return L"흡혈아장";
		else
			return L"";
	}
	break;


	case SLOT_TYPE::SHORTCUT:
	{
		return L"숏컷" + to_wstring(m_iSlotIdx + 1);
	}
	break;

	case SLOT_TYPE::SKILL:
	{
		wstring Result = L"";
		if (m_pParentGrid)
		{

			if (m_pParentGrid->Get_Idx() == 2)
				Result = L"연혈 슬롯1";
			else
				Result = L"연혈 슬롯2";

			if (m_iSlotIdx == 0)
				Result += L"(좌)";

			else if (m_iSlotIdx == 1)
				Result += L"(상)";

			else if (m_iSlotIdx == 2)
				Result += L"(우)";

			else if (m_iSlotIdx == 3)
				Result += L"(하)";

		}

		return Result;
	}
	break;
	
	case SLOT_TYPE::PASSIVE:
		return L"패시브";
		break;


	case SLOT_TYPE::BLOODCODE:
		return L"블러드 코드";
		break;

	default:
		break;
	}

	return wstring();
}


void Client::UIObj_Slot::Set_Visible(_bool _isVisible)
{
	__super::Set_Visible(_isVisible);


	

	if (m_pParentGrid)
	{
		//if (m_pParentGrid->Get_SlotGridType() == UIObj_SlotGrid::SLOT_GRID_TYPE::TOTAL)
		//	Setting_TotalSlot(_isVisible);	//Visible토글

		//else
		//	Setting_MainSlot();	//inventorymanager를 통해 맞는 아이콘표시

		if (m_pParentGrid->Get_SlotGridType() != UIObj_SlotGrid::SLOT_GRID_TYPE::TOTAL)
			Setting_MainSlot();
	}

	else
	{

		if (m_pSelectMark)
		{
			m_pSelectMark->Set_Visible(false);
			m_pSelectMark->Set_Active(false);
		}
	}

}

///////////////////////////////////////////////////////////////////////////////////////////////////////
void        Client::UIObj_Slot::After_ApplyData()
{
	if (m_UIType == "EQUIP_WEAPON")
		m_SlotType = _UINT(SLOT_TYPE::EQUIP_WEAPON);

	else if (m_UIType == "EQUIP_ARMOR")
		m_SlotType = _UINT(SLOT_TYPE::EQUIP_ARMOR);

	else if(m_UIType=="PASSIVE")
		m_SlotType = _UINT(SLOT_TYPE::PASSIVE);

	else if (m_UIType == "SKILL")
		m_SlotType = _UINT(SLOT_TYPE::SKILL);

	

	else if (m_UIType == "SHORTCUT")
		m_SlotType = _UINT(SLOT_TYPE::SHORTCUT);


	else if (m_UIType == "BLOODCODE")
		m_SlotType = _UINT(SLOT_TYPE::BLOODCODE);

	else if (m_UIType == "TOTAL")
		m_SlotType = _UINT(SLOT_TYPE::TOTAL);

	else if (m_UIType == "SHORTCUT_QUICK")
		m_SlotType = _UINT(SLOT_TYPE::SHORTCUT_QUICK);

	UIObject* pIcon = Get_Child(L"Slot_Icon");
	if (pIcon)
		m_pIcon = pIcon;

	UIObject* pCount = Get_Child(L"Text");
	if (pCount)
		m_pCountText = dynamic_cast<UIObj_BitmapFont*>(pCount);


	UIObject* pSelectMark = Get_Child(L"SelectMark");
	if (pSelectMark)
	{
		m_pSelectMark = pSelectMark;
		m_pSelectMark->Set_Active(false);
		m_pSelectMark->Set_Visible(false);
	}

}

void Client::UIObj_Slot::OnHoverEnter()
{
	

	__super::OnHoverEnter();
	PlayHoverSound();
#ifdef _DEBUGwa
	Print_UISlotType();
	Print_UIParentGridType();
#endif

	if (m_pParentGrid)
	{
		UIObj_SlotGrid::SLOT_GRID_TYPE	SlotGridType = m_pParentGrid->Get_SlotGridType();

		if (!m_pWindow_PlayerMenu)
		{
			UIObject* pObj = m_pGameInstance->Find_UI_ByName(L"Window_PlayerMenu");
			m_pWindow_PlayerMenu = dynamic_cast<UIObj_Window_PlayerMenu*>(pObj);
			CHECK_JUST_NULL(pObj);
		}

		if (m_bHoverOutline)
			m_pWindow_PlayerMenu->OnHoverEnter_HoverOutline(_float2(m_Combined.m_fX, m_Combined.m_fY));


		if (SlotGridType == UIObj_SlotGrid::SLOT_GRID_TYPE::TOTAL)
			m_pInventoryManager->Publish_UpdateSubWindowEvent(m_pEquipItemInfo);


		else
		{
			

			UIObj_Window_PlayerMenu::PlayerMenuEvent Event;
			Event.SlotName = Get_SlotName();

			if (m_pEquipItemInfo)
				Event.ItemName = m_pEquipItemInfo->ItemName;

			else
				Event.ItemName = L"장비 없음";

			m_pGameInstance->Publish(Event);


			
		}
	}

	else
	{
		if ((SLOT_TYPE)m_SlotType == (SLOT_TYPE::BLOODCODE))
		{
			UIObj_Window_PlayerMenu::PlayerMenuEvent Event;
			Event.SlotName = L"블러드 코드";

			if (m_pEquipItemInfo)
				Event.ItemName = m_pEquipItemInfo->ItemName;

			else
				Event.ItemName = L"장비 없음";

			m_pGameInstance->Publish(Event);
		
		
			

		}
	}
}

void Client::UIObj_Slot::OnHoverExit()
{
	

	__super::OnHoverExit();
	if (m_SlotType == _UINT(SLOT_TYPE::BLOODCODE))
		return;

	if (m_bHoverOutline)
		m_pWindow_PlayerMenu->OnHoverExit_HoverOutline();

	if (!m_pWindow_PlayerMenu)
	{
		UIObject* pObj = m_pGameInstance->Find_UI_ByName(L"Window_PlayerMenu");
		m_pWindow_PlayerMenu = dynamic_cast<UIObj_Window_PlayerMenu*>(pObj);
		CHECK_JUST_NULL(pObj);
	}


}


/******************************************************* 객체 준비 함수 *******************************************************/

void Client::UIObj_Slot::OnClick()
{
	__super::OnClick();

	PlayClickSound();

	if (m_pParentGrid)
	{
		_uint GridIdx = m_pParentGrid->Get_Idx();
		if (m_pParentGrid->Get_SlotGridType() == UIObj_SlotGrid::SLOT_GRID_TYPE::TOTAL)
		{
			//점유하고있는 아이템정보가있을때..
			if (m_pEquipItemInfo && !m_pEquipItemInfo->m_bMain)
			{
				//선택한 객체를 inventorymanager의 select에전송
				HRESULT hr= m_pInventoryManager->Set_SelectItem(m_pEquipItemInfo);
				//체크무늬표시
				if (hr==S_OK)
				{
					if (m_pSelectMark)
					{
						m_pSelectMark->Set_Visible(true);
						m_pSelectMark->Set_Active(true);
						
						//색깔설정
						UI_Image* pImage = dynamic_cast<UI_Image*>(m_pSelectMark->Get_UIRenderComponent());
						CHECK_JUST_NULL(pImage);
						pImage->Set_Color(_float4(1.f, 1.f, 1.f, 1.f));

					}
				}
			}
			
		}

		//단일슬롯일떄.
		else
		{
			//inventorymanager에게 전달,total출력전 이거 선택했어요를 인벤토리에게 알림
			m_pInventoryManager->Set_SelectSlotInfo(GridIdx, m_iSlotIdx,(SLOT_TYPE)m_SlotType);
			
			//전체슬롯창열기위한 정보전송
			// 클릭이후  totaltarget들에게 이벤트전송(select mark 설정)
			m_pInventoryManager->Publish_UpdateTotalSlotGrid();
			//m_pInventoryManager->Publish_UpdateTotalSlotMarkColor();
			
			//클릭이후 subwindow에게 desc전달
			m_pInventoryManager->Publish_UpdateSubWindowEvent(m_pEquipItemInfo);

		}
	}

}

void Client::UIObj_Slot::Reset_Slot()
{
	if (m_pIcon)
	{
		m_pIcon->Set_Visible(false);
		m_pIcon->Set_Active(false);

	}

	if (m_pCountText)
	{
		m_pCountText->Set_Visible(false);
		m_pCountText->Set_Active(false);
	}


	}
