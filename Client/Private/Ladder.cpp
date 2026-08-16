#include "Client_Define.h"
#include "Ladder.h"
#include "GameInstance.h"
#include "Level_Main.h"
#include "MinimapRenderComponent.h"
#include "Player.h"
#include "InteractionManager.h"
#include "UIObj_FadeScreen.h"
#include "UIObj_PopUp_Interaction.h"


CLadder::CLadder(ID3D11Device* pD, ID3D11DeviceContext* pC)
	: MapObject(pD, pC)
{
}

CLadder::CLadder(const CLadder& original)
	: MapObject(original)
{
}

HRESULT CLadder::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT CLadder::Initialize(void* arg)
{
	if (FAILED(__super::Initialize(arg)))
		return E_FAIL;

	if (nullptr != arg)
	{
		MAPOBJECT_DESC* pDesc = (MAPOBJECT_DESC*)arg;

		if (pDesc->jExtraData.contains("LadderLength"))
		{
			m_iTotalLength = pDesc->jExtraData["LadderLength"].get<_float>();
		}
		else
		{
			m_iTotalLength = 5.0f;
		}

		m_iLength = (_uint)ceil(m_iTotalLength / 2.0f);
		if (m_iLength < 2)
			m_iLength = 2;
	}
	else
	{
		m_iLength = 5;
	}

	if (FAILED(Ready_Components()))
		return E_FAIL;

	Vector4 vPos = m_pTransformCom->Get_State(DIRECTION::POSITION);

	static _uint namenum = 0;
	m_wstrName = L"Ladder_" + to_wstring(namenum++);

	m_eObjType = OBJTYPE::TYPE_MAP;

	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); ++i)
	{
		m_vecObjPass.push_back(0);
	}

	PHYSX_ACTOR_DESC actordesc;
	actordesc.matWorld = m_pTransformCom->Get_WorldFloat4x4();
	actordesc.pModel = m_pModelTopCom;
	m_pGameInstance->PhysX_Load_Static_Actor_Auto(actordesc);

	_matrix matOriginalWorld = m_pTransformCom->Get_WorldMatrix();

	for (_uint i = 0; i < m_iLength; ++i)
	{
		Model* pCurrentModel = (i == m_iLength - 1) ? m_pModelBottomCom : m_pModelMiddleCom;
		if (nullptr == pCurrentModel) continue;

		_matrix matOffset = XMMatrixTranslation(0.f, -2.0f * (_float)i, 0.f);
		_matrix matFinalWorld = matOffset * matOriginalWorld;

		_float4x4 matFinalFloat4x4;
		XMStoreFloat4x4(&matFinalFloat4x4, matFinalWorld);

		PHYSX_ACTOR_DESC bodyDesc;
		bodyDesc.matWorld = matFinalFloat4x4;
		bodyDesc.pModel = pCurrentModel;
		m_pGameInstance->PhysX_Load_Static_Actor_Auto(bodyDesc);
	}

	_vector vTop = m_pTransformCom->Get_State(DIRECTION::POSITION);
	XMStoreFloat3(&m_vTopPosition, vTop);

	_matrix matBottomOffset = XMMatrixTranslation(0.f, -2.0f * (_float)(m_iLength - 1), 0.f);
	_matrix matBottomWorld = matBottomOffset * matOriginalWorld;
	_vector vBottom = matBottomWorld.r[3];
	XMStoreFloat3(&m_vBottomPosition, vBottom);


	m_eInteractionType = INTERACTION_TYPE::LADDER;
	InteractionManager::GetInstance()->Register_InteractableObject(this);

	return S_OK;
}

