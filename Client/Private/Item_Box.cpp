#include "Client_Define.h"
#include "Item_Box.h"
#include "InteractionManager.h"
#include "ItemManager.h"
#include "InventoryManager.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::Item_Box::Item_Box()
{
}

Client::Item_Box::Item_Box(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: MapObject(pDevice, pContext)
{
}

Client::Item_Box::Item_Box(const Item_Box& original)
	: MapObject(original)
{
}

Client::Item_Box::~Item_Box()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::Item_Box::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT Client::Item_Box::Initialize(void* arg)
{
	static _uint namenum = 0;
	MAPOBJECT_DESC* pDesc = nullptr;

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
	}
	else
	{
		m_iItemID = 0;
	}

	CAST(GAMEOBJECT_DESC*)(arg)->wstrName = L"Item_Box_" + to_wstring(namenum++);

	CHECK_FAILED(MapObject::Initialize(arg), E_FAIL);

	if (pDesc != nullptr && !pDesc->jExtraData.is_null())
	{
		m_jExtraData = pDesc->jExtraData;
	}

	CHECK_FAILED(Ready_Components(), E_FAIL);

	m_eObjType = OBJTYPE::TYPE_MAP;

	PHYSX_ACTOR_DESC actordesc;
	actordesc.matWorld = m_pTransformCom->Get_WorldFloat4x4();
	actordesc.pModel = m_pModelCom;
	m_pPhysXActor = m_pGameInstance->PhysX_Load_Static_Actor_Auto(actordesc);

	Set_AllPass_VecObjPass(7);

	Vector4 vPos = m_pTransformCom->Get_State(DIRECTION::POSITION);

	m_eInteractionType = INTERACTION_TYPE::ITEM_BOX;
	InteractionManager::GetInstance()->Register_InteractableObject(this);

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/


//////////////////////////////////////////////////////// 컨테이너 추가 함수 ////////////////////////////////////////////////////////
HRESULT Client::Item_Box::Ready_Components()
{
	Add_Shader(L"Prototype_Component_Shader_VTXMesh");
	Add_Model(Proto_Model(L"ItemBox_Body"));
	Add_Component(m_iLevel, Proto_Model(L"ItemBox_Cover"), L"Com_Model_Cover", RCAST(Component**)(&m_pCoverModelCom));

	_matrix matTranslation = XMMatrixTranslation(0.f, 0.6f, -0.42f);
	m_matCoverLocal = matTranslation;

	Add_Object_To_Octree(RENDER_GROUP::NONBLEND);
	Set_AllPass_ByModel(m_pModelCom, 7);

	return S_OK;
}
/******************************************************* 컨테이너 추가 함수 *******************************************************/

//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Item_Box::Update_Priority(const _float fTimeDelta)
{

	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Item_Box::Update_Parallel(const _float fTimeDelta)
{
	// Update_Parallel 함수 : 함수 하나를 쓰레드로 빼서 CPU 병렬연산 수행

	return 0;
}

_int Client::Item_Box::Update(const _float fTimeDelta)
{
	if (m_bIsOpened && m_fOpenAngle < 90.f)
	{
		m_fOpenAngle += 120.f * fTimeDelta;
		if (m_fOpenAngle > 90.f)
			m_fOpenAngle = 90.f;
	}

	_matrix matRotation = XMMatrixRotationX(XMConvertToRadians(-m_fOpenAngle));

	_matrix matTranslation = XMMatrixTranslation(0.f, 0.6f, -0.42f);

	m_matCoverLocal = matRotation * matTranslation;

	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Item_Box::Update_Late(const _float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);
	//if (m_pColliderCom)
	//{
	//	m_pColliderCom->Update(m_pLightComponent->Get_Light_Matrix());
	//	_matrix temp = m_pTransformCom->Get_WorldMatrix();
	//	m_pColliderCom->Add_Debug_Render();
	//}

	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/

//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Client::Item_Box::Render(const _float fTimeDelta)
{
	CHECK_FAILED(m_pTransformCom->Bind_WorldMatrix(m_pShaderCom), E_FAIL);
	CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_ObjectID, &m_iObjectID, sizeof(_uint)), E_FAIL);

	// m_pModelCom->Add_DebugRender();
	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
	{
		_uint test = 0;
		CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 0, i, aiTextureType_DIFFUSE, 0, &test), E_FAIL);
		CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 2, i, aiTextureType_NORMALS, 0, &test), E_FAIL);

		CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 4, i, aiTextureType_METALNESS, 0, &test), E_FAIL);
		CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 10, i, aiTextureType_EMISSIVE, 0, &test), E_FAIL);
		CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 3, i, aiTextureType_OPACITY, 0, &test), E_FAIL);
		CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 6, i, aiTextureType_SHININESS, 0, &test), E_FAIL);
		CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 5, i, aiTextureType_SPECULAR, 0, &test), E_FAIL);
		// 1 g_bitFlag
		m_pShaderCom->Bind_RawValue_ByHandle(g_bitFlag, &test, sizeof(_uint));
		//여기서 다 구분하고 던져줘야되는데
		//여기서 던져준다
		//CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_BaseTexture", i, aiTextureType_SPECULAR), E_FAIL);

		//CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_CtrlTexture", i, aiTextureType_METALNESS), E_FAIL);

		//CHECK_FAILED(m_pModelCom->Bind_Morphs(m_pShaderCom, i), E_FAIL);

		/* 쉐이더의 n번 패스를 사용해서 그리겠다고 선언 */
		CHECK_FAILED(m_pShaderCom->Begin(m_vecObjPass[i]), E_FAIL);

		// CHECK_FAILED(m_pModelCom->Bind_Bones(m_pShaderCom, "g_BonesMatrices", i), E_FAIL);

		/* n번 패스가 사용하는 전역변수들을 전부 GPU에 바인딩 해준다 */
		CHECK_FAILED(m_pShaderCom->Bind_Resources(m_vecObjPass[i]), E_FAIL);

		/* 그린다 */
		CHECK_FAILED(m_pModelCom->Render(fTimeDelta, i), E_FAIL);
	}
	if (m_pCoverModelCom)
	{
		_float4x4 fBoxWorld = m_pTransformCom->Get_WorldFloat4x4();
		_matrix matBoxWorld = XMLoadFloat4x4(&fBoxWorld);

		_matrix matCoverWorld = m_matCoverLocal * matBoxWorld;

		_float4x4 fCoverWorld;
		XMStoreFloat4x4(&fCoverWorld, matCoverWorld);

		CHECK_FAILED(m_pShaderCom->Bind_Matrix_ByHandle(g_WorldMatrix, fCoverWorld), E_FAIL);

		for (_uint i = 0; i < m_pCoverModelCom->Get_NumMeshes(); i++)
		{
			_uint test = 0;
			CHECK_FAILED(m_pCoverModelCom->Bind_Material_FullSlot(m_pShaderCom, 0, i, aiTextureType_DIFFUSE, 0, &test), E_FAIL);
			CHECK_FAILED(m_pCoverModelCom->Bind_Material_FullSlot(m_pShaderCom, 2, i, aiTextureType_NORMALS, 0, &test), E_FAIL);
			CHECK_FAILED(m_pCoverModelCom->Bind_Material_FullSlot(m_pShaderCom, 4, i, aiTextureType_METALNESS, 0, &test), E_FAIL);
			CHECK_FAILED(m_pCoverModelCom->Bind_Material_FullSlot(m_pShaderCom, 10, i, aiTextureType_EMISSIVE, 0, &test), E_FAIL);
			CHECK_FAILED(m_pCoverModelCom->Bind_Material_FullSlot(m_pShaderCom, 3, i, aiTextureType_OPACITY, 0, &test), E_FAIL);
			CHECK_FAILED(m_pCoverModelCom->Bind_Material_FullSlot(m_pShaderCom, 6, i, aiTextureType_SHININESS, 0, &test), E_FAIL);
			CHECK_FAILED(m_pCoverModelCom->Bind_Material_FullSlot(m_pShaderCom, 5, i, aiTextureType_SPECULAR, 0, &test), E_FAIL);

			m_pShaderCom->Bind_RawValue_ByHandle(g_bitFlag, &test, sizeof(_uint));

			CHECK_FAILED(m_pShaderCom->Begin(m_vecObjPass[i]), E_FAIL);
			CHECK_FAILED(m_pShaderCom->Bind_Resources(m_vecObjPass[i]), E_FAIL);
			CHECK_FAILED(m_pCoverModelCom->Render(fTimeDelta, i), E_FAIL);
		}
	}

	return S_OK;
}

