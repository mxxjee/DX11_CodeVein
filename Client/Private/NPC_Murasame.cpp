#include "Client_Define.h"
#include "NPC_Murasame.h"
#include "GameInstance.h"
#include "Mesh.h"
#include "DialogueManager.h"
#include "UIObj_NpcDialogue.h"
#include "Camera_NPC.h"
#include "UIObj_Window_Shop.h"
#include "ShopManager.h"


Client::NPC_Murasame::NPC_Murasame(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: NPC(pDevice, pContext)
{
}

Client::NPC_Murasame::NPC_Murasame(const NPC_Murasame& original)
	: NPC(original)
{
}

HRESULT Client::NPC_Murasame::Initialize_Prototype(LEVEL _level)
{
	CHECK_FAILED(NPC::Initialize_Prototype(_level), E_FAIL);
	return S_OK;
}

HRESULT Client::NPC_Murasame::Initialize(void* _arg)
{
	NPC_DESC* pArg = (NPC_DESC*)_arg;

	pArg->tControllerDesc.eActorType = PX_ACTOR_TYPE::STATIC_MAP;

	pArg->tControllerDesc.iObjectID = m_iObjectID;
	pArg->tControllerDesc.fRadius = 0.3f;
	pArg->tControllerDesc.fHeight = 0.4f;

	pArg->tControllerDesc.fSlopeLimit = 2.f;
	pArg->tControllerDesc.fStepOffset = 0.2f;

	CHECK_FAILED(NPC::Initialize(_arg), E_FAIL);

	m_pController = Create_Controller(pArg->tControllerDesc);

	if (m_pController && m_pTransformCom)
	{
		_vector vPos = m_pTransformCom->Get_State(DIRECTION::POSITION);

		m_pController->setFootPosition(physx::PxExtendedVec3(
			XMVectorGetX(vPos), XMVectorGetY(vPos), XMVectorGetZ(vPos)
		));
	}

    CHECK_FAILED(Ready_Components(), E_FAIL);
    CHECK_FAILED(Ready_States(), E_FAIL);
    CHECK_FAILED(Ready_ActionHashs(), E_FAIL);
    m_pTransformCom->Rotation(0.f, XMConvertToRadians(-90.f), 0.f);

	m_pNPCData = DialogueManager::GetInstance()->Get_DialogueData("../../DataFiles/NPC_Data/Murasame.json");
	//샵메뉴 오프셋설정..(거의 고정스)!
	UIObj_Window_Shop::WINDOWSHOPEVENT windowShopEvent;
	windowShopEvent.eType = UIObj_Window_Shop::WINDOWSHOPEVNETTPYE::INITIALIZE;
	windowShopEvent.m_pTarget = this;

	
	windowShopEvent.TargetOffSet = _float3(m_pNPCData->OffSet.x, m_pNPCData->OffSet.y,1.3f);
	m_pGameInstance->Publish(windowShopEvent);


	return S_OK;
}

_int Client::NPC_Murasame::Update_Priority(_float fTimeDelta)
{
	if (m_bIsActive == false) return -1;
	__super::Update_Priority(fTimeDelta);
	return 0;
}

_int Client::NPC_Murasame::Update_Parallel(const _float fTimeDelta)
{
	if (m_bIsActive == false) return -1;
	__super::Update_Parallel(fTimeDelta);
	return 0;
}

_int Client::NPC_Murasame::Update(const _float fTimeDelta)
{
	if (m_bIsActive == false) return -1;
	__super::Update(fTimeDelta);

	Update_LookAt(fTimeDelta);

	m_pModelCom->Play_Animation_CS(fTimeDelta);

	_vector vRootDelta = Calculate_RootPos(true, true, false);

	if (m_pController)
	{
		physx::PxControllerFilters filters;
		m_pController->move(physx::PxVec3(XMVectorGetX(vRootDelta), XMVectorGetY(vRootDelta), XMVectorGetZ(vRootDelta)), 0.001f, fTimeDelta, filters);
	}

	return 0;
}

void Client::NPC_Murasame::Update_LookAt(const _float fTimeDelta)
{
	GameObject* pPlayer = m_pGameInstance->Get_Player();
	if (!pPlayer) return;

	_vector vPlayerPos = pPlayer->Get_Transform()->Get_State(DIRECTION::POSITION);
	_vector vMyPos = m_pTransformCom->Get_State(DIRECTION::POSITION);

	//좌우계산.y값은 무시
	_vector vDirToPlayerXZ = vPlayerPos - vMyPos;
	vDirToPlayerXZ = XMVectorSetY(vDirToPlayerXZ, 0.f);
	vDirToPlayerXZ = XMVector3Normalize(vDirToPlayerXZ);

	_vector vLookXZ = m_pTransformCom->Get_State(DIRECTION::LOOK);
	vLookXZ = XMVectorSetY(vLookXZ, 0.f);
	vLookXZ = XMVector3Normalize(vLookXZ);

	_vector vRightXZ = m_pTransformCom->Get_State(DIRECTION::RIGHT);
	vRightXZ = XMVectorSetY(vRightXZ, 0.f);
	vRightXZ = XMVector3Normalize(vRightXZ);

	_float fDot = XMVectorGetX(XMVector3Dot(vLookXZ, vDirToPlayerXZ));
	fDot = max(-1.f, min(1.f, fDot));
	_float fYaw = XMConvertToDegrees(acosf(fDot));

	_float fRightDot = XMVectorGetX(XMVector3Dot(vRightXZ, vDirToPlayerXZ));
	if (fRightDot > 0.f) fYaw *= -1.f;

	//위아래 보는모션도 있길래 만들어놨는데 플레이어가 키큰캐릭터인거 아니면 안쓸듯 상하계산
	_vector vDirToPlayer3D = XMVector3Normalize(vPlayerPos - vMyPos);
	_float fPitch = XMConvertToDegrees(asinf(XMVectorGetY(vDirToPlayer3D)));

	_uint iTargetAnimIndex = 0;
	_bool bIsTurnAnim = false;
	_float fTurnThreshold = 60.f; //발을 움직여 회전해야 하는 각도

	// 크게 벗어나면 몸 전체 턴
	if (fYaw > fTurnThreshold)
	{
		if (fYaw > 135.f) iTargetAnimIndex = 7;
		else iTargetAnimIndex = 6;
		bIsTurnAnim = true;
	}
	else if (fYaw < -fTurnThreshold)
	{
		if (fYaw < -135.f) iTargetAnimIndex = 10;
		else iTargetAnimIndex = 9;
		bIsTurnAnim = true;
	}
	// 적당한 각도면 고개/상체만 회전 근데 동작안함,,,아마도
	else
	{
		if (fYaw > 30.f)
		{
			iTargetAnimIndex = 2; // Offset L
		}
		else if (fYaw < -30.f)
		{
			iTargetAnimIndex = 3; // Offset R
		}
		else
		{
			if (fPitch < -20.f) iTargetAnimIndex = 1;      // Offset F
			else if (fPitch > 20.f) iTargetAnimIndex = 4;  // Offset U
			else iTargetAnimIndex = 0;                     // 정면 
		}
	}

	// 애니메이션 적용
	if (m_iCurrentAnimIndex != iTargetAnimIndex)
	{
		if (bIsTurnAnim)
		{
			m_pModelCom->Clear_AnimationUpper();
			m_pModelCom->Set_Animation_CS(iTargetAnimIndex, false);
		}
		else
		{
			m_pModelCom->Set_Animation_CS(0, true);

			if (iTargetAnimIndex == 0)
			{
				m_pModelCom->Clear_AnimationUpper();
			}
			else
			{
				m_pModelCom->Set_AnimationUpper_CS(iTargetAnimIndex, true, 1.0f, true, 0.2f, 1.0f);
			}
		}
		m_iCurrentAnimIndex = iTargetAnimIndex;
	}
}

_int Client::NPC_Murasame::Update_Late(const _float fTimeDelta)
{
	if (m_bIsActive == false) return -1;
	__super::Update_Late(fTimeDelta);

	return 0;
}

HRESULT Client::NPC_Murasame::Render(const _float fTimeDelta)
{
	if (FAILED(__super::Render(fTimeDelta))) return E_FAIL;

	for (_uint i = 0; i < m_pModelCom->Get_NumMeshes(); i++)
	{
		_uint iBitFlag = 0;

		if (m_vecObjPass[i] == 6) // 눈동자
		{
			m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 0, i, aiTextureType_DIFFUSE);  // C
			m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 3, i, aiTextureType_OPACITY);  // W
			m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 10, i, aiTextureType_EMISSIVE);// H
			m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 5, i, aiTextureType_SPECULAR); // S

			if (i == 3) iBitFlag |= (1 << 0);
		}
		else
		{
			CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 0, i, aiTextureType_DIFFUSE, 0, &iBitFlag), E_FAIL);
			CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 1, i, aiTextureType_NORMALS, 0, &iBitFlag), E_FAIL);
			CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 10, i, aiTextureType_EMISSIVE, 0, &iBitFlag), E_FAIL);
			CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 11, i, aiTextureType_SHININESS, 0, &iBitFlag), E_FAIL);
			CHECK_FAILED(m_pModelCom->Bind_Material_FullSlot(m_pShaderCom, 4, i, aiTextureType_METALNESS, 0, &iBitFlag), E_FAIL);
			//AO나중에 처리
			//CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_ShadCTexture", i, aiTextureType_EMISSIVE), E_FAIL);
			//CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_BaseTexture", i, aiTextureType_SPECULAR), E_FAIL);
			//CHECK_FAILED(m_pModelCom->Bind_Material(m_pShaderCom, "g_CtrlTexture", i, aiTextureType_METALNESS), E_FAIL);
		}

		m_pShaderCom->Bind_RawValue_ByHandle(g_bitFlag, &iBitFlag, sizeof(_uint));

		CHECK_FAILED(m_pShaderCom->Begin(m_vecObjPass[i]), E_FAIL);
		CHECK_FAILED(m_pModelCom->Bind_CombiendAndOffsetSRV(m_pShaderCom), E_FAIL);
		CHECK_FAILED(m_pModelCom->Bind_ReampSRV(m_pShaderCom, i), E_FAIL);
		CHECK_FAILED(m_pShaderCom->Bind_Resources(m_vecObjPass[i]), E_FAIL);
		CHECK_FAILED(m_pModelCom->Render(fTimeDelta, i), E_FAIL);
	}
	return S_OK;
}

