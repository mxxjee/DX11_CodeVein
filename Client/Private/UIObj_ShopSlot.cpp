#include "Client_Define.h"
#include "UIObj_ShopSlot.h"
#include "ShopManager.h"
#include "UI_Button.h"
#include "UISoundUtil.h"



Client::UIObj_ShopSlot::UIObj_ShopSlot()
{
}

Client::UIObj_ShopSlot::UIObj_ShopSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:UIObject(pDevice,pContext)
{
}

Client::UIObj_ShopSlot::UIObj_ShopSlot(const UIObj_ShopSlot& original)
	:UIObject(original)
{
}

Client::UIObj_ShopSlot::~UIObj_ShopSlot()
{
}
//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_ShopSlot::Initialize_Prototype(_uint iLevel)
{
	m_iLevel = iLevel;
	return S_OK;
}

HRESULT Client::UIObj_ShopSlot::Initialize(void* arg)
{
	CHECK_FAILED(__super::Initialize(arg), E_FAIL);

	m_pShopManager = ShopManager::GetInstance();


	m_Actions.emplace(hash<string>{}("HoverItemSlot"), [this]()
		{
			//사운드재생
			PlayHoverSound();
			//Manager에게 갱신
			m_pShopManager->Set_FocusItem(m_pItemInfo);
		});



	/*m_Actions.emplace(hash<string>{}("OnBuyItem"), [this]()
		{
			
		});*/

	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////


_int Client::UIObj_ShopSlot::Update_Priority(const _float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);
	return 0;
}

_int Client::UIObj_ShopSlot::Update(const _float fTimeDelta)
{
	__super::Update(fTimeDelta);
	return 0;
}

_int Client::UIObj_ShopSlot::Update_Late(const _float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);
	return 0;
}

HRESULT Client::UIObj_ShopSlot::Render(const _float fTimeDelta)
{
	__super::Render(fTimeDelta);
	return S_OK;
}

void Client::UIObj_ShopSlot::Free()
{
	__super::Free();
}

UIObj_ShopSlot* Client::UIObj_ShopSlot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	UIObj_ShopSlot* pInstance = new UIObj_ShopSlot(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_ShopSlot 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* Client::UIObj_ShopSlot::Clone(void* pArg)
{
	UIObj_ShopSlot* pInstance = new UIObj_ShopSlot(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_ShopSlot 사본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
void        Client::UIObj_ShopSlot::After_ApplyData()
{
	__super::After_ApplyData();
	//그리드 타입지정하기

	if (m_UIType == "EQUIP_ARMOR")
		m_eItemCategory = ITEM_CATEGORY::EQUIP_ARMOR;

	else if (m_UIType == "EQUIP_WEAPON")
		m_eItemCategory = ITEM_CATEGORY::EQUIP_WEAPON;

	else if (m_UIType == "SHORTCUT")
		m_eItemCategory = ITEM_CATEGORY::ITEM;


	UIObject* pIcon = Get_Child(L"Slot_Icon");
	if (pIcon)
	{
		m_pIcon_Image = dynamic_cast<UI_Image*>(pIcon->Get_Component_FromName(Proto_UIImage));
	}

	//UI_Button* pButton = dynamic_cast<UI_Button*>(Get_Component_FromName(Proto_UIButton));
	//if (pButton)
	//{
	//	pButton->Set_OnClickFunc([this]()
	//		{
	//			//Manager에게 갱신
	//			m_pShopManager->Buy_Item();
	//		});
	//}

}
void Client::UIObj_ShopSlot::OnClick()
{
	__super::OnClick();

	PlayClickSound();

	//Manager에게 갱신
	m_pShopManager->Buy_Item();

}
void Client::UIObj_ShopSlot::Set_ItemInfo(ItemInfo* pInfo)
{
	CHECK_JUST_NULL(pInfo);
	CHECK_JUST_NULL(m_pIcon_Image);


	 m_pItemInfo = pInfo; 
	 m_pIcon_Image->Change_Texture(pInfo->ItemTexKey);

}


void        Client::UIObj_ShopSlot::Execute_By_Event(const string& strActionName, void* pArg)
{
	CHECK_JUST_NULL(m_pShopManager);
	CHECK_JUST_NULL(m_pItemInfo);

	size_t TargetHash = hash<string>{}(strActionName);

	auto iter = m_Actions.find(TargetHash);
	if (iter != m_Actions.end())
		iter->second();

}