HRESULT CLadder::Ready_Components()
{
	CHECK_FAILED(Add_Shader(L"Prototype_Component_Shader_VTXMesh"), E_FAIL);

	CHECK_FAILED(Add_Component(m_iLevel, L"Prototype_Component_Model_LadderTop", L"Com_Model_Top", RCAST(Component**)(&m_pModelTopCom)), E_FAIL);
	CHECK_FAILED(Add_Component(m_iLevel, L"Prototype_Component_Model_LadderMiddle", L"Com_Model_Middle", RCAST(Component**)(&m_pModelMiddleCom)), E_FAIL);
	CHECK_FAILED(Add_Component(m_iLevel, L"Prototype_Component_Model_LadderBottom", L"Com_Model_Bottom", RCAST(Component**)(&m_pModelBottomCom)), E_FAIL);

	m_pModelCom = m_pModelTopCom;

	if (m_pModelCom != nullptr)
	{
		m_pModelCom->LocalAABB_To_WorldAABB(m_pTransformCom->Get_WorldMatrix());
	}

	MinimapRenderComponent::MINIMAPDESC MinimapDesc;
	MinimapDesc.pOwner = this;
	MinimapDesc.eType = MinimapRenderComponent::ICON_TYPE::LADDER;
	MinimapDesc.m_bIsTrace = false;	//발자국남김
	CHECK_FAILED(Add_Component(_UINT(LEVEL::STATIC), Proto_Com_Minimap, Com_Minimap, RCAST(Component**)(&m_pMinimapRenderCom), &MinimapDesc), E_FAIL);

	return S_OK;
}

_int CLadder::Update_Priority(const _float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);

	//if (m_pGameInstance->KeyDown(DIK_F))
	//{
	//	Player* pPlayer = DCAST(Player*)(m_pGameInstance->Get_Player());
	//	
	//	CHECK_NULL_RESULT(pPlayer, 0);

	//	Transform* pPlayerTransform = pPlayer->Get_Transform();

	//	CHECK_NULL_RESULT(pPlayerTransform, 0);
	//	
	//	_vector vPlayerPos = pPlayerTransform->Get_State(DIRECTION::POSITION);
	//	_vector vTopPos = XMLoadFloat3(&m_vTopPosition);
	//	_vector vBottomPos = XMLoadFloat3(&m_vBottomPosition);

	//	_float fDistToTop = XMVectorGetX(XMVector3Length(vPlayerPos - vTopPos));
	//	_float fDistToBottom = XMVectorGetX(XMVector3Length(vPlayerPos - vBottomPos));

	//	_float fInteractLimit = 5.0f;

	//	if (fDistToTop <= fInteractLimit || fDistToBottom <= fInteractLimit)
	//	{
	//		_vector vLadderLook = XMVector3Normalize(m_pTransformCom->Get_State(DIRECTION::LOOK));  // 사다리의 앞(Z축)
	//		_vector vLadderUp = XMVector3Normalize(m_pTransformCom->Get_State(DIRECTION::UP));    // 사다리의 위(Y축)
	//		_vector vLadderRight = XMVector3Normalize(m_pTransformCom->Get_State(DIRECTION::RIGHT)); // 사다리의 오른쪽(X축)

	//		if (fDistToTop < fDistToBottom)
	//		{
	//			_vector vTeleportPos = vBottomPos + (vLadderUp * 0.5f) + (vLadderLook * -2.0f);
	//			pPlayer->Teleport(vTeleportPos);
	//		}
	//		else
	//		{
	//			_vector vTeleportPos = vTopPos + (vLadderUp * 3.0f) + (vLadderLook * 2.0f);
	//			pPlayer->Teleport(vTeleportPos);
	//		}
	//	}
	//}

	return 0;
}

_int CLadder::Update_Parallel(const _float fTimeDelta)
{
	__super::Update_Parallel(fTimeDelta);

	return 0;
}

_int CLadder::Update(const _float fDT)
{
	return __super::Update(fDT);
}

_int CLadder::Update_Late(const _float fDT)
{
	Add_RenderGroup(RENDER_GROUP::NONBLEND);
	Add_RenderGroup(RENDER_GROUP::SHADOW_BAKE);

	__super::Update_Late(fDT);
	m_pMinimapRenderCom->Update_Late(fDT);

	return 0;
}