HRESULT Client::NPC_Murasame::Ready_Components()
{
	__super::Ready_Components();

	const _wstring NPC_MURASAME = L"Prototype_Component_Model_NPC_Murasame";
	Add_Model(NPC_MURASAME);
	Add_Shader(Proto_Com_Shader_VTXAnimMesh);

	m_pModelCom->Set_Animation_CS(0, true);

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();
	Set_VecObjPassSize(iNumMeshes);
	Set_AllPass_VecObjPass(7);

	Set_Pass_VecObjPass(3, 6);
	Set_Pass_VecObjPass(4, 6);


	return S_OK;
}

HRESULT Client::NPC_Murasame::Ready_States()
{
	return S_OK;
}

#pragma region IInteractable
bool Client::NPC_Murasame::IsInteractable()
{
	return true;
}

void Client::NPC_Murasame::Enter_InteractionRange(GameObject* pPlayer)
{
	__super::Enter_InteractionRange(pPlayer);

}

void Client::NPC_Murasame::Stay_InteractionRange(GameObject* pPlayer, const _float& fTimeDelta)
{
    //인터렉션중이라면 판단x
    
    CHECK_TRUE(m_bPreInteraction);

        //들어와있느데 창이 비활성화되어있다면 켜주는명령
    UI_MasterEvent Event;
    Event.m_EventTarget = UI_EVENT_TARGET::SYSTEM;
    Event.m_ActionName = "ActiveUI";
    Event.m_Text = L"PopUp_NPC";
    Event.m_bPersistent = true;
    Event.m_bFlag = true;
    m_pGameInstance->Publish(Event);
}

