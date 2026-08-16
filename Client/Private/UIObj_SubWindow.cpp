#include "Client_Define.h"
#include "UIObj_SubWindow.h"
#include "UIObj_SlotGrid.h"
#include "UIObj_Text.h"
#include "UIObj_Slot.h"

#include "InventoryManager.h"
#include "UIObj_SubDesc.h"



Client::UIObj_SubWindow::UIObj_SubWindow()
{
}

Client::UIObj_SubWindow::UIObj_SubWindow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:UIObject(pDevice,pContext)
{
}

Client::UIObj_SubWindow::UIObj_SubWindow(const UIObj_SubWindow& original)
	:UIObject(original)
{
}

Client::UIObj_SubWindow::~UIObj_SubWindow()
{
}

//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_SubWindow::Initialize_Prototype(_uint iLevel)
{
	m_iLevel = iLevel;
	return S_OK;
}

HRESULT Client::UIObj_SubWindow::Initialize(void* arg)
{
	CHECK_FAILED(__super::Initialize(arg), E_FAIL);
	m_pInventoryManager = InventoryManager::GetInstance();

	string ActionName = "Update_Subwindow";
	size_t Hash = hash<string>{}(ActionName);
	m_Actions.emplace(Hash, [this](const SubWindowEvent& Event)
		{
			Update_SelectCategoryMark(Event.m_eCategory);
			Update_DescriptionInfo(Event.m_Info);


			//Subwindow 설정(카테고리에 맞게 켜준다)
			auto iter = m_mapSubDescGroups.find(Event.m_eCategory);

			if (iter != m_mapSubDescGroups.end())
			{

				iter->second->Set_Active(true);
				iter->second->Update_Group(Event.m_Info);
			}
		});


	m_pGameInstance->Subscribe<SubWindowEvent>([this](const SubWindowEvent& Event)
		{
			auto iter = m_Actions.find(Event.ActionHash);
			if (iter != m_Actions.end())
				iter->second(Event);

		});
	return S_OK;
}


/******************************************************* 객체 준비 함수 *******************************************************/



_int Client::UIObj_SubWindow::Update_Priority(const _float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);
	return 0;
}

_int Client::UIObj_SubWindow::Update(const _float fTimeDelta)
{
	__super::Update(fTimeDelta);
	return 0;
}

_int Client::UIObj_SubWindow::Update_Late(const _float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);
	return 0;
}

HRESULT Client::UIObj_SubWindow::Render(const _float fTimeDelta)
{
	__super::Render(fTimeDelta);
	return 0;
}

void Client::UIObj_SubWindow::Free()
{
	__super::Free();
}

