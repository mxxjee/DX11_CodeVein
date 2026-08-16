#include "Client_Define.h"
#include "Item.h"
#include "PoolingManager.h"
#include "ParticleSystem.h"
#include "InteractionManager.h"
#include "ItemManager.h"
#include "InventoryManager.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::Item::Item()
{
}

Client::Item::Item(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: MapObject(pDevice, pContext)
{
}

Client::Item::Item(const Item& original)
	: MapObject(original)
{
}

Client::Item::~Item()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::Item::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT Client::Item::Initialize(void* arg)
{
	static _uint namenum = 0;
	MAPOBJECT_DESC* pDesc = nullptr;
	MAPOBJECT_DESC tempDesc{};

	if (nullptr != arg)
	{
		pDesc = (MAPOBJECT_DESC*)arg;

		if (pDesc->jExtraData.contains("ItemID"))
		{
			m_iItemID = pDesc->jExtraData["ItemID"].get<_uint>();

			if (pDesc->jExtraData.contains("ItemTrigger"))
				m_eItemTrigger = (ITEM_TRIGGER)pDesc->jExtraData["ItemTrigger"].get<int>();
			else
				m_eItemTrigger = ITEM_TRIGGER::NONE;
		}
		else
		{
			m_iItemID = 0;
		}

		CAST(GAMEOBJECT_DESC*)(arg)->wstrName = L"Item_" + to_wstring(namenum++);
	}
	else
	{
		m_iItemID = 0;

		tempDesc.wstrName = L"Item_Pool_" + to_wstring(namenum++);
		arg = &tempDesc;
	}

	CHECK_FAILED(MapObject::Initialize(arg), E_FAIL);

	if (pDesc != nullptr && !pDesc->jExtraData.is_null())
	{
		m_jExtraData = pDesc->jExtraData;
	}

	CHECK_FAILED(Ready_Components(), E_FAIL);

	m_eObjType = OBJTYPE::TYPE_MAP;

	Vector4 vPos = m_pTransformCom->Get_State(DIRECTION::POSITION);
	LIGHT_DESC loadDesc;

	loadDesc.eType = (LIGHT)2;

	loadDesc.vDiffuse = { 10.0f, 10.0f, 10.0f, 1.0f };
	loadDesc.vAmbient = { 0.f, 0.f, 0.f, 0.f };
	loadDesc.vSpecular = { 0.f, 0.f, 0.f, 0.f };
	loadDesc.vDirection = { 0.f, 0.f, 0.f, 0.f };

	loadDesc.vPosition = { vPos.x, vPos.y + 0.2f, vPos.z, 1.0f };

	loadDesc.fRange = 2.5f;

	_uint iNewLightIndex = m_pGameInstance->Get_LightCnt();

	if (SUCCEEDED(Add_Component(_UINT(LEVEL::STATIC), L"Proto_Component_Point_Light",
		TEXT("Com_Light" + to_wstring(iNewLightIndex + 1)),
		reinterpret_cast<Component**>(&m_pLightComponent),
		&loadDesc)))
	{
		_vector vLoadedPos = XMLoadFloat4(&loadDesc.vPosition);
		m_pLightComponent->Set_Flicker(true, 2.0f, 0.6f, 1.1f);
		m_pLightComponent->Update(0, vLoadedPos);
	}

	m_eInteractionType = INTERACTION_TYPE::ITEM;
	InteractionManager::GetInstance()->Register_InteractableObject(this);

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/

void Client::Item::OnSpawn(void* pArg)
{
	m_bIsLooted = false;
	Set_Visible(true);
	Set_Active(true);

	if (m_pLightComponent != nullptr)
		m_pLightComponent->Set_Visible(true);

	if (pArg != nullptr)
	{
		MAPOBJECT_DESC* pDesc = (MAPOBJECT_DESC*)pArg;
		if (pDesc->bSetWorldPos)
			m_pTransformCom->Set_Matrix(XMLoadFloat4x4(&pDesc->matWorldPos));

		if (pDesc->jExtraData.contains("ItemID"))
		{
			m_iItemID = pDesc->jExtraData["ItemID"].get<_uint>();

			if (pDesc->jExtraData.contains("ItemTrigger"))
				m_eItemTrigger = (ITEM_TRIGGER)pDesc->jExtraData["ItemTrigger"].get<int>();
			else
				m_eItemTrigger = ITEM_TRIGGER::NONE;
		}
	}

	if (m_pItemEffect == nullptr)
	{
		_float3 fPos;
		XMStoreFloat3(&fPos, Get_Position());

		m_pItemEffect = CAST(ParticleSystem*)(PoolingManager::Get_Instance()->Acquire_Effect(POOL_ID::EFFECT_GROUND_ITEM));
		if (m_pItemEffect != nullptr)
		{
			m_pItemEffect->Set_WorldPosition(fPos);
			m_pItemEffect->OnSpawn(nullptr);
		}
	}
}	

void Client::Item::OnDespawn()
{
	//InteractionManager::GetInstance()->UnRegisterInteractable(this);

	if (m_pItemEffect != nullptr)
	{
		PoolingManager::Get_Instance()->Despawn_Object(m_pItemEffect);
		m_pItemEffect = nullptr;
	}

	if (m_pLightComponent != nullptr)
		m_pLightComponent->Set_Visible(false);

	Set_Visible(false);
	Set_Active(false);
}

//////////////////////////////////////////////////////// 컨테이너 추가 함수 ////////////////////////////////////////////////////////
HRESULT Client::Item::Ready_Components()
{
	m_eInteractionType = INTERACTION_TYPE::ITEM;
	InteractionManager::GetInstance()->Register_InteractableObject(this);

	return S_OK;
}
/******************************************************* 컨테이너 추가 함수 *******************************************************/

//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Item::Update_Priority(const _float fTimeDelta)
{

	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Item::Update_Parallel(const _float fTimeDelta)
{
	// Update_Parallel 함수 : 함수 하나를 쓰레드로 빼서 CPU 병렬연산 수행

	return 0;
}

_int Client::Item::Update(const _float fTimeDelta)
{	
	if (m_pLightComponent != nullptr)
	{
		_vector vPos = m_pTransformCom->Get_State(DIRECTION::POSITION);

		vPos = XMVectorSetY(vPos, XMVectorGetY(vPos) + 0.1f);
		m_pLightComponent->Update(fTimeDelta, vPos);
	}
	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Item::Update_Late(const _float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);

	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/

ordered_json Client::Item::Get_ExtraData()
{
	ordered_json j = __super::Get_ExtraData();
	j["ItemID"] = m_iItemID;
	j["ItemID"] = m_iItemID;
	j["ItemTrigger"] = (int)m_eItemTrigger;
	return j;
}



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Item* Client::Item::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	Item* pInstance = new Item(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"Item 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* Client::Item::Clone(void* pArg)
{
	Item* pInstance = new Item(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"Item 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::Item::Free()
{
	if (m_bIsClone)
		InteractionManager::GetInstance()->UnRegisterInteractable(this);

	//m_pGameInstance->Remove_StaticObject(this);

	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/

#pragma region IInteractable
bool Client::Item::IsInteractable()
{
	if (m_bIsLooted)
		return false;

	GameObject* pPlayer = m_pGameInstance->Get_Player();
	if (pPlayer)
	{
		_vector vPlayerPos = pPlayer->Get_Position();
		_vector vItemPos = m_pTransformCom->Get_State(DIRECTION::POSITION);

		_float fPlayerY = XMVectorGetY(vPlayerPos);
		_float fItemY = XMVectorGetY(vItemPos);
		_float fDistY = fabs(fPlayerY - fItemY);

		_float fMaxYOffSet = 1.5f;

		if (fDistY <= fMaxYOffSet)
		{
			return true;
		}
	}

	return false;
}

void Client::Item::Enter_InteractionRange(GameObject* pPlayer)
{
	UI_MasterEvent Event;
	Event.m_EventTarget = UI_EVENT_TARGET::SYSTEM;
	Event.m_ActionName = "ActiveUI";
	Event.m_Text = L"PopUp_Interaction";
	Event.m_bPersistent = true;
	Event.m_bFlag = true;
	m_pGameInstance->Publish(Event);
}

void Client::Item::Stay_InteractionRange(GameObject* pPlayer, const _float& fTimeDelta)
{

}

void Client::Item::Exit_InteractionRange(GameObject* pPlayer)
{
	UI_MasterEvent Event;
	Event.m_EventTarget = UI_EVENT_TARGET::SYSTEM;
	Event.m_ActionName = "ActiveUI";
	Event.m_Text = L"PopUp_Interaction";
	Event.m_bPersistent = true;
	Event.m_bFlag = false;
	m_pGameInstance->Publish(Event);
}

void Client::Item::Enter_Interaction(GameObject* pPlayer)
{
	if (m_bIsLooted)
		return;

	PlayerIntercation PlayerInteracitonEvent;
	PlayerInteracitonEvent.bEKeyDown = true;
	PlayerInteracitonEvent.eInteractionType = INTERACTION_TYPE::ITEM;
	m_pGameInstance->Publish(PlayerInteracitonEvent);

	ItemInfo* pItemInfo = ItemManager::GetInstance()->Get_ItemInfo(m_iItemID);
	if (pItemInfo)
	{

		if (FAILED(InventoryManager::GetInstance()->Add_Item(pItemInfo)))
		{
			//Safe_Delete(pItemInfo);
			m_bPreInteraction = false;
		}
		Safe_Delete(pItemInfo);
	}

	UI_MasterEvent Event;
	Event.m_EventTarget = UI_EVENT_TARGET::SYSTEM;
	Event.m_ActionName = "ActiveUI";
	Event.m_Text = L"PopUp_NPC";
	Event.m_bPersistent = true;
	Event.m_bFlag = false;
	m_pGameInstance->Publish(Event);

	if (m_eItemTrigger != ITEM_TRIGGER::NONE)
	{
		ITEM_TRIGGER_EVENT TriggerEvent;
		TriggerEvent.eTrigger = m_eItemTrigger;
		m_pGameInstance->Publish(TriggerEvent);
	}

	m_bIsLooted = true;
		
	PoolingManager::Get_Instance()->Despawn_Object(this);

}

void Client::Item::Stay_Interaction(GameObject* pPlayer, const _float& fTimeDelta)
{
}

void Client::Item::Exit_Interaction(GameObject* pPlayer)
{
	m_bPreInteraction = false;
	InteractionManager::GetInstance()->Reset_BestPriority();
}


#pragma endregion