void Client::NPC_Murasame::Exit_InteractionRange(GameObject* pPlayer)
{
	__super::Exit_InteractionRange(pPlayer);
	DialogueManager::GetInstance()->Exit_Dialogue();
}

void Client::NPC_Murasame::Enter_Interaction(GameObject* pPlayer)
{
	m_pGameInstance->Play_Sound("murasame_talk_normal13_0020_murasame_Play", 0.5f, false);
	DialogueManager::GetInstance()->Start_Dialogue(this, m_pNPCData);
}

void Client::NPC_Murasame::Stay_Interaction(GameObject* pPlayer, const _float& fTimeDelta)
{
   /* if (m_pGameInstance->KeyDown(DIK_ESCAPE))
    {
        DialogueManager::GetInstance()->Exit_Dialogue();
        Exit_Interaction(nullptr);
    }*/
    if (m_pGameInstance->KeyDown(DIK_E))
    {
       DialogueManager::GetInstance()->Next_Dialogue();

		
	}
}

void Client::NPC_Murasame::Exit_Interaction(GameObject* pPlayer)
{
	__super::Exit_Interaction(pPlayer);
	//머ㅜ......나중에....플레이어상태도려녹ㅎ기..??머..
	//WCOUT(L"현재 " << m_wstrName << L" 의 Exit_Interaction ");
	m_bPreInteraction = false;

	// 카메라 교체된채로 Exit하면 원래 카메라로 돌리기
	if (m_bIsStartInterAction == true)
	{
		m_bIsStartInterAction = false;
		m_pGameInstance->Camera_Change(_UINT(CAMERA::PLAYER_FOCUS_01));
		m_pGameInstance->Play_Sound("murasame_talk_buildup01_e0020_murasame_Play", 0.5f, false);
	}
}