HRESULT Client::Item_Box::Render_Shadow(const _float fTimeDelta, _int iCascadeNum)
{
	CHECK_FAILED(m_pShaderCom->Bind_Matrix_ByHandle(g_WorldMatrix, m_pTransformCom->Get_WorldFloat4x4()), E_FAIL);

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();
	for (size_t i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pShaderCom->Begin(5, true)))
			return E_FAIL;

		CHECK_FAILED(m_pShaderCom->Bind_Resources(5), E_FAIL);
		CHECK_FAILED(m_pModelCom->Render(fTimeDelta, i), E_FAIL);
	}

	if (m_pCoverModelCom)
	{
		_float4x4 fBoxWorld = m_pTransformCom->Get_WorldFloat4x4();
		_matrix matBoxWorld = XMLoadFloat4x4(&fBoxWorld);
		_matrix matCoverWorld = m_matCoverLocal * matBoxWorld;

		_float4x4 fCoverWorld;
		XMStoreFloat4x4(&fCoverWorld, matCoverWorld);

		CHECK_FAILED(m_pShaderCom->Bind_Matrix_ByHandle(g_WorldMatrix, fCoverWorld), E_FAIL);

		_uint iCoverNumMeshes = m_pCoverModelCom->Get_NumMeshes();
		for (size_t i = 0; i < iCoverNumMeshes; i++)
		{
			if (FAILED(m_pShaderCom->Begin(5, true)))
				return E_FAIL;

			CHECK_FAILED(m_pShaderCom->Bind_Resources(5), E_FAIL);
			CHECK_FAILED(m_pCoverModelCom->Render(fTimeDelta, i), E_FAIL);
		}
	}

	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/