HRESULT CLadder::Render(const _float fDT)
{
	if (false == m_bIsVisible)
		return S_OK;

	// CHECK_FAILED(GameObject::Bind_ProjectionMatrices(m_pShaderCom), E_FAIL);

	_matrix matOriginalWorld = m_pTransformCom->Get_WorldMatrix();

	// Top
	if (m_pModelTopCom != nullptr)
	{
		_float4x4 matFinalFloat4x4;

		XMStoreFloat4x4(&matFinalFloat4x4, matOriginalWorld);

		if (FAILED(m_pShaderCom->Bind_Matrix_ByHandle(g_WorldMatrix, matFinalFloat4x4)))
			return E_FAIL;

		for (_uint j = 0; j < m_pModelTopCom->Get_NumMeshes(); j++)
		{
			_uint bitflag = 0;
			CHECK_FAILED(m_pModelTopCom->Bind_Material_FullSlot(m_pShaderCom, 0, j, aiTextureType_DIFFUSE, 0, &bitflag), E_FAIL);
			CHECK_FAILED(m_pModelTopCom->Bind_Material_FullSlot(m_pShaderCom, 2, j, aiTextureType_NORMALS, 0, &bitflag), E_FAIL);
			m_pShaderCom->Bind_RawValue_ByHandle(g_bitFlag, &bitflag, sizeof(_uint));
			_uint iPass = 7;
			CHECK_FAILED(m_pShaderCom->Begin(iPass), E_FAIL);
			CHECK_FAILED(m_pShaderCom->Bind_Resources(iPass), E_FAIL);
			CHECK_FAILED(m_pModelTopCom->Render(fDT, j), E_FAIL);
		}
	}

	// 사다리 본체
	for (_uint i = 0; i < m_iLength; ++i)
	{
		Model* pCurrentModel = nullptr;

		if (i == m_iLength - 1)
			pCurrentModel = m_pModelBottomCom;
		else
			pCurrentModel = m_pModelMiddleCom;

		if (nullptr == pCurrentModel) continue;

		_matrix matOffset = XMMatrixTranslation(0.f, -2.0f * (_float)i, 0.f);
		_matrix matFinalWorld = matOffset * matOriginalWorld;

		_float4x4 matFinalFloat4x4;
		XMStoreFloat4x4(&matFinalFloat4x4, matFinalWorld);

		if (FAILED(m_pShaderCom->Bind_Matrix_ByHandle(g_WorldMatrix, matFinalFloat4x4)))
			return E_FAIL;

		for (_uint j = 0; j < pCurrentModel->Get_NumMeshes(); j++)
		{
			_uint bitflag = 0;
			CHECK_FAILED(pCurrentModel->Bind_Material_FullSlot(m_pShaderCom, 0, j, aiTextureType_DIFFUSE), E_FAIL);
			CHECK_FAILED(pCurrentModel->Bind_Material_FullSlot(m_pShaderCom, 2, j, aiTextureType_NORMALS), E_FAIL);
			m_pShaderCom->Bind_RawValue_ByHandle(g_bitFlag, &bitflag, sizeof(_uint));
			_uint iPass = 7;//_uint iPass = (m_vecObjPass.size() > j) ? m_vecObjPass[j] : 0;
			CHECK_FAILED(m_pShaderCom->Begin(iPass), E_FAIL);
			CHECK_FAILED(m_pShaderCom->Bind_Resources(iPass), E_FAIL);
			CHECK_FAILED(pCurrentModel->Render(fDT, j), E_FAIL);
		}
	}

	return S_OK;
}