#pragma endregion

bool        Client::NPC_Murasame::DoAction(string ActionName)
{
    size_t TargetKey = hash<string>{}(ActionName);
    auto iter = m_ActionHashs.find(TargetKey);

    CHECK_TRUE_RESULT(iter == m_ActionHashs.end(),false);



    //동작수행..
    iter->second();

    return true;

}

NPC_Murasame* Client::NPC_Murasame::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	NPC_Murasame* pInstance = new NPC_Murasame(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(_level))) {
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}

GameObject* Client::NPC_Murasame::Clone(void* pArg)
{
	NPC_Murasame* pInstance = new NPC_Murasame(*this);
	if (FAILED(pInstance->Initialize(pArg))) {
		Safe_Release(pInstance);
		return nullptr;
	}
	return pInstance;
}


void Client::NPC_Murasame::Free()
{
    __super::Free();
}

HRESULT     Client::NPC_Murasame::Ready_ActionHashs()
{
    m_ActionHashs.emplace(hash<string>{}("OPEN_SHOP"), [this]()
        {
            //현재 창 닫구
            DialogueManager::GetInstance()->Close_SelectionMenu();
            
            //샵 메뉴열어주시요
			UI_MasterEvent Event;
			Event.m_EventTarget = UI_EVENT_TARGET::SYSTEM;
			Event.m_ActionName = "OpenWindow";
			Event.m_Text = L"Window_shop";
			Event.m_bPersistent = true;

			m_pGameInstance->Publish(Event);

			//shopmanager를통해서 슬롯그리드판단!!(다른곳ㄱ에넣었더니 타이밍문제로 focus안되엇어도 다 켜져서 여기다놓음)
			//push_window함수내부에서 setvisible을 다떄려버리넴

			//킬때마다 weapon으로 강제세팅
			ShopManager::GetInstance()->Set_FocusCategory(ITEM_CATEGORY::EQUIP_WEAPON,true);

			//첫 슬롯정보로 세팅
			ItemInfo* pFirstItem = ShopManager::GetInstance()->Get_Item_InShop(ITEM_CATEGORY::EQUIP_WEAPON, 0);
			ShopManager::GetInstance()->Set_FocusItem(pFirstItem);

	
        });

    m_ActionHashs.emplace(hash<string>{}("OPEN_CELL"), []()
        {

        });

    m_ActionHashs.emplace(hash<string>{}("NONE"), [this]()
        {
            //현재 창 닫구
            DialogueManager::GetInstance()->Exit_Dialogue();
            Exit_Interaction(nullptr);

            //리턴하세유
            return;
        });

    return S_OK;
}

void        Client::NPC_Murasame::Start_NPCCamera()
{
	// 처음 InterAction하는거면 카메라 교체
	if (m_bIsStartInterAction == false)
	{
		m_bIsStartInterAction = true;
		m_pGameInstance->Camera_Change(_UINT(CAMERA::NPC_SHOP));

		// 카메라의 목표 위치랑 카메라가 바라볼 위치 보내주기
		Camera_NPC::TRANSITION_DESC desc;
		// TargetPosition은 Json에 있는 Offset기준으로 맞춰도 될듯
		// 민지가 해죠잉 이 값만 바꾸면 돼
		desc.vTargetPosition = _float4(0.f, 1.8f, -9.8f, 1.f);
		desc.vTargetLookAt = _float4(1.f, 2.0f, -9.8f, 1.f);

		// 움직여라 카메라여
		CAST(Camera_NPC*)(m_pGameInstance->Get_CurrentCamera())->Start_Transition(desc);
	}
}