void Client::UIObj_SubWindow::Set_Active(_bool _isActive)
{
	__super::Set_Active(_isActive);

	//활성화되었을때 할일..
	CHECK_JUST_NULL(m_SlotGrid);
	
	/*아이템 설명창 일단 끄고, 이후 처리에서 켜준다.*/
	for (auto& pair : m_mapSubDescGroups)
	{
		pair.second->Set_Active(false);
	}
	



}
void Client::UIObj_SubWindow::Update_SelectCategoryMark(ITEM_CATEGORY eCategory)
{

	CHECK_TRUE(m_CategoryItems.empty());
	CHECK_TRUE(m_CategoryItems.size() <= _UINT(eCategory));

	UIObject* pTarget = m_CategoryItems[_UINT(eCategory)];
	if (pTarget)
	{
		UITransform transform = pTarget->Get_LocalTransform();
		m_pCategory_SelectMark->Set_Position(transform.m_fX, transform.m_fY+3.7f);
	}
}
void Client::UIObj_SubWindow::Update_DescriptionInfo(ItemInfo* pInfo)
{
	if (pInfo)
	{
		if (m_pDescTextName)
		{
			m_pDescTextName->Set_Visible(true);
			m_pDescTextName->Set_Text(pInfo->ItemName);

		}

		if (m_pDescInfoText)
		{
			m_pDescInfoText->Set_Visible(true);
			m_pDescInfoText->Set_Text(pInfo->ItemDescription);

		}
		if (m_pDescIcon)
		{
			m_pDescIcon->Set_Visible(true);

			UI_Image* pImage = dynamic_cast<UI_Image*>(m_pDescIcon->Get_UIRenderComponent());
			if (pImage)
				pImage->Change_Texture(pInfo->ItemTexKey);

		}
	}

	else
	{
		if (m_pDescTextName)
			m_pDescTextName->Set_Visible(false);

		if (m_pDescInfoText)
			m_pDescInfoText->Set_Visible(false);

		if (m_pDescIcon)
		{
			m_pDescIcon->Set_Visible(false);

		}
	}


}
//////////////////////////////////////////////////////////////////////////////////////////////
UIObj_SubWindow* Client::UIObj_SubWindow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	UIObj_SubWindow* pInstance = new UIObj_SubWindow(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_SubWindow 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* Client::UIObj_SubWindow::Clone(void* pArg)
{
	UIObj_SubWindow* pInstance = new UIObj_SubWindow(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_SubWindow 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}
//////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////
void        Client::UIObj_SubWindow::After_ApplyData()
{
	//자식캐싱
	UIObject* pslotGrid = Get_Child(L"SlotGrid");
	if (pslotGrid)
	{
		m_SlotGrid = dynamic_cast<UIObj_SlotGrid*>(pslotGrid);
		if (m_SlotGrid)
			m_SlotGrid->Set_SlotGridType(UIObj_SlotGrid::SLOT_GRID_TYPE::TOTAL);
	}


	UIObject* pDescTextName = Get_Child(L"Desc_Text_Name");
	if (pDescTextName)
	{
		m_pDescTextName = dynamic_cast<UIObj_Text*>(pDescTextName);
	
	}


	UIObject* pDescInfoText = Get_Child(L"Desc_Text_Info");
	if (pDescInfoText)
	{
		m_pDescInfoText = dynamic_cast<UIObj_Text*>(pDescInfoText);

	}



	UIObject* pDescIcon = Get_Child(L"SelectSlot_Icon");
	if (pDescIcon)
	{
		UIObject* pSelectIcon = pDescIcon->Get_Child(L"Slot_Icon");
		if (pSelectIcon)
			m_pDescIcon = pSelectIcon;

	}

	UIObject* pWeaponIcons = Get_Child(L"Category_Weapons");
	if (pWeaponIcons)
	{
		wstring IconNames[] = {
			L"Category_Icon_Weapon",
			L"Category_Icon_Armor",
			L"Category_Icon_Passive",
			L"Category_Icon_Skils",
			L"Category_Icon_Items" };


		for (auto& wstr : IconNames)
		{
			UIObject* pTarget = pWeaponIcons->Get_Child(wstr);
			if (pTarget)
				m_CategoryItems.push_back(pTarget);
		}
	}

	UIObject* pSelectMark = Get_Child(L"Category_Icon");
	if (pSelectMark)
	{
		m_pCategory_SelectMark = pSelectMark;
	}




	//SKILL_INFO
	UIObject* pSkillInfo = Get_Child(L"Desc_SubInfo_Skills");
	if (pSkillInfo)
	{
		UIObj_SubDesc* pDesc = dynamic_cast<UIObj_SubDesc*>(pSkillInfo);
		if (pDesc)
		{
			pDesc->Set_Active(false);
			pDesc->Set_Category(ITEM_CATEGORY::SKILL);
			m_mapSubDescGroups.emplace(ITEM_CATEGORY::SKILL, pDesc);
		}
	}


	//WEAPON_INFO
	UIObject* pWeaponInfo = Get_Child(L"Desc_SubInfo_Weapons");
	if (pWeaponInfo)
	{
		UIObj_SubDesc* pDesc = dynamic_cast<UIObj_SubDesc*>(pWeaponInfo);
		if (pDesc)
		{
			pDesc->Set_Active(false);
			pDesc->Set_Category(ITEM_CATEGORY::EQUIP_WEAPON);
			m_mapSubDescGroups.emplace(ITEM_CATEGORY::EQUIP_WEAPON, pDesc);
		}
			
	}


	//ARMOR_INFO
	UIObject* pArmorInfo = Get_Child(L"Desc_SubInfo_Armor");
	if (pArmorInfo)
	{
		UIObj_SubDesc* pDesc = dynamic_cast<UIObj_SubDesc*>(pArmorInfo);
		if (pDesc)
		{
			pDesc->Set_Active(false);
			pDesc->Set_Category(ITEM_CATEGORY::EQUIP_ARMOR);
			m_mapSubDescGroups.emplace(ITEM_CATEGORY::EQUIP_ARMOR, pDesc);
		}
	}


	//PASSIVE_INFO
	UIObject* pPassiveInfo = Get_Child(L"Desc_SubInfo_Passive");
	if (pPassiveInfo)
	{
		UIObj_SubDesc* pDesc = dynamic_cast<UIObj_SubDesc*>(pPassiveInfo);
		if (pDesc)
		{
			pDesc->Set_Active(false);
			pDesc->Set_Category(ITEM_CATEGORY::PASSIVE);
			m_mapSubDescGroups.emplace(ITEM_CATEGORY::PASSIVE, pDesc);
		}
			
	}

	//ITEM_INFO
	UIObject* pItemInfo = Get_Child(L"Desc_SubInfo_Items");
	if (pItemInfo)
	{
		UIObj_SubDesc* pDesc = dynamic_cast<UIObj_SubDesc*>(pItemInfo);
		if (pDesc)
		{
			pDesc->Set_Visible(false);
			pDesc->Set_Active(false);
			pDesc->Set_Category(ITEM_CATEGORY::ITEM);
			m_mapSubDescGroups.emplace(ITEM_CATEGORY::ITEM, pDesc);
		}
			

	}
}
/******************************************************* 객체 준비 함수 *******************************************************/