HRESULT CLadder::Render_Shadow(const _float fDT, _int iCascadeNum)
{
	if (false == m_bIsVisible)
		return S_OK;

	// CHECK_FAILED(GameObject::Bind_ProjectionMatrices(m_pShaderCom), E_FAIL);

	_matrix matOriginalWorld = m_pTransformCom->Get_WorldMatrix();

	// Top
	if (m_pModelTopCom != nullptr)
	{
		_float4x4 matFinalFloat4x4;

		XMStoreFloat4x4(&matFinalFloat4x4, matOriginalWorld);

		if (FAILED(m_pShaderCom->Bind_Matrix_ByHandle(g_WorldMatrix, matFinalFloat4x4)))
			return E_FAIL;

		for (_uint j = 0; j < m_pModelTopCom->Get_NumMeshes(); j++)
		{
			_uint bitflag = 0;
			CHECK_FAILED(m_pModelTopCom->Bind_Material_FullSlot(m_pShaderCom, 0, j, aiTextureType_DIFFUSE, 0, &bitflag), E_FAIL);
			CHECK_FAILED(m_pModelTopCom->Bind_Material_FullSlot(m_pShaderCom, 2, j, aiTextureType_NORMALS, 0, &bitflag), E_FAIL);
			m_pShaderCom->Bind_RawValue_ByHandle(g_bitFlag, &bitflag, sizeof(_uint));
			_uint iPass = 5;
			CHECK_FAILED(m_pShaderCom->Begin(iPass), E_FAIL);
			CHECK_FAILED(m_pShaderCom->Bind_Resources(iPass), E_FAIL);
			CHECK_FAILED(m_pModelTopCom->Render(fDT, j), E_FAIL);
		}
	}

	// 사다리 본체
	for (_uint i = 0; i < m_iLength; ++i)
	{
		Model* pCurrentModel = nullptr;

		if (i == m_iLength - 1)
			pCurrentModel = m_pModelBottomCom;
		else
			pCurrentModel = m_pModelMiddleCom;

		if (nullptr == pCurrentModel) continue;

		_matrix matOffset = XMMatrixTranslation(0.f, -2.0f * (_float)i, 0.f);
		_matrix matFinalWorld = matOffset * matOriginalWorld;

		_float4x4 matFinalFloat4x4;
		XMStoreFloat4x4(&matFinalFloat4x4, matFinalWorld);

		if (FAILED(m_pShaderCom->Bind_Matrix_ByHandle(g_WorldMatrix, matFinalFloat4x4)))
			return E_FAIL;

		for (_uint j = 0; j < pCurrentModel->Get_NumMeshes(); j++)
		{
			_uint bitflag = 0;
			CHECK_FAILED(pCurrentModel->Bind_Material_FullSlot(m_pShaderCom, 0, j, aiTextureType_DIFFUSE), E_FAIL);
			CHECK_FAILED(pCurrentModel->Bind_Material_FullSlot(m_pShaderCom, 2, j, aiTextureType_NORMALS), E_FAIL);
			m_pShaderCom->Bind_RawValue_ByHandle(g_bitFlag, &bitflag, sizeof(_uint));
			_uint iPass = 5;
			CHECK_FAILED(m_pShaderCom->Begin(iPass), E_FAIL);
			CHECK_FAILED(m_pShaderCom->Bind_Resources(iPass), E_FAIL);
			CHECK_FAILED(pCurrentModel->Render(fDT, j), E_FAIL);
		}
	}

	return S_OK;
	return S_OK;
}

ordered_json CLadder::Get_ExtraData()
{
	ordered_json j = __super::Get_ExtraData();
	j["LadderLength"] = m_iTotalLength;
	return j;
}