ordered_json Client::Item_Box::Get_ExtraData()
{
	ordered_json j = __super::Get_ExtraData();
	j["ItemID"] = m_iItemID;
	j["ItemTrigger"] = (int)m_eItemTrigger;
	return j;
}

//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Item_Box* Client::Item_Box::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	Item_Box* pInstance = new Item_Box(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"Item_Box 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* Client::Item_Box::Clone(void* pArg)
{
	Item_Box* pInstance = new Item_Box(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"Item_Box 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::Item_Box::Free()
{
	__super::Free();

	m_pGameInstance->PhysX_Remove_Actor(m_pPhysXActor);
	m_pGameInstance->Remove_StaticObject(this);
}
/******************************************************* 객체 반환 함수 *******************************************************/

#pragma region IInteractable
bool Client::Item_Box::IsInteractable()
{
	if (m_bIsOpened)
		return false;

	GameObject* pPlayer = m_pGameInstance->Get_Player();
	if (pPlayer)
	{
		_vector vPlayerPos = pPlayer->Get_Position();
		_vector vBoxPos = m_pTransformCom->Get_State(DIRECTION::POSITION);

		_float fPlayerY = XMVectorGetY(vPlayerPos);
		_float fBoxY = XMVectorGetY(vBoxPos);
		_float fDistY = fabs(fPlayerY - fBoxY);

		_float fMaxYOffSet = 1.5f;

		if (fDistY <= fMaxYOffSet)
		{
			return true;
		}
	}

	return false;
}

void Client::Item_Box::Enter_InteractionRange(GameObject* pPlayer)
{
	UI_MasterEvent Event;
	Event.m_EventTarget = UI_EVENT_TARGET::SYSTEM;
	Event.m_ActionName = "ActiveUI";
	Event.m_Text = L"PopUp_Chest";
	Event.m_bPersistent = true;
	Event.m_bFlag = true;
	m_pGameInstance->Publish(Event);
}

void Client::Item_Box::Stay_InteractionRange(GameObject* pPlayer, const _float& fTimeDelta)
{

}

void Client::Item_Box::Exit_InteractionRange(GameObject* pPlayer)
{
	//UI없애깅
	//WCOUT(L"현재 " << m_wstrName << L" 의 interaction 범위에서 나갔습니다.Exit_InteractionRange ");
	UI_MasterEvent Event;
	Event.m_EventTarget = UI_EVENT_TARGET::SYSTEM;
	Event.m_ActionName = "ActiveUI";
	Event.m_Text = L"PopUp_Chest";
	Event.m_bPersistent = true;
	Event.m_bFlag = false;
	m_pGameInstance->Publish(Event);

}

void Client::Item_Box::Enter_Interaction(GameObject* pPlayer)
{
	if (!m_bIsOpened)
	{
		m_bIsOpened = true;

		PlayerIntercation PlayerInteracitonEvent;
		PlayerInteracitonEvent.bEKeyDown = true;
		PlayerInteracitonEvent.eInteractionType = INTERACTION_TYPE::ITEM_BOX;
		m_pGameInstance->Publish(PlayerInteracitonEvent);

		ItemInfo* pItemInfo = ItemManager::GetInstance()->Get_ItemInfo(m_iItemID);
		if (pItemInfo)
		{
			InventoryManager::GetInstance()->Add_Item(pItemInfo);
			Safe_Delete(pItemInfo);
		}

		if (m_eItemTrigger != ITEM_TRIGGER::NONE)
		{
			ITEM_TRIGGER_EVENT TriggerEvent;
			TriggerEvent.eTrigger = m_eItemTrigger;
			m_pGameInstance->Publish(TriggerEvent);
		}
	}
}

void Client::Item_Box::Stay_Interaction(GameObject* pPlayer, const _float& fTimeDelta)
{
}

void Client::Item_Box::Exit_Interaction(GameObject* pPlayer)
{
	m_bPreInteraction = false;
	InteractionManager::GetInstance()->Reset_BestPriority();
}


#pragma endregion