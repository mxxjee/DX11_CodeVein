#include "Client_Define.h"
#include "UIObj_SubDesc.h"
#include "UIObj_Text.h"
#include "UIObj_BitmapFont.h"
#include "ItemManager.h"


Client::UIObj_SubDesc::UIObj_SubDesc()
{
}

Client::UIObj_SubDesc::UIObj_SubDesc(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:UIObject(pDevice,pContext)
{
}

Client::UIObj_SubDesc::UIObj_SubDesc(const UIObj_SubDesc& original)
	:UIObject(original)
{
}

Client::UIObj_SubDesc::~UIObj_SubDesc()
{
}


//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_SubDesc::Initialize_Prototype(_uint iLevel)
{
	m_iLevel = iLevel;
	return S_OK;
}

HRESULT Client::UIObj_SubDesc::Initialize(void* arg)
{
	CHECK_FAILED(__super::Initialize(arg), E_FAIL);

	m_Texts.resize(_UINT(ITEM_VALUE::END));
	m_InputTexts.resize(_UINT(ITEM_VALUE::END));

	return S_OK;
}
//////////////////////////////////////////////////////////////////////
_int Client::UIObj_SubDesc::Update_Priority(const _float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);
	return 0;
}

_int Client::UIObj_SubDesc::Update(const _float fTimeDelta)
{
	__super::Update(fTimeDelta);
	return 0;
}

_int Client::UIObj_SubDesc::Update_Late(const _float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);

	return 0;
}

HRESULT Client::UIObj_SubDesc::Render(const _float fTimeDelta)
{
	__super::Render(fTimeDelta);
	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////
UIObj_SubDesc* Client::UIObj_SubDesc::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
	UIObj_SubDesc* pInstance = new UIObj_SubDesc(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_SubDesc 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* Client::UIObj_SubDesc::Clone(void* pArg)
{

	UIObj_SubDesc* pInstance = new UIObj_SubDesc(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_SubDesc 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}
void Client::UIObj_SubDesc::Update_Group(ItemInfo* pInfo)
{
	if (pInfo == nullptr)
	{
		for (auto& pObj : m_InputTexts)
		{
			if(pObj)
				pObj->Set_Active(false);
			
		}

		for (auto& pObj : m_Texts)
		{
			if (pObj)
				pObj->Set_Active(false);

		}
		
		if(m_Seconds)
			m_Seconds->Set_Active(false);

		for (auto& pObj : m_Etcs)
			pObj->Set_Active(false);

		if (m_Text_Total)
			m_Text_Total->Set_Active(false);

		if (m_Text_Count)
			m_Text_Count->Set_Active(false);
		
		if (m_Text_Current)
			m_Text_Current->Set_Active(false);

		if (m_Text_Slash)
			m_Text_Slash->Set_Active(false);


		for (auto& pObj : m_Etcs)
			pObj->Set_Active(false);

		return;

	}


	if (m_InputTexts[_UINT(ITEM_VALUE::ATTACK_POWER)])
	{
		m_InputTexts[_UINT(ITEM_VALUE::ATTACK_POWER)]->Set_Text(L"공격력");
		//m_InputTexts[_UINT(ITEM_VALUE::ATTACK_POWER)]-> Set_Active(true);
	}


	if (m_InputTexts[_UINT(ITEM_VALUE::PHYSICS_DMG)])
		m_InputTexts[_UINT(ITEM_VALUE::PHYSICS_DMG)]->Set_Text(to_wstring((int)pInfo->m_Values[_UINT(ITEM_VALUE::PHYSICS_DMG)]));

	if (m_InputTexts[_UINT(ITEM_VALUE::ICHOR_COST)])
		m_InputTexts[_UINT(ITEM_VALUE::ICHOR_COST)]->Set_Text(to_wstring((int)pInfo->m_Values[_UINT(ITEM_VALUE::ICHOR_COST)]));

	if (m_InputTexts[_UINT(ITEM_VALUE::COOLDOWN)])
		m_InputTexts[_UINT(ITEM_VALUE::COOLDOWN)]->Set_Text(to_wstring((int)pInfo->m_Values[_UINT(ITEM_VALUE::COOLDOWN)]));

	if (m_InputTexts[_UINT(ITEM_VALUE::SKILL_TYPE)])
		m_InputTexts[_UINT(ITEM_VALUE::SKILL_TYPE)]->Set_Text(pInfo->m_SkillType);

	if (m_InputTexts[_UINT(ITEM_VALUE::LINEAGE_TYPE)])
		m_InputTexts[_UINT(ITEM_VALUE::LINEAGE_TYPE)]->Set_Text(pInfo->m_wstrLineage_Type);


	if (m_InputTexts[_UINT(ITEM_VALUE::ATTACK_TYPE)])
		m_InputTexts[_UINT(ITEM_VALUE::ATTACK_TYPE)]->Set_Text(pInfo->m_AttackType);


	if (m_Text_Total)
	{
		//이건원본값 참조
		ItemInfo* ProtoItem = ItemManager::GetInstance()->Get_ItemInfo(pInfo->ItemID);
		if(ProtoItem)
			m_Text_Total->Set_Text(to_wstring((int)ProtoItem->maxItemSize));

		Safe_Delete(ProtoItem);
	}


	if (m_Text_Current)
		m_Text_Current->Set_Text(to_wstring((int)pInfo->itemCount));
	
}
///////////////////////////////////////////////////////////////////////////////////////
void Client::UIObj_SubDesc::Free()
{
	__super::Free();
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
void        Client::UIObj_SubDesc::After_ApplyData()
{
	__super::After_ApplyData();
	//자식 캐싱


	//많은정보들을 캐싱..
	//1. 일단 그냥 표시하는애들부터 캐싱
	const wstring Texts[_UINT(ITEM_VALUE::END)] = {
		L"Text_Attack",
		L"Text_Physics",
		L"Text_SpendValue",
		L"Text_time",
		L"Text_Type",
		L"Text_Category",
		L"Text_AttackType"
	};

	int i = 0;
	for (auto& pwstr : Texts)
	{
		UIObject* pChild = Get_Child(pwstr);
		if (pChild)
		{
			UIObj_Text* pText = dynamic_cast<UIObj_Text*>(pChild);
			if (pText)
				m_Texts[i]=pText;


		}
		++i;
	}


	///InputText들 캐싱
	const wstring InputTexts[_UINT(ITEM_VALUE::END)] = {
	L"Text_Attack",
	L"Text_Physics_Input",
	L"Text_SpendValue_Input",
	L"Text_Time_Input",
	L"Text_Type_Input",
	L"Text_Category_Input",
	L"Text_AttackType_Input"
	};
	i = 0;
	for (auto& pwstr : InputTexts)
	{
		UIObject* pChild = Get_Child(pwstr);
		if (pChild)
		{
			UIObj_Text* pText = dynamic_cast<UIObj_Text*>(pChild);
			if (pText)
				m_InputTexts[i] = pText;


		}
		++i;
	}


	m_Seconds = Get_Child(L"Text_Time_Second");

	const wstring wstrs[] = { L"Text_WeaponInfo",L"Text_Parameter",L"Text_None" };
	for (auto& wstr : wstrs)
	{
		UIObject* pObj = Get_Child(wstr);
		if(pObj)
			m_Etcs.push_back(pObj);
	}

	m_Text_Total = dynamic_cast<UIObj_BitmapFont*>(Get_Child(L"Text_Total"));
	m_Text_Count = dynamic_cast<UIObj_Text*>(Get_Child(L"Text_Count"));
	m_Text_Current = dynamic_cast<UIObj_BitmapFont*>(Get_Child(L"Text_Current"));
	m_Text_Slash = dynamic_cast<UIObj_BitmapFont*>(Get_Child(L"Text_Slash"));
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