CLadder* CLadder::Create(ID3D11Device* pD, ID3D11DeviceContext* pC, LEVEL _level)
{
	CLadder* pInstance = new CLadder(pD, pC);
	if (FAILED(pInstance->Initialize_Prototype(_level)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

GameObject* CLadder::Clone(void* arg)
{
	CLadder* pInstance = new CLadder(*this);
	if (FAILED(pInstance->Initialize(arg)))
	{
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}


#pragma region IInteractable
bool CLadder::IsInteractable()
{
	//사다리 거리비교 추가하긴했는데 지금 테스트가안대유.. 이상하면주석처리 ㅠ 하고 트리거로 하든가해야할듯
	//거리비교말고 다른 진입조건이 없으므로 true
	//y값비교?

	GameObject* pPlayer = m_pGameInstance->Get_Player();
	if (pPlayer)
	{
		//top 과 bottom중에서 가까운점 구하기
		_vector PlayerPos = pPlayer->Get_Position();


		_float PlayerToBottomDistance = XMVectorGetX(XMVector3LengthSq(PlayerPos - m_vBottomPosition));
		_float PlayerToTopDistance = XMVectorGetX(XMVector3LengthSq(PlayerPos - m_vTopPosition));

		if (PlayerToBottomDistance < PlayerToTopDistance)
		{
			//아래랑 더 가까움
			_float PlayerY = XMVectorGetY(PlayerPos);
			_float Distance = fabs(PlayerY - m_vBottomPosition.y);

			if (Distance <= YOffSet)
				return true;

		}

		else
		{
			//top이랑 더 가까움
			_float PlayerY = XMVectorGetY(PlayerPos);
			_float Distance = fabs(PlayerY -m_vTopPosition.y);

			if (Distance <= YOffSet)
				return true;
		}
	
	}
	return false;
}

void CLadder::Enter_InteractionRange(GameObject* pPlayer)
{
	

	UI_MasterEvent Event;
	Event.m_EventTarget = UI_EVENT_TARGET::SYSTEM;
	Event.m_ActionName = "ActiveUI";
	Event.m_Text = L"PopUp_LadderUp";
	Event.m_bPersistent = true;
	Event.m_bFlag = true;
	m_pGameInstance->Publish(Event);

	m_eLadderMode = LadderMode::END;
}

void CLadder::Stay_InteractionRange(GameObject* pPlayer,const _float& fTimeDelta)
{
	//UI띄우깅 (내려가기/올라가기 판단)
	Player* ppPlayer = DCAST(Player*)(pPlayer);
	CHECK_JUST_NULL(pPlayer);

	Transform* pPlayerTransform = pPlayer->Get_Transform();
	CHECK_JUST_NULL(pPlayerTransform);

	_vector vPlayerPos = pPlayerTransform->Get_State(DIRECTION::POSITION);
	_vector vTopPos = XMLoadFloat3(&m_vTopPosition);
	_vector vBottomPos = XMLoadFloat3(&m_vBottomPosition);

	_float fDistToTop = XMVectorGetX(XMVector3Length(vPlayerPos - vTopPos));
	_float fDistToBottom = XMVectorGetX(XMVector3Length(vPlayerPos - vBottomPos));

	_float fInteractLimit = 5.0f;

	if (fDistToTop <= fInteractLimit || fDistToBottom <= fInteractLimit)
	{
		_vector vLadderLook = XMVector3Normalize(m_pTransformCom->Get_State(DIRECTION::LOOK));  // 사다리의 앞(Z축)
		_vector vLadderUp = XMVector3Normalize(m_pTransformCom->Get_State(DIRECTION::UP));    // 사다리의 위(Y축)
		_vector vLadderRight = XMVector3Normalize(m_pTransformCom->Get_State(DIRECTION::RIGHT)); // 사다리의 오른쪽(X축)

		LadderMode newMode = (fDistToTop < fDistToBottom) ? LadderMode::LADDERDOWN : LadderMode::LADDERUP;

		if (newMode != m_eLadderMode)
		{
			if(newMode== LadderMode::LADDERDOWN)
				Show_LadderDown_UI();

			else
				Show_LadderUp_UI();

			m_eLadderMode = newMode;
		}
		
	}

}

void CLadder::Exit_InteractionRange(GameObject* pPlayer)
{
	//UI없애깅
	//WCOUT(L"현재 " << m_wstrName << L" 의 interaction 범위에서 나갔습니다.Exit_InteractionRange ");
	UI_MasterEvent Event;
	Event.m_EventTarget = UI_EVENT_TARGET::SYSTEM;
	Event.m_ActionName = "ActiveUI";
	Event.m_Text = L"PopUp_LadderUp";
	Event.m_bPersistent = true;
	Event.m_bFlag = false;
	m_pGameInstance->Publish(Event);

	m_eLadderMode = LadderMode::END;

}

void CLadder::Enter_Interaction(GameObject* pPlayer)
{
		
	WCOUT(L"실행 모드: " << (m_eLadderMode == LadderMode::LADDERDOWN ? L"DOWN" : L"UP"));

	_vector vLadderLook = XMVector3Normalize(m_pTransformCom->Get_State(DIRECTION::LOOK));
	_vector vLadderUp = XMVector3Normalize(m_pTransformCom->Get_State(DIRECTION::UP));
	_vector vRight = XMVector3Normalize(m_pTransformCom->Get_State(DIRECTION::RIGHT));
	_vector vTopPos = XMLoadFloat3(&m_vTopPosition);
	_vector vBottomPos = XMLoadFloat3(&m_vBottomPosition);

	_vector vTeleportPos = XMVectorSet(0, 0, 0, 1);
	//_float fFadeTime = 1.0f;

	_float fYOffset = { 0.f };
	_float fLookOffset = { 0.f };
	//m_eLadderMode
	if (m_eLadderMode == LadderMode::LADDERUP)
	{
		// 올라갈때
		vTeleportPos = vBottomPos + (vLadderUp * -1.94f) + (vLadderLook * -0.41f);

		COUT("Bottom텔포 위치 x: " << XMVectorGetX(vTeleportPos));
		COUT("Bottom텔포 위치 y: " << XMVectorGetY(vTeleportPos));
		COUT("Bottom텔포 위치 z: " << XMVectorGetZ(vTeleportPos));
	}
	else if (m_eLadderMode == LadderMode::LADDERDOWN)
	{
		// 내려갈때 + (vLadderLook * +1.2f)
		vTeleportPos = vTopPos + (vRight * -0.35f) + (vLadderUp * 0.15f) + (vLadderLook * -0.5f);
		COUT("Top텔포 위치 x: " << XMVectorGetX(vTeleportPos));
		COUT("Top텔포 위치 y: " << XMVectorGetY(vTeleportPos));
		COUT("Top텔포 위치 z: " << XMVectorGetZ(vTeleportPos));
	}

	PlayerIntercation PlayerInteracitonEvent;
	PlayerInteracitonEvent.bEKeyDown = true;
	PlayerInteracitonEvent.eInteractionType = INTERACTION_TYPE::LADDER;
	if (m_eLadderMode == LadderMode::LADDERUP)
		PlayerInteracitonEvent.bClimbUp = true;
	XMStoreFloat4(&PlayerInteracitonEvent.vSnapPos, vTeleportPos);
	_vector vLook = XMVector3Normalize(m_pTransformCom->Get_State(DIRECTION::LOOK));
	if (m_eLadderMode == LadderMode::LADDERDOWN) //내려갈때는 룩방향 반대로
		vLook = XMVectorNegate(vLook);
	XMStoreFloat3(&PlayerInteracitonEvent.vLook, vLook);

	PlayerInteracitonEvent.vLadderBottomPos = m_vBottomPosition;
	PlayerInteracitonEvent.vLadderTopPos = m_vTopPosition;


	m_pGameInstance->Publish(PlayerInteracitonEvent);


	// 페이드 및 텔레포트 실행
	//UIObj_FadeScreen::FadeScreenEvent Event;
	//Event.eType = UIObj_FadeScreen::FadeScreenEventType::AUTO_FADE;
	//Event.m_fSecond = fFadeTime;
	//Event.m_EndFunc = [pPlayer, vTeleportPos]()
	//	{
	//		Player* ppPlayer = DCAST(Player*)(pPlayer);
	//		if (ppPlayer) ppPlayer->Teleport(vTeleportPos);
	//		IInteractable* pObj = InteractionManager::GetInstance()->Get_Current_Interaction_Target();
	//		if (pObj) pObj->Exit_Interaction(pPlayer);

	//	};

	//m_pGameInstance->Publish(Event);
	m_pGameInstance->Play_Sound("Ladder_Down_Fast", 0.4f, false);
	

}
void CLadder::Stay_Interaction(GameObject* pPlayer,const _float& fTimeDelta)
{
	//WCOUT(L"현재 " << m_wstrName << L" 의 interaction 중입니다.Stay_Interaction ");


}

void CLadder::Exit_Interaction(GameObject* pPlayer)
{
	//머ㅜ......나중에....플레이어상태도려녹ㅎ기..??머..
	//WCOUT(L"현재 " << m_wstrName << L" 의 Exit_Interaction ");
	m_bPreInteraction = false;
	m_eLadderMode = LadderMode::END;
	InteractionManager::GetInstance()->Reset_BestPriority();

}


#pragma endregion

void CLadder::Free()
{
	if (m_bIsClone)
		InteractionManager::GetInstance()->UnRegisterInteractable(this);
	__super::Free();
}

void CLadder::Show_LadderUp_UI()
{
	UIObj_PopUp_Interaction::PopUp_Interaction_Event PopUpEvent;
	PopUpEvent.m_eType = UIObj_PopUp_Interaction::Interaction_Type::LADDER_UP;
	PopUpEvent.m_Text = L"올라가기";
	m_pGameInstance->Publish(PopUpEvent);


	UI_MasterEvent Event;
	Event.m_EventTarget = UI_EVENT_TARGET::SYSTEM;
	Event.m_ActionName = "ActiveUI";
	Event.m_Text = L"PopUp_LadderUp";
	Event.m_bPersistent = true;
	Event.m_bFlag = true;
	m_pGameInstance->Publish(Event);
}

void CLadder::Show_LadderDown_UI()
{
	UIObj_PopUp_Interaction::PopUp_Interaction_Event PopUpEvent;
	PopUpEvent.m_eType = UIObj_PopUp_Interaction::Interaction_Type::LADDER_UP;
	PopUpEvent.m_Text = L"내려가기";
	m_pGameInstance->Publish(PopUpEvent);

	UI_MasterEvent Event;
	Event.m_EventTarget = UI_EVENT_TARGET::SYSTEM;
	Event.m_ActionName = "ActiveUI";
	Event.m_Text = L"PopUp_LadderUp";
	Event.m_bPersistent = true;
	Event.m_bFlag = true;
	m_pGameInstance->Publish(Event);
}
