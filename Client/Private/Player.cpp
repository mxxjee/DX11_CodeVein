#include "Client_Define.h"
#include "Player.h"
#include "Player_MasterRig.h"
#include "Player_Body.h"
#include "Player_Hair.h"
#include "Player_Head.h"
#include "Player_Outer.h"
#include "Player_BloodWeapon.h"
#include "Player_Weapon.h"
#include "PWeapon_Bayonet.h"
#include "PWeapon_GreatSword.h"
#include "PWeapon_Halberd.h"
#include "PWeapon_Hammer.h"
#include "PWeapon_Sword.h"
#include "Player_Injection.h"
#include "PhysX_Function.h"

#include "Player_Stat.h"

#include "Player_Idle.h"
#include "Player_Run.h"
#include "Player_Walk.h"
#include "Player_Dash.h"
#include "Player_Roll.h"
#include "Player_BackStep.h"
#include "Player_AttackNormal.h"
#include "Player_AttackStrong.h"
#include "Player_AttackDodgeF.h"
#include "Player_AttackDodgeB.h"
#include "Player_SpecialAttack.h"
#include "Player_GuardStart.h"
#include "Player_GuardLoop.h"
#include "Player_GuardEnd.h"
#include "Player_GuardWalk.h"
#include "Player_GuardBreak.h"
#include "Player_Damage.h"
#include "Player_GuardHit.h"
#include "Player_UseItem.h"
#include "Player_DamageBlow.h"
#include "Player_DamageEnd.h"
#include "Player_AttackParry.h"
#include "Player_SpecialSuck.h"
#include "Player_BackStab.h"
#include "Player_AttackStrongCharge.h"
#include "Player_AttackStrongStart.h"
#include "Player_UpperDummy.h"
#include "Player_WeaponChange.h"
#include "Player_CheckPoint.h"
#include "Player_PickItem.h"
#include "Player_OpenBox.h"
#include "Player_Death.h"
#include "Player_Skill.h"
#include "Player_Ladder.h"
#include "Player_Custom.h"

#include "Camera_Object.h"

#pragma region SKILL
#include "Skill_Base.h"
#include "SkillManager.h"
#include "Skill_Enchant.h"
#include "Skill_IndraCoil.h"
#include "Skill_KangRyong.h"
#include "Skill_RadiantBarrel.h"
#include "Skill_RoyalHeart.h"
#include "Skill_ShadowAssault.h"
#include "Skill_StrikeRiser.h"
#include "Skill_Yeomgeom.h"
#pragma endregion


#pragma region EFFECTS_HEADER
#include "ParticleSystem.h"
#include "TrailEffect.h"
#pragma endregion

#include "MinimapRenderComponent.h"
#include "InteractionManager.h"
#include "InventoryManager.h"
#include "PoolingManager.h"
#include "ItemManager.h"


#include "GameClock.h"
#include "Bone.h"
#include "Model.h"
#include "Layer.h"
#include "Camera_Player.h"
#include "Monster.h"
#include "UIObj_QuickSlot.h"

#include "Yakumo.h"

Client::Player::Player()
{
}

Client::Player::Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:ContainerObject(pDevice, pContext)
{
}

Client::Player::Player(const Player& original)
	:ContainerObject(original)
{
}

Client::Player::~Player()
{
}
void Client::Player::Set_Position_ByController(_fvector vWorldPos)
{
	if (m_pController)
		m_pController->setPosition(ToPxExtendedVec3(vWorldPos));

	m_vPhysicsPos = vWorldPos;
	m_vRenderPos = vWorldPos;

	if (m_pTransformCom)
		m_pTransformCom->Set_State(DIRECTION::POSITION, vWorldPos);
}
/******************************************************* 생성자, 소멸자 *******************************************************/



void Client::Player::Reset_PlayerAnimEventDesc()
{
	m_tPlayerRunTimeEvent.iInputAreaCount = 0;
	m_tPlayerRunTimeEvent.iCanMoveCount = 0;
	m_tPlayerRunTimeEvent.iCanComboCount = 0;
	m_tPlayerRunTimeEvent.iCanEscapeCount = 0;
	m_tPlayerRunTimeEvent.bBlockRegenStamina = false;
	m_eAnimPhase = ANIM_FRAMEPHASE::ANIMFRAMEPHASE_END; 
	m_tPlayerRunTimeEvent.iCanSuperArmorCount = 0; 
	m_tPlayerRunTimeEvent.iCanInvincible = 0; 
	m_tPlayerRunTimeEvent.iCanLockOnHomingRotation = 0; 
	m_tPlayerRunTimeEvent.iCanWeaponVisible = 0;
	m_tPlayerRunTimeEvent.iCanBloodWeaponVisible = 0;
	m_tPlayerRunTimeEvent.iCanParry = 0;
}

void Client::Player::Reset_PlayerMoveInputDesc()
{
	m_tInputDesc.InputDir = INPUT_DIR::INPUT_END;
	m_tInputDesc.vMoveDir = {};
}

void Client::Player::Reset_PlayerPreesedInputDesc()
{
	m_tInputDesc.bShiftPressed = { false };
	m_tInputDesc.bAltPressed = { false };
}

void Client::Player::Reset_PlayerConsumeDesc()
{
	m_tInputDesc.bSpaceDown = false;
	m_tInputDesc.bAltDown = false;
	m_tInputDesc.bAltUp = false;
	m_tInputDesc.bCDown = false;
	m_tInputDesc.eLastMouseKeyState = MOUSEKEYSTATE::END;
	m_tInputDesc.bDownKeyDown = false;
	m_tInputDesc.bUpKeyDown = false;
}

void Client::Player::Register_Collider_Player()
{
	m_pGameInstance->Register_Collider(COLLISION_GROUP::PLAYER, m_pColliderCom);
}

//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::Player::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT Client::Player::Initialize(void* arg)
{
	static _uint namenum = 0;

	PLAYER_DESC tPlayerDesc;
	PLAYER_DESC* pArgDesc = nullptr;

	if (arg == nullptr)
	{
		tPlayerDesc.fSpeed = 30.f;
		tPlayerDesc.fRotationSpeed = XMConvertToRadians(60.f);
		pArgDesc = &tPlayerDesc;
	}
	else
	{
		pArgDesc = CAST(PLAYER_DESC*)(arg);

	}

	PHYSX_CONTROLLER_DESC controllerDesc;
	controllerDesc.vPosition.x = pArgDesc->vPosition.x;
	controllerDesc.vPosition.y = pArgDesc->vPosition.y;
	controllerDesc.vPosition.z = pArgDesc->vPosition.z;


	_float fAngleDegree = 60.f;
	controllerDesc.fRadius = 0.3f;
	controllerDesc.fHeight = 0.67f;
	controllerDesc.fSlopeLimit = fAngleDegree;
	controllerDesc.fStepOffset = 0.2f;
	controllerDesc.fContactOffset = 0.02f;
	controllerDesc.eActorType = PX_ACTOR_TYPE::PLAYER;
	controllerDesc.iObjectID = m_iObjectID;
	
	m_pController = Create_Controller(controllerDesc);

	pArgDesc->wstrName = L"Player_" + to_wstring(namenum++);

	CHECK_FAILED(GameObject::Initialize(pArgDesc), E_FAIL);

	/*				필수				 */
	m_pGameInstance->Add_Player(this);

	CHECK_FAILED(Ready_Components(), E_FAIL);
	CHECK_FAILED(Ready_PartObjects(), E_FAIL);
	CHECK_FAILED(Ready_States(), E_FAIL);
	CHECK_FAILED(Ready_Skills(), E_FAIL);
	CHECK_FAILED(Ready_Event(), E_FAIL);
	CHECK_FAILED(Ready_PhysXEvent(), E_FAIL);


	Reset_PlayerMoveInputDesc();
	Reset_PlayerPreesedInputDesc();
	Reset_PlayerConsumeDesc();

	m_vPhysicsPos = m_pTransformCom->Get_State(DIRECTION::POSITION);
	m_vRenderPos = m_vPhysicsPos;
	Update_RenderMatrix(); //초기화 시점에 쓰레기값 안들어가게 행렬 갱신

	m_bEnablePhysics = true;
	m_pController->setFootPosition(ToPxExtendedVec3(Get_Position()));

	m_pInteractionManager = InteractionManager::GetInstance();

	m_umapWeaponTable = {
		{1000,WEAPON_TYPE::BAYONET},
		{1001,WEAPON_TYPE::HAMMER},
		{1002,WEAPON_TYPE::SWORD},
		{1003,WEAPON_TYPE::HALBERD},
		{1004,WEAPON_TYPE::GREATESWORD},
		{1005,WEAPON_TYPE::SWORD},
		{1006,WEAPON_TYPE::GREATESWORD},
	};

	/* 패링, 뒤잡 시네마틱 연출 대기시간 */
	m_tCinematicAlarm.Limit = 0.7f;


	m_PlayerDeadAlarm.m_AlarmFunc = [this] {
		Layer* layer = m_pGameInstance->Get_Layer(L"Layer_Monster");
		vector<GameObject*> layervector = layer->Get_GameObjects_Vector();
		for (auto& object : layervector)
		{
			POOLING->Despawn_Object(object);
		}
		m_PlayerDeadAlarm.Off();
		};

	m_PlayerDeadAlarm.Limit = 5.f;
	m_PlayerDeadAlarm.Off();

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Player::Update_Priority(const _float fTimeDelta)
{
	if (!m_bIsActive)
		return -1;


	__super::Update_Priority(fTimeDelta);

	if (m_pGameInstance->KeyDown(DIK_F11))
	{
		if (m_pGameInstance->KeyPress(DIK_LCONTROL))
			m_bCanInput = !m_bCanInput;
	}

	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Player::Update_Parallel(const _float fTimeDelta)
{
	if (!m_bIsActive)
		return -1;


	__super::Update_Parallel(fTimeDelta);

	m_PlayerDeadAlarm.Update(fTimeDelta);

	if(m_pMasterRigModel)
	{
		_uint animIndex = m_pMasterRigModel->Get_CurrentAnimationIndex();
		if (animIndex == 244 || animIndex == 242)
		{
			m_fLadderAdditional = -0.0161f;
		}
		else if (animIndex == 249 || animIndex == 251)
		{
			m_fLadderAdditional = 0.0161f;
		}
		else
		{
			m_fLadderAdditional = 0.f;
		}
	}

	if (m_pGameInstance->KeyDown(DIK_CAPSLOCK))
	{
		m_bDAMAGEMODE = !m_bDAMAGEMODE;
		if (m_bDAMAGEMODE)
		{
			m_fAdditionalDamage = 800.f;
		}
		else
		{
			m_fAdditionalDamage = 0.f;
		}
	}
	
	// 시네마틱 연출이 발동됐으면
	if(m_bPlayCineamtic)
	{
		// 연출 나올때까지 기다렸다가
		m_tCinematicAlarm.Update(fTimeDelta);
		// 시간 됐으면 연출 시작
		if (m_tCinematicAlarm.Elapsed >= m_tCinematicAlarm.Limit)
		{
			// 몬스터 위치 플레이어 앞으로 옮기기
			_vector playerPos = Get_Position();
			_vector playerLook = Get_Look();
			_vector monsterPos = playerPos + (playerLook * 1.5f);
			m_pTargetMonster->Teleport(monsterPos);
			m_pTargetMonster->LookAt(playerPos);

			// 플레이어 상태 변환
			m_pStateMachineCom->Change_State(SPECIALSUCK);
			// 알람 오프
			m_tCinematicAlarm.Off();
		}
	}

#ifdef _DEBUG
	//플레이어 중력 부여하는 키. 꼭 맵 다 로딩하고 누르기!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 안그러면 저아래로 꺼짐 
	//if (m_pGameInstance->KeyDown(DIK_UP))
	//{
	//	m_bEnablePhysics = !m_bEnablePhysics;

	//	if (!m_bEnablePhysics)
	//		m_fGravity = 0.f;

	//	if (m_pGameInstance->KeyPress(DIK_LALT))
	//	{
	//		m_bEnablePhysics = false;
	//		m_fGravity = -0.5f;
	//	}
	//}
#endif // _DEBUG

	return 0;
}

_int Client::Player::Update(const _float fTimeDelta)
{
	if (!m_bIsActive)
		return -1;
	if (m_pGameInstance->KeyDown(DIK_LCONTROL))
	{
		m_bDebugControl = !m_bDebugControl;
		//ItemInfo* pKangRyongSkillItemInfo = ItemManager::GetInstance()->Get_ItemInfo(L"상실의 조각(대)");
		//InventoryManager::GetInstance()->Add_Item(pKangRyongSkillItemInfo, false);

		m_pPlayerStatCom->FullRestore_Stamina();
		m_pPlayerStatCom->Heal_Hp(m_pPlayerStatCom->Get_MaxHp());
	}

#ifdef _DEBUG
	if (m_pGameInstance->KeyDown(DIK_DELETE))
	{
		m_pStateMachineCom->Change_State(CHECKPOINT);
	}
#endif // _DEBUG

	if (m_bDebugControl)
	{
		m_pPlayerStatCom->FullRestore_Stamina();
		m_pPlayerStatCom->Heal_Hp(m_pPlayerStatCom->Get_MaxHp());
		m_pPlayerStatCom->Apply_Myeonghyeol(m_pPlayerStatCom->Get_MaxMyeonghyeol());
		//SkillManager::GetInstance()->Debug_SkillReset();
	}

	if (m_bDissolving)
	{
		m_fTimeElapsed += fTimeDelta * m_fDissolveDir;
		m_fTimeElapsed = clamp(m_fTimeElapsed, 0.f, m_fDissolveMax);

		// 완료 체크
		if ((m_fDissolveDir > 0.f && m_fTimeElapsed >= m_fDissolveMax) ||
			(m_fDissolveDir < 0.f && m_fTimeElapsed <= 0.f))
			m_bDissolving = false;
	}

	if(m_bCanInput == true)
	{
		if(!g_bFreeCam)
			Gather_Input(fTimeDelta);
	}
	
	if (m_pGameInstance->KeyDown(DIK_I))
	{
		//여기서 디버그용도로 동행자 소환 및 위치 세팅해주기
		GameObject* pGameObject = m_pGameInstance->Get_GameObject(_UINT(LEVEL::PLAYER),L"Layer_Yakumo",L"Yakumo_0");
		Yakumo* PYakumo = dynamic_cast<Yakumo*>(pGameObject);
		PYakumo->Set_Active(true);
		PYakumo->Set_Position_ByController(m_vRenderPos);

	}

	m_pStateMachineCom->Update_State(fTimeDelta);	// 상태머신 업데이트
	m_pUpperStateMachineCom->Update_State(fTimeDelta); //상체 상태머신 업데이트
	SkillManager::GetInstance()->Update_CooldownAndDuration(fTimeDelta); //스킬들 전체 쿨타임,지속시간 관리 

	Update_Weapon();
	Update_LockOn(fTimeDelta); //락온 업데이트

	if (!g_bFreeCam)
	{
		Apply_MoveAndRotation(fTimeDelta);
		Update_RotationLerp(fTimeDelta);
	}


	//_uint iCurState =  m_pStateMachineCom->Get_CurStateType();
	//_uint iUpperState = m_pUpperStateMachineCom->Get_CurStateType();

	//COUT("애님스피드: " << m_pMasterModel->Get_SingleAnimation_Speed());

	//COUT("위치 x: "<< XMVectorGetX(m_vRenderPos));
	//COUT("위치 y: " << XMVectorGetY(m_vRenderPos));
	//COUT("위치 z: " << XMVectorGetZ(m_vRenderPos));

	//COUT("전신 상태 : " << iCurState);
	//COUT("상체 상태 :" << iUpperState);

	if (m_pColliderCom)
		m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	Update_RenderMatrix(); //렌더용 행렬 계산
	__super::Update(fTimeDelta);

	// 무기 본 월드 위치 갱신
	Update_WeaponPosition();

	Process_AttackSweep_Capsule();

	if (m_bActiveYeongeom)
		Update_Yeongeomjeonmu(fTimeDelta); //염검전무 판정 활성화

	// Trail 재생
	if (m_bTrailActive &&
		m_vecTrailEffects.empty() == false)
	{
		_float3 vFinalRoot = {}, vFinalTip = {};

		vFinalRoot.x = m_pWeaponBoneMatrixEnd->_41;
		vFinalRoot.y = m_pWeaponBoneMatrixEnd->_42;
		vFinalRoot.z = m_pWeaponBoneMatrixEnd->_43;

		vFinalTip.x = m_pWeaponBoneMatrix->_41;
		vFinalTip.y = m_pWeaponBoneMatrix->_42;
		vFinalTip.z = m_pWeaponBoneMatrix->_43;

		for (auto& pTrail : m_vecTrailEffects)
			pTrail->Add_TrailPoint(vFinalRoot, vFinalTip);
	}

#ifdef _DEBUG
	//// T키로 Sweep 수동 테스트
	//if (m_pGameInstance->KeyDown(DIK_T))
	//{
	//	m_bAttackSweepActive = !m_bAttackSweepActive;
	//	if (m_bAttackSweepActive)
	//	{
	//		m_setHitTargets.clear();
	//		m_vPrevWeaponPos = m_vCurrentWeaponPos;
	//		COUT("[Debug] Sweep ON");
	//	}
	//	else
	//	{
	//		m_setHitTargets.clear();
	//		COUT("[Debug] Sweep OFF");
	//	}
	//}
	Add_Debug_Controller_Capsule(fTimeDelta);
	if(m_bAttackSweepActive)
	{
		Add_Debug_WeaponCapsule();
	}
	Add_Debug_LookLine();
#endif
	Update_PlayerCustomDesc();
	m_pGameInstance->Follow_ShadowCascade_Matrix(m_pTransformCom->Get_State(DIRECTION::POSITION)); //
	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int Client::Player::Update_Late(const _float fTimeDelta)
{
	if (!m_bIsActive)
		return -1;


	__super::Update_Late(fTimeDelta);

	Update_Stamina(fTimeDelta);
	Update_FocusGauge(fTimeDelta);
	//m_bFocusState = true;
	m_pPlayerStatCom->Update_UIEvent();

	//MinimapComponent
	m_pMinimapRendercom->Update_Late(fTimeDelta);

	//Interaction키 입력여부
	m_pInteractionManager->OnPressedInteractionKey();

	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT Client::Player::Render(const _float fTimeDelta)
{
	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/


#ifdef _DEBUG
void Client::Player::Add_Debug_WeaponCapsule(_float _radius, _float3 _color)
{
	GameObject::Add_Debug_Capsule(*m_pWeaponBoneMatrix, *m_pWeaponBoneMatrixEnd, m_fAttackRadius, _color);
}

#endif // _DEBUG


void Client::Player::Gather_Input(const _float fTimeDelta)
{
	Reset_PlayerMoveInputDesc();
	Reset_PlayerPreesedInputDesc();

	if (m_tInputDesc.bZeroStamina || m_bOpenUI)
	{
		Reset_PlayerConsumeDesc();

		if (m_bOpenUI)
			return;
	}

	Camera* pPlayerCamera = m_pGameInstance->Get_Camera(static_cast<_uint>(CAMERA::PLAYER_FOCUS_01));
	if (pPlayerCamera == nullptr)
		return;

	Transform* pCameraTransformCom = pPlayerCamera->Get_Transform();

	_vector vCameraLook = pCameraTransformCom->Get_State(DIRECTION::LOOK);
	_vector vCameraRight = pCameraTransformCom->Get_State(DIRECTION::RIGHT);

	//y성분은 안쓸거니까 날려주고 , 평면 x,z 이동
	vCameraLook = XMVectorSet(XMVectorGetX(vCameraLook), 0.f, XMVectorGetZ(vCameraLook), 0.f);
	vCameraRight = XMVectorSet(XMVectorGetX(vCameraRight), 0.f, XMVectorGetZ(vCameraRight), 0.f);

	//단위벡터로 방향만 남겨주고
	vCameraLook = XMVector3Normalize(vCameraLook);
	vCameraRight = XMVector3Normalize(vCameraRight);


	State* pCurState = m_pStateMachineCom->Get_CurrentState();
	State* pUpperCurState = m_pUpperStateMachineCom->Get_CurrentState();

	if (pCurState->Get_HasAnimEvents()==true || pUpperCurState->Get_HasAnimEvents()==true) //애니메이션 이벤트가 있는 상태 객체 (이거 InputEvent라고 생각해야할듯)
	{
		if (m_tPlayerRunTimeEvent.bInputArea() == true) //입력 가능한 이벤트가 활성화 된다면
		{
			if (m_pGameInstance->MouseDown(MOUSEKEYSTATE::LB))
				m_tInputDesc.eLastMouseKeyState = MOUSEKEYSTATE::LB;
			if (m_pGameInstance->MouseDown(MOUSEKEYSTATE::RB))
				m_tInputDesc.eLastMouseKeyState = MOUSEKEYSTATE::RB;
			if (m_pGameInstance->KeyPress(DIK_LSHIFT))
				m_tInputDesc.bShiftPressed = true;
			if (m_pGameInstance->KeyDown(DIK_SPACE))
				m_tInputDesc.bSpaceDown = true;
			if (m_pGameInstance->KeyDown(DIK_C))
				m_tInputDesc.bCDown = true;
			if (m_pGameInstance->KeyDown(DIK_UP))
				m_tInputDesc.bUpKeyDown = true;
			if (m_pGameInstance->KeyDown(DIK_DOWN))
				m_tInputDesc.bDownKeyDown = true;


			if (m_pGameInstance->KeyPress(DIK_LALT))
			{
				m_tInputDesc.bAltPressed = true;
			}
			else
				m_bOnGuard = false;

			if (m_pGameInstance->KeyDown(DIK_LALT))
			{
				m_tInputDesc.bAltDown = true;
				m_bOnGuard = true;
				m_vSaveCameraLook = vCameraLook;
			}

			if (m_pGameInstance->KeyUp(DIK_LALT))
				m_tInputDesc.bAltUp = true;

			m_tInputDesc.iSkillSlotKeyDown = -1;

			if (m_pGameInstance->KeyDown(DIK_1)) //스킬 키입력은 동시입력안되므로 else if로
				m_tInputDesc.iSkillSlotKeyDown = 0;
			else if (m_pGameInstance->KeyDown(DIK_2))
				m_tInputDesc.iSkillSlotKeyDown = 1;
			else if (m_pGameInstance->KeyDown(DIK_3))
				m_tInputDesc.iSkillSlotKeyDown = 2;
			else if (m_pGameInstance->KeyDown(DIK_4))
				m_tInputDesc.iSkillSlotKeyDown = 3;
			else if (m_pGameInstance->KeyDown(DIK_5))
				m_tInputDesc.iSkillSlotKeyDown = 4;
			else if (m_pGameInstance->KeyDown(DIK_6))
				m_tInputDesc.iSkillSlotKeyDown = 5;
			else if (m_pGameInstance->KeyDown(DIK_7))
				m_tInputDesc.iSkillSlotKeyDown = 6;
			else if (m_pGameInstance->KeyDown(DIK_8))
				m_tInputDesc.iSkillSlotKeyDown = 7;


		}
		else //입력 가능한 이벤트가 활성화 되어 있지 않다면 리셋
		{
			Reset_PlayerConsumeDesc();
		}
	}
	else //애니메이션 이벤트가 없는 상태 객체
	{
		Set_ApplyTranslation(true);

		if (m_pGameInstance->MouseDown(MOUSEKEYSTATE::LB))
			m_tInputDesc.eLastMouseKeyState = MOUSEKEYSTATE::LB;
		if (m_pGameInstance->MouseDown(MOUSEKEYSTATE::RB))
			m_tInputDesc.eLastMouseKeyState = MOUSEKEYSTATE::RB;
		if (m_pGameInstance->KeyPress(DIK_LSHIFT))
			m_tInputDesc.bShiftPressed = true;
		if (m_pGameInstance->KeyDown(DIK_SPACE))
			m_tInputDesc.bSpaceDown = true;
		if (m_pGameInstance->KeyDown(DIK_C))
			m_tInputDesc.bCDown = true;
		if (m_pGameInstance->KeyDown(DIK_UP))
			m_tInputDesc.bUpKeyDown = true;
		if (m_pGameInstance->KeyDown(DIK_DOWN))
			m_tInputDesc.bDownKeyDown = true;


		if (m_pGameInstance->KeyDown(DIK_LALT))
		{
			m_bOnGuard = true;
			m_vSaveCameraLook = vCameraLook;
			m_tInputDesc.bAltDown = true;
		}
		if (m_pGameInstance->KeyPress(DIK_LALT))
		{
			m_tInputDesc.bAltPressed = true;
		}
		else
			m_bOnGuard = false;
		if (m_pGameInstance->KeyUp(DIK_LALT))
			m_tInputDesc.bAltUp = true;

		m_tInputDesc.iSkillSlotKeyDown = -1;

		if (m_pGameInstance->KeyDown(DIK_1)) //스킬 키입력은 동시입력안되므로 else if로
			m_tInputDesc.iSkillSlotKeyDown = 0;
		else if (m_pGameInstance->KeyDown(DIK_2))
			m_tInputDesc.iSkillSlotKeyDown = 1;
		else if (m_pGameInstance->KeyDown(DIK_3))
			m_tInputDesc.iSkillSlotKeyDown = 2;
		else if (m_pGameInstance->KeyDown(DIK_4))
			m_tInputDesc.iSkillSlotKeyDown = 3;
		else if (m_pGameInstance->KeyDown(DIK_5))
			m_tInputDesc.iSkillSlotKeyDown = 4;
		else if (m_pGameInstance->KeyDown(DIK_6))
			m_tInputDesc.iSkillSlotKeyDown = 5;
		else if (m_pGameInstance->KeyDown(DIK_7))
			m_tInputDesc.iSkillSlotKeyDown = 6;
		else if (m_pGameInstance->KeyDown(DIK_8))
			m_tInputDesc.iSkillSlotKeyDown = 7;
	}

	_bool bW = m_pGameInstance->KeyPress(DIK_W);
	_bool bA = m_pGameInstance->KeyPress(DIK_A);
	_bool bS = m_pGameInstance->KeyPress(DIK_S);
	_bool bD = m_pGameInstance->KeyPress(DIK_D);

	if (m_tInputDesc.bLockOn && m_pLockOnTarget != nullptr)
	{
		if (m_tInputDesc.bShiftPressed)
		{
			m_fLockOnShiftPressTime += fTimeDelta;
			m_bLockOnShift = (m_fLockOnShiftPressTime > 1.2f); 
		}
		else
		{
			m_fLockOnShiftPressTime = 0.f;
			m_bLockOnShift = false;
		}

		if (m_bLockOnShift) 
		{
			if (bW) m_tInputDesc.vMoveDir += vCameraLook;
			if (bA) m_tInputDesc.vMoveDir -= vCameraRight;
			if (bS) m_tInputDesc.vMoveDir -= vCameraLook;
			if (bD) m_tInputDesc.vMoveDir += vCameraRight;
		}
		else
		{
			_vector vMyPos = m_pTransformCom->Get_State(DIRECTION::POSITION);
			_vector vTargetPos = m_pLockOnTarget->Get_Position();

			_vector vDir = vTargetPos - vMyPos;

			vDir = XMVectorSet(XMVectorGetX(vDir), 0.f, XMVectorGetZ(vDir), 0.f); //y값 날려버리고

			vDir = XMVector3Normalize(vDir);

			_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.0f, 0.f, 0.f), vDir));//외적해서 right 뽑아주고

			_float fFoward = 0.f;
			if (bW) fFoward += 1.f;
			if (bS) fFoward -= 1.f;

			_float fRight = 0.f;
			if (bD) fRight += 1.f;
			if (bA) fRight -= 1.f;

			m_tInputDesc.vMoveDir = vDir * fFoward + vRight * fRight; //락온 타겟 기준 방향 벡터 구해주고

			if (XMVectorGetX(XMVector3LengthSq(m_tInputDesc.vMoveDir)) > 1e-6f)
				m_tInputDesc.vMoveDir = XMVector3Normalize(m_tInputDesc.vMoveDir);
		}
	}
	else
	{
		if (bW)m_tInputDesc.vMoveDir += vCameraLook;
		if (bA)m_tInputDesc.vMoveDir -= vCameraRight;
		if (bS)m_tInputDesc.vMoveDir -= vCameraLook;
		if (bD)m_tInputDesc.vMoveDir += vCameraRight;
	}

	if (bW && bA) m_tInputDesc.InputDir = INPUT_DIR::LEFT_UP;
	else if (bW && bD) m_tInputDesc.InputDir = INPUT_DIR::RIGHT_UP;
	else if (bS && bA) m_tInputDesc.InputDir = INPUT_DIR::LEFT_DOWN;
	else if (bS && bD) m_tInputDesc.InputDir = INPUT_DIR::RIGHT_DOWN;
	else if (bW)m_tInputDesc.InputDir = INPUT_DIR::UP;
	else if (bA)m_tInputDesc.InputDir = INPUT_DIR::LEFT;
	else if (bS)m_tInputDesc.InputDir = INPUT_DIR::DOWN;
	else if (bD)m_tInputDesc.InputDir = INPUT_DIR::RIGHT;

}

void Client::Player::Apply_MoveAndRotation(_float fTimeDelta)
{
	//사다리 상태일때
	_uint iCurrentState = m_pStateMachineCom->Get_CurStateType();

	if (iCurrentState == LADDER)
	{
		// 사다리 상태에서는 루트모션만 적용
		if (m_pController)
		{
			_vector additY = XMVectorSet(0.f, m_fLadderAdditional * fTimeDelta, 0.f, 0.f);
			_vector vRootWorldDelta = m_pMasterRig->Get_RootWorldDelta() + additY;
			PxVec3 RootWorldDelta = ToPxVec3(vRootWorldDelta);

			physx::PxControllerFilters filters;
			m_pController->move(RootWorldDelta, 0.001f, fTimeDelta, filters);

			PxExtendedVec3 pxPos = m_pController->getPosition();
			_float fYOffset = (1.0f * 0.5f) + 0.15f;
			m_vPhysicsPos = XMVectorSet((_float)pxPos.x, (_float)pxPos.y - fYOffset, (_float)pxPos.z, 1.f);

			m_vRenderPos = m_vPhysicsPos; // 사다리에서는 보간 없이 즉시 반영
			m_pTransformCom->Set_State(DIRECTION::POSITION, m_vRenderPos);
		}
		return;
	}

	// 물리 이동 로직
	if (m_pController)
	{
		if (!m_bApplyTranslation)
			m_tInputDesc.vMoveDir = XMVectorZero();

		physx::PxVec3 vMoveDir = ToPxVec3(m_tInputDesc.vMoveDir);

		m_fSpeed = 5.f;

		if (iCurrentState == DASH) //Dash 상태일때로 해야겠는데 
			m_fSpeed = 7.f;

		if (iCurrentState == WALK)
			m_fSpeed = 2.f;

		if (m_tInputDesc.bAltDown)			
			m_fSpeed = 0.f;
		if (m_tInputDesc.bAltPressed)
			m_fSpeed = 1.f;
		
		// y축이동막음. 비행 구현할거라면..?
		vMoveDir.y = 0.f;
		if (vMoveDir.magnitudeSquared() > 0.0001f)
			vMoveDir.normalize(); //대각선 이동 속도 정규화

		if (m_tInputDesc.bLockOn && m_pLockOnTarget)
		{
			_vector vMyPos = m_pTransformCom->Get_State(DIRECTION::POSITION);
			_vector vTargetPos = m_pLockOnTarget->Get_Position();

			_vector vDir = vTargetPos - vMyPos;

			_float fDist = XMVectorGetX(XMVector3Length(vDir));

			_float fNearDist = 0.5f;// 최소거리
			_float fFarDist = 3.5f; //최대거리
			_float fWeightNear = 1.f - clamp((fDist - fNearDist) / (fFarDist - fNearDist), 0.f, 1.f);

			//락온 대상과 플레이어의 거리가 가까울때 AD(좌우) 이동시 빠른 속도로 움직이는 것처럼 느껴지는 문제를 해결해주기 위해서
			//가까우면 스피드를 조절
			_float fStrafeScale = 1.0f - 0.60f * fWeightNear; //0.6배 만큼 느려지도록 설정
			m_fSpeed *= fStrafeScale;
		}

		if (m_bDebugControl && m_pGameInstance->KeyPress(DIK_LSHIFT))
			m_fSpeed = 40.f;

		physx::PxVec3 vFinalVelocity = vMoveDir * m_fSpeed * fTimeDelta;

		// 중력 처리
		// 바닥에 닿아있을때의 중력 / 공중에 있을때의 중력(가속 붙음)
		if (m_bEnablePhysics)
		{
			if (m_bIsGrounded)
			{
				if (m_fVerticalVelocity < 0.f)
					m_fVerticalVelocity = -1.0f;
			}
			else
				m_fVerticalVelocity -= 25.f * fTimeDelta; // 공중: 중력 가속	
			
		}
		else
		{
			m_fVerticalVelocity = 0.f;
		}
		_vector vRootWorldDelta = m_pMasterRig->Get_RootWorldDelta();
		PxVec3 RootWorldDelta = ToPxVec3(vRootWorldDelta);

		vFinalVelocity.y = (m_fVerticalVelocity) * fTimeDelta;
		vFinalVelocity += RootWorldDelta; //루트모션 적용

		// 부딪힐 물체 필터링(아군이나 무기같은거랑 부딪혀서 끼지않게)
		physx::PxControllerFilters filters;
		filters.mCCTFilterCallback = nullptr;
		filters.mFilterCallback = nullptr;

		physx::PxControllerCollisionFlags flags = m_pController->move(vFinalVelocity //이번 프레임에 이동할 "거리 벡터"
			, 0.001f //"이 이하 이동은 무시" 같은 안정화
			, fTimeDelta
			, filters); //나중에 "자기 무기/아군/트리거는 무시" 같은거 하려면 여기 확장.

		//move 결과 flags로 "아래로 충돌했는지"를 받음, 이 값이 다음 프레임 중력 로직에 들어가서 "접착력/낙하"가 결정됨.
		m_bIsGrounded = (flags & physx::PxControllerCollisionFlag::eCOLLISION_DOWN);
	}

	// 계산결과를 적용한다
	// 엔진상 캡슐(캐릭터)의 위치
	if (m_pController != nullptr)
	{
		PxExtendedVec3 pxPos = m_pController->getPosition();

		_float fYOffset = (1.0f * 0.5f) + 0.15f;

		m_vPhysicsPos = XMVectorSet((_float)pxPos.x, (_float)pxPos.y - fYOffset, (_float)pxPos.z, 1.f);

		// Transform 반영을 RenderPos 보간 이후에
		Update_RenderPos(fTimeDelta);
		m_pTransformCom->Set_State(DIRECTION::POSITION, m_vRenderPos);
	}

	//회전 업데이트 하기 전에

	if (!m_bOnGuard)
	{
		if (m_bLockOnShift)
		{
			Start_Dir_RotationLerp();
		}
		else if (m_tInputDesc.bLockOn && m_pLockOnTarget != nullptr)
		{
			_vector vDir = m_pLockOnTarget->Get_Position() - m_pTransformCom->Get_State(DIRECTION::POSITION);

			_float fDist = XMVectorGetX(XMVector3Length(vDir));
			if (fDist < 1e-6f) return;

			vDir = XMVectorSet(XMVectorGetX(vDir), 0.f, XMVectorGetZ(vDir), 0.f);
			vDir = XMVector3Normalize(vDir);


			_float fNearDist = 1.0f;
			_float fFarDist = 4.0f;
			_float fTime = clamp((fDist - fNearDist) / (fFarDist - fNearDist), 0.f, 1.0f);
			_float fBaseSpeed = fLerp(600.f, 840.f, fTime);

			if (m_tPlayerRunTimeEvent.bCanLockOnHomingRotation()) //락온 이벤트 발생시 기존 회전속도 * 이벤트  값(회전 늦추는 용도같음)
			{
				_float fRatio = m_tPlayerRunTimeEvent.fHomingRotationSpeedRatio; //대충 0.1~0.5정도의 값 들어옴
				fBaseSpeed *= fRatio;
			}

			m_fLockOnRotationSpeed = fBaseSpeed;

			m_pTransformCom->LookDir_Smooth(vDir, m_fLockOnRotationSpeed, fTimeDelta);

		}
		else
		{
			if(!Is_AttackingState())
				Start_Dir_RotationLerp();
			
		}
	}	


	if (XMVectorGetX(XMVector3LengthSq(m_tInputDesc.vMoveDir)) > 0.0001f)//여기서 입력이 0이 아닐떄만 회전을 하므로 
	{
		if (m_bOnGuard) //가드 시작할때 카메라 마지막 위치를 기준으로 방향고정
		{
			m_pTransformCom->LookDir(m_vSaveCameraLook);
		}
	}
}

void Client::Player::Start_Dir_RotationLerp()
{
	//여기서 이제 내가 바라보는 방향으로 회전시작하라고 m_pTransformCom에 명령
	m_pTransformCom->Statrt_Rotation_Lerp(m_tInputDesc.vMoveDir, 0.1f);
}

void Client::Player::Update_RotationLerp(_float fTimeDelta)
{
	m_pTransformCom->Update_Rotation_Lerp(fTimeDelta);
}

void Client::Player::Apply_AnimEventCount(const PLAYERANIM_EVENT& PlayerAnimEvent, _int iSign)
{
	auto AddClamp = [](_int& iValue, _int iDelta)
		{
			iValue += iDelta;
			if (iValue < 0) iValue = 0;
		};

	if (PlayerAnimEvent.bInputArea) AddClamp(m_tPlayerRunTimeEvent.iInputAreaCount, iSign);
	if (PlayerAnimEvent.bCanCombo) AddClamp(m_tPlayerRunTimeEvent.iCanComboCount, iSign);
	if (PlayerAnimEvent.bCanEscape) AddClamp(m_tPlayerRunTimeEvent.iCanEscapeCount, iSign);
	if (PlayerAnimEvent.bCanMove) AddClamp(m_tPlayerRunTimeEvent.iCanMoveCount, iSign);
	if (PlayerAnimEvent.bInvincible) AddClamp(m_tPlayerRunTimeEvent.iCanInvincible, iSign);
	if (PlayerAnimEvent.bSuperArmor) AddClamp(m_tPlayerRunTimeEvent.iCanSuperArmorCount, iSign);
	if (PlayerAnimEvent.bLockOnHomingRotation) AddClamp(m_tPlayerRunTimeEvent.iCanLockOnHomingRotation, iSign);
	if (PlayerAnimEvent.bBloodWeaponVisible) AddClamp(m_tPlayerRunTimeEvent.iCanBloodWeaponVisible, iSign);
	if (PlayerAnimEvent.bWeaponVisible) AddClamp(m_tPlayerRunTimeEvent.iCanWeaponVisible, iSign);
	if (PlayerAnimEvent.bCanParry) AddClamp(m_tPlayerRunTimeEvent.iCanParry, iSign);
	if (PlayerAnimEvent.bInjectionVisible) AddClamp(m_tPlayerRunTimeEvent.iCanInjectionVisible, iSign);
}

void Client::Player::Update_Stamina(_float fTimeDelta)
{
	m_fRegenCoolTime -= fTimeDelta;

	_float fCurrentStamina = m_pPlayerStatCom->Get_Stamina();
	_uint iCurrentStateType = m_pStateMachineCom->Get_CurStateType();
	
	if (fCurrentStamina <= 0)
	{
		if (!m_tInputDesc.bZeroStamina)
		{
			m_tInputDesc.bZeroStamina = true;
			m_fRegenCoolTime = 1.0f;
		}
	}
	else
	{
		m_tInputDesc.bZeroStamina = false;
	}
	
	if (iCurrentStateType == RUN) //게임 원작 느낌 살려서 Run일때는 스태미나가 더 빨리참.
		m_fRegenPerSecond = 50.f;
	else if (iCurrentStateType == GUARDWALK)
		m_fRegenPerSecond = 10.f;
	else
		m_fRegenPerSecond = 40.f;

	if (iCurrentStateType == ATTACKSTRONG || iCurrentStateType == ATTACKSTRONGCHARGE) //어택스트롱일때는 스태미나 재생x
		m_tPlayerRunTimeEvent.bBlockRegenStamina = true;

	if (iCurrentStateType == DASH)
	{
		m_pPlayerStatCom->ContinuousDecrease_Stamina(m_fDashDecrasePerSecond, fTimeDelta);
		m_fRegenCoolTime = 1.0f;
		return;
	}
		
	if (iCurrentStateType == GUARDHIT) //가드히트상태일때도 스태미나 재생x
		return;

	if (m_tPlayerRunTimeEvent.bBlockRegenStamina) //스태미나 재생 못할때 프레임구간 
	{
		//근데 블락일때 해야하는 시간에 애니메이션이 중간에 바뀌어서 false가 안된다면을 기준으로 강제로 스태미나 재생

		if (iCurrentStateType == GUARDWALK)
		{
			m_pPlayerStatCom->Regen_Stamina(m_fRegenPerSecond, fTimeDelta);
			return;
		}

		if (iCurrentStateType == IDLE ||iCurrentStateType == RUN || iCurrentStateType == WALK || iCurrentStateType == DASH) 
		{

			if (m_fRegenCoolTime <= 0.f)
			{
				m_pPlayerStatCom->Regen_Stamina(m_fRegenPerSecond, fTimeDelta);
				return;
			}
		}
		else
		{
			m_fRegenCoolTime = 0.3f; //스태미나 소모하는 애니메이션들이 IDLE WALK RUN과 같이 소모가 없는 애니메이션들의 경우에는 상태가 바뀌었을때 바로 리젠을 시작하지않고
			//쿨타임을 줘서 원본게임과 쿨타임 적용
		}
		return;
	}
	else//스태미나 재생 가능할때 프레임 구간  
	{
		//if (iCurrentStateType == DASH) //대쉬일때는 스태미나 재생 x
		//	return;

		if (m_eAnimPhase == ANIM_FRAMEPHASE::END)
		{
			m_pPlayerStatCom->Regen_Stamina(m_fRegenPerSecond, fTimeDelta);
			return;
		}

		if (iCurrentStateType == GUARDWALK)
		{
			m_pPlayerStatCom->Regen_Stamina(m_fRegenPerSecond, fTimeDelta);
			return;
		}

		if (iCurrentStateType == IDLE || iCurrentStateType == RUN || iCurrentStateType == WALK || iCurrentStateType == DASH)
		{
			if (m_fRegenCoolTime <= 0.f)
			{
				m_pPlayerStatCom->Regen_Stamina(m_fRegenPerSecond, fTimeDelta);
				return;
			}
		}
		else
		{
			m_fRegenCoolTime = 0.3f; //스태미나 소모하는 애니메이션들이 IDLE WALK RUN과 같이 소모가 없는 애니메이션들의 경우에는 상태가 바뀌었을때 바로 리젠을 시작하지않고
			//쿨타임을 줘서 원본게임과 쿨타임 적용
		}
	}
}

void Client::Player::Update_FocusGauge(_float fTimeDelta)
{
	if (m_pPlayerStatCom->Get_IsMaxFocusGauge() && !m_bFocusState) //집중상태가 트루라면(이건 피격당했을때만 갱신되니까)
	{
		m_pPlayerStatCom->Set_IsMaxFocusGauge(false); //확실하게 꺼주고
		m_pPlayerStatCom->FullRestore_Stamina(); //스태미나 완전 회복시켜주고
		m_bFocusState = true;
		m_pPlayerStatCom->Set_FocusState(true);

		// 집중 상태 이펙트
		ParticleSystem* pEffect = CAST(ParticleSystem*)(POOLING->Acquire_Effect(POOL_ID::EFFECT_FOCUS));
		if (pEffect != nullptr)
		{
			_float3 vEffectPos = m_pTransformCom->Get_Position_Float3();
			vEffectPos.y += 1.f;
			pEffect->Set_WorldPosition(vEffectPos);
			pEffect->OnSpawn(nullptr);

			// 플레이어를 따라가는 콜백 등록
			Transform* pTransform = m_pTransformCom;
			pEffect->Set_FollowCallback([pTransform]() -> _float4x4 {
				_float4x4 matWorld = pTransform->Get_WorldFloat4x4();
				matWorld._42 += 1.f;
				return matWorld;
				});
		}
	}
	
	if (m_bFocusState) //집중상태 이므로 여기서 계속 감소 
	{
		if (m_pPlayerStatCom->Get_FocusGauge() <= 0.f) //게이지가 0보다 같거나 작아지면 집중상태 off		
		{
			m_bFocusState = false;
			m_pPlayerStatCom->Set_FocusState(false);
		}
		if (m_bIsDealingDamage) //데미지를 발행했으면 바로 감소
		{
			m_pPlayerStatCom->Reduce_FocusGauge(15.f);
			m_bIsDealingDamage = false; 
		}
		else
		m_pPlayerStatCom->ContinuousDecrease_FocusGauge(m_fFoucsGaugeDecreasePerSecond, fTimeDelta); //지속감소

	}

}

void Client::Player::Decrease_Stamina(const PLAYERANIM_EVENT& PlayerAnimEvent)
{
	_uint iCurrentStateType = m_pStateMachineCom->Get_CurStateType();
	if (iCurrentStateType == DASH)
		return;
	
	m_pPlayerStatCom->Decrease_Stamina(PlayerAnimEvent.fStaminaCost);
}

void Client::Player::Update_UseItem(const PLAYERANIM_EVENT& PlayerAnimEvent)
{
	//우선은 간단하게 사용했는지 여부로 판단 
	//추가로 아이템 타입,개수,등록되어있는지 등등 판단해서 
	//아이템 클래스 ? 장비 ? 인벤토리 등등 필요하겠네
	if (PlayerAnimEvent.bUseItem)
	{
		//현재 포커스 되어있는 아이템정보를 가져온다.
		ItemInfo* pItemInfo = InventoryManager::GetInstance()->Get_ShorcutFocusItem();
		if (pItemInfo == nullptr || pItemInfo->itemCount <=0)
			return; 

		if (pItemInfo->ItemID == 5001) //재생물약
		{
			m_pPlayerStatCom->Heal_Hp(500.f); //우선 상수값
			InventoryManager::GetInstance()->Use_FocusItem();
		}
		else if (pItemInfo->ItemID == 5000) //명혈 농축약
		{
			m_pPlayerStatCom->Apply_Myeonghyeol(6);
			InventoryManager::GetInstance()->Use_FocusItem();
		}
		else if (pItemInfo->ItemID == 5002) //재생 유도약
		{
			m_pPlayerStatCom->Heal_Hp(400.f); //우선 상수값
			InventoryManager::GetInstance()->Use_FocusItem();
		}
		else if (pItemInfo->ItemID == 5009) //재생력의 확장 인자
		{
			ItemInfo* pItem = ItemManager::GetInstance()->Get_ItemInfo(5009);
			ItemInfo* pRestoreItem = ItemManager::GetInstance()->Get_ItemInfo(5001);

			if (pItem && pRestoreItem)
			{
				ItemManager::GetInstance()->Set_MaxCount(L"재생력", pRestoreItem->maxItemSize + 1);
				InventoryManager::GetInstance()->Reset_HillItem();
				InventoryManager::GetInstance()->Use_FocusItem();

				Safe_Delete(pItem);
				Safe_Delete(pRestoreItem);
			}

		}
		else if (pItemInfo->ItemID == 5010) //상실의 조각(대)600
		{
			InventoryManager::GetInstance()->Add_Haze(600);
			InventoryManager::GetInstance()->Use_FocusItem();
		}
		else if (pItemInfo->ItemID == 5011) //응축된 상실의 조각(소)1000
		{
			InventoryManager::GetInstance()->Add_Haze(1000);
			InventoryManager::GetInstance()->Use_FocusItem();
		}

	}
}

//////////////////////////////////////////////////////// 무기 변경 함수 ////////////////////////////////////////////////////////
void Client::Player::Update_ChangeWeapon(const PLAYERANIM_EVENT& PlayerAnimEvent)
{
	//무기 교체 이벤트가 발생했을때
	//인벤토리 0번슬롯과 1번슬롯을검사를해 
	if (PlayerAnimEvent.bChangeEquipWeapon)
	{

		for (_uint i = 0; i < 2; ++i)
		{
			ItemInfo* pItemInfoWeapon = InventoryManager::GetInstance()->Get_MainSlotItemInfo(_UINT(MAINUISLOT::EQUIP_WEAPON), i);
			if (pItemInfoWeapon == nullptr)
				continue;

			auto iter = m_umapWeaponTable.find(pItemInfoWeapon->ItemID);
			if (iter == m_umapWeaponTable.end())
				continue;

			WEAPON_TYPE eWeaponType = iter->second;

			if (eWeaponType != m_eCurrentWeaponType)
			{
				Switch_Weapon(eWeaponType);
				break;
			}

		}

		/*무기교체했으므로, 사용할수 잇는 스킬에 따라서 색변경!*/
		UIObj_QuickSlot::QuickSlotEvent Event;
		Event.m_eEventType = UIObj_QuickSlot::QuickSlotEventType::WEAPON_CHANGE;
		Event.eWeaponType = m_pActiveWeapon->Get_WeaponType();
		m_pGameInstance->Publish(Event);
	}

}
void Client::Player::Switch_Weapon(WEAPON_TYPE eChangeWeaponType)
{
	m_pActiveWeapon->Set_PartActive(false);
	m_pActiveWeapon = m_pWeapons[_UINT(eChangeWeaponType)]; //바뀌어야할 웨폰 타입으로 바꿔주고

	m_pActiveWeapon->Set_PartActive(true);
	Sync_BoneMatrix();
	m_eCurrentWeaponType = eChangeWeaponType;

}
_bool Client::Player::Is_AttackingState()
{
	_bool bIsAttackState = false;

	_uint iCurStateType = m_pStateMachineCom->Get_CurStateType();
	
	if (iCurStateType == ATTACKNORMAL || iCurStateType == SPECIALATTACK  || iCurStateType == ATTACKDODGEB || iCurStateType == ATTACKDODGEF 
		|| iCurStateType == ATTACKPARRY || iCurStateType == ROLL)

		bIsAttackState = true;

	return bIsAttackState;
}
void Client::Player::Sample_AttackMoveDir_FromInput()
{
	_vector vMoveDir = m_tInputDesc.vMoveDir;

	if (XMVectorGetX(XMVector3LengthSq(vMoveDir)) > 1e-6f)
		m_vCacheAttackMoveDir = XMVector3Normalize(vMoveDir);
	else
		m_vCacheAttackMoveDir = XMVector3Normalize(m_pTransformCom->Get_State(DIRECTION::LOOK));

}
void Client::Player::Start_AttackDir_RotationLerp(_float fMaxRotation, _float fLerpDuration)
{
	_float fMaxDegreeRotation = fMaxRotation; 
	_float fMaxRadian = XMConvertToRadians(fMaxDegreeRotation);

	_vector vCurrentDir = XMVector3Normalize(XMVectorSetY(m_pTransformCom->Get_State(DIRECTION::LOOK), 0.f));
	_vector vTargetDir = XMVector3Normalize(XMVectorSetY(m_vCacheAttackMoveDir, 0.f));

	_float fDot = clamp(XMVectorGetX(XMVector3Dot(vCurrentDir, vTargetDir)), -1.f, 1.f);
	_float fRadian = acosf(fDot);

	_float fRotationRatio = (fRadian > 1e-6f) ? min(fMaxRadian / fRadian, 1.f) : 1.f;

	//현재 방향과 타겟방향사이를 최대 각도와 내적의 각도를 기준으로 비율 만들어서 보간
	_vector vLimitedDir = XMVector3Normalize(XMVectorLerp(vCurrentDir, vTargetDir, fRotationRatio));

	m_pTransformCom->Statrt_Rotation_Lerp(vLimitedDir, fLerpDuration);
}
void Client::Player::Update_Weapon()
{
	//m_pActiveWeapon->Set_PartActive(false); //일반무기 비활성화
	//m_pActiveBloodWeapon->Set_PartActive(true); //블러드웨폰 활성화

	if (m_bPlayCineamtic) //시네마틱 재생일때
	{
		m_pActiveWeapon->Set_PartActive(false); //일반무기 비활성화
		m_pActiveBloodWeapon->Set_PartActive(true); //블러드웨폰 활성화
		return;
	}

	if (m_tPlayerRunTimeEvent.bCanWeaponVisible() || m_pStateMachineCom->Get_CurStateType()==LADDER || m_pStateMachineCom->Get_CurStateType()==CUSTOM) //일반 무기 비활성화 구간
	{
		if (m_pActiveWeapon == nullptr)
			return;

		m_pActiveWeapon->Set_PartActive(false);
	}
	else
	{
		if (m_pActiveWeapon == nullptr)
			return;
		m_pActiveWeapon->Set_PartActive(true);
	}

	if (m_tPlayerRunTimeEvent.bCanBloodWeaponVisible()) //흡혈 무기 활성화 구간
	{
		if (m_pActiveBloodWeapon == nullptr)
			return;
		m_pActiveBloodWeapon->Set_PartActive(true);	
	}
	else
	{
		if (m_pActiveBloodWeapon == nullptr)
			return;
		m_pActiveBloodWeapon->Set_PartActive(false);
	}

}
void Client::Player::Update_RenderMatrix()
{
	_matrix WorldMatrix = XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());

	WorldMatrix.r[3] = XMVectorSet(XMVectorGetX(m_vRenderPos), XMVectorGetY(m_vRenderPos), XMVectorGetZ(m_vRenderPos), 1.f);

	XMStoreFloat4x4(&m_RenderWorldMatrix, WorldMatrix);
}

void Client::Player::Update_RenderPos(_float fTimeDelta)
{

	_float fPxY = XMVectorGetY(m_vPhysicsPos);
	_float fRenderY = XMVectorGetY(m_vRenderPos);

	m_vRenderPos = XMVectorSet(XMVectorGetX(m_vPhysicsPos), fRenderY, XMVectorGetZ(m_vPhysicsPos), 1.f);

	// 공중이면 즉시 추종 (낙하/점프)
	if (!m_bIsGrounded)
	{
		m_vRenderPos = m_vPhysicsPos;
		return;
	}

	// 분기 제거, 항상 보간으로 처리
	_float fFollowSpeed = (fPxY >= fRenderY) ? m_fRenderFollowUpSpeed : m_fRenderFollowDownSpeed;

	_float fRatio = 1.0f - expf(-fFollowSpeed * fTimeDelta);
	_float fNewY = fLerp(fRenderY, fPxY, fRatio);

	m_vRenderPos = XMVectorSet(XMVectorGetX(m_vPhysicsPos), fNewY, XMVectorGetZ(m_vPhysicsPos), 1.f);
}

void Client::Player::Create_Projectile(const PLAYERANIM_EVENT& PlayerAnimEvent)
{
	//명혈다썼으면 총입력불가
	CHECK_TRUE(m_pPlayerStatCom->Get_Myeonghyeol() <= 0);

	State* pCurState = m_pStateMachineCom->Get_CurrentState();
	if (pCurState == nullptr)
		return;

	if (PlayerAnimEvent.bCreateProjectile)
		pCurState->On_ProjectileEvent();
}

void Client::Player::Update_AnimationSpeed(const PLAYERANIM_EVENT& PlayerAnimEvent)
{
	if (PlayerAnimEvent.bControlAnimSpeed)
	{
		m_pMasterModel->Set_SingleAnimation_Speed(PlayerAnimEvent.fAnimationSpeed);
	}

}

void Client::Player::Enter_LadderState(const PlayerIntercation& e)
{
	_vector vSnapPos = XMLoadFloat4(&e.vSnapPos);

	_float fControllerYOffset = (1.0f * 0.5f) + 0.15f; // 0.65f
	_vector vControllerPos = vSnapPos + XMVectorSet(0.f, fControllerYOffset, 0.f, 0.f);
	m_pController->setPosition(ToPxExtendedVec3(vControllerPos));

	m_vPhysicsPos = vSnapPos;
	m_vRenderPos = vSnapPos;
	m_fVerticalVelocity = 0.f;

	m_pTransformCom->Set_State(DIRECTION::POSITION, vSnapPos);

	m_pTransformCom->LookDir(XMLoadFloat3(&e.vLook));


	Update_RenderMatrix();

	m_bClimbUp = e.bClimbUp;
	m_vLadderBottomPos = e.vLadderBottomPos;
	m_vLadderTopPos = e.vLadderTopPos;
	m_pStateMachineCom->Change_State(LADDER);

}

void Client::Player::Enter_DeadState()
{
	Camera_Player* pPlayerCamera = CAST(Camera_Player*)(m_pGameInstance->Get_Camera(static_cast<_uint>(CAMERA::PLAYER_FOCUS_01)));
	if (pPlayerCamera == nullptr)
		return;

	m_pLockOnTarget = nullptr;
	SkillManager::GetInstance()->Debug_SkillReset(); 

	m_bFocusState = false;
	m_pPlayerStatCom->Set_FocusState(false);
	m_pPlayerStatCom->Reduce_FocusGauge(m_pPlayerStatCom->Get_MaxFocusGauge());

	m_bPlayerDead = true;

	m_PlayerDeadAlarm.On();

	//우선 락온 초기화
	if (m_tInputDesc.bLockOn)
	{
		m_tInputDesc.bLockOn = false;
		pPlayerCamera->Clear_LockOnTarget();
	}
}

void Client::Player::Enter_CustomScene()
{
	//무기같은경우는 Update_Weapon()에서 처리중
	m_pStateMachineCom->Change_State(CUSTOM); //상태
	m_pActiveOuter->Set_PartActive(false); //아우터(망토) ->비활 
}

void Client::Player::Enter_MainScene()
{
	m_pStateMachineCom->Change_State(IDLE); //상태
	m_pActiveOuter->Set_PartActive(true); //아우터(망토) ->비활 
}

_bool Client::Player::Check_FocusStateSuperArmor() const
{
	if (!m_bFocusState)
		return false;

	_float fRand = static_cast<_float>(rand()) / static_cast<_float>(RAND_MAX); //0~1

	return fRand <= 0.33f; //로얄하트확률보다 낮으면 성공 (0.66)로 줄예정 , 3분의2로 성공 / 0.66너무 잘터진다 0.33으로 수정 
}

void Client::Player::Setting_Damage()
{
	if (m_eEnemyDamagePower == DAMAGEPOWER::NORMALPOWER || m_eEnemyDamagePower == DAMAGEPOWER::STRONGPOWER)
	{
		m_pMasterRig->Request_ClearAnimationUpper(0.1f);
		m_pUpperStateMachineCom->Change_State(Player::PLAYERUPPERSTATE_END); //상체 상태머신은 nullptr로 초기화
		m_pPlayerStatCom->Add_FocusGauge(10.f, m_iObjectID);
		m_pStateMachineCom->Change_State(DAMAGE);
		//COUT("노말/강");
	}
	else if (m_eEnemyDamagePower == DAMAGEPOWER::HEAVYPOWER || m_eEnemyDamagePower == DAMAGEPOWER::SPECIALPOWER)
	{
		m_pMasterRig->Request_ClearAnimationUpper(0.1f);
		m_pUpperStateMachineCom->Change_State(Player::PLAYERUPPERSTATE_END); //상체 상태머신은 nullptr로 초기화
		m_pPlayerStatCom->Add_FocusGauge(15.0f, m_iObjectID);
		m_pStateMachineCom->Change_State(DAMAGEBLOW);
	}
	else //없으면 우선 데미지로
	{
		m_pMasterRig->Request_ClearAnimationUpper(0.1f);
		m_pUpperStateMachineCom->Change_State(Player::PLAYERUPPERSTATE_END); //상체 상태머신은 nullptr로 초기화
		m_pPlayerStatCom->Add_FocusGauge(10.f, m_iObjectID);
		//COUT("나머지");
		m_pStateMachineCom->Change_State(DAMAGE);
	}
}

_bool Client::Player::CanUseSkill()
{
	if (m_tInputDesc.iSkillSlotKeyDown <0)
		return false;

	Skill_Base* pSkill = SkillManager::GetInstance()->Requset_Skill(m_tInputDesc.iSkillSlotKeyDown);
	if (pSkill == nullptr)
		return false;

	Player_Skill* pSkillState = dynamic_cast<Player_Skill*>(m_pStateMachineCom->Find_State(SKILL));
	if (pSkillState == nullptr)
		return false;

	pSkillState->Set_CurrentSkill(pSkill);
	m_pStateMachineCom->Change_State(SKILL);

	return true;
}

void Client::Player::Update_SkillEvent(const PLAYERANIM_EVENT& PlayerAnimEvent)
{
	Player_Skill* pSkillState = dynamic_cast<Player_Skill*>(m_pStateMachineCom->Find_State(SKILL));
	if (pSkillState == nullptr)
		return;

	if (PlayerAnimEvent.bKetsugiStart && m_bIsSkillAttempt)
	{
		m_bIsSkillAttempt = false;
		pSkillState->Get_CurrentSkill()->On_StartKetsugi(); //현재 재생중인 스킬의 명혈 감소 / 쿨타임 감소 시작 / 버프 등등
	}
}

void Client::Player::Update_Yeongeomjeonmu(_float fTimeDelta)
{
	m_fYeongeomTime += fTimeDelta;

	if (m_fYeongeomTime >= 0.1f)
	{
		_vector vMyPos = m_pTransformCom->Get_State(DIRECTION::POSITION);

		PxTransform pose(PxVec3(
			XMVectorGetX(vMyPos),
			XMVectorGetY(vMyPos),
			XMVectorGetZ(vMyPos)));

		//BoundingSphere DSDSD;
		//DSDSD.Center = Get_Position_Float3();
		//DSDSD.Radius = 3.3f;
		//
		//Add_Debug_Sphere(DSDSD);

		// 거리에 따라 Overlap 또는 Sweep 선택
		vector<PHYSX_USER_DATA*> vecHitData;
		vector<_float3> vecHitPositions; // 접촉 위치 저장용

		PHYSX_OVERLAP_RESULT overlapResult = m_pGameInstance->PhysX_Overlap_Sphere(m_fYeonmgeomRadius, pose);

		GameObject* pBsetGameObject = nullptr; //조건에 맞는 놈 구분 

		if (overlapResult.bHit) //맞은놈
		{
			vecHitData = overlapResult.vecUserData; //맞은놈의 데이터
			vecHitPositions = overlapResult.vecHitData;
		}

		for (_uint i = 0; i < vecHitData.size(); ++i)
		{
			PHYSX_USER_DATA* pUserData = vecHitData[i];

			if (pUserData == nullptr)
				continue;
			if (pUserData->iObjectID == m_iObjectID) //같은놈
				continue;
			if (pUserData->eActorType != PX_ACTOR_TYPE::MONSTER) //몬스터 아니라면
				continue;

			GameObject* pMonster = pUserData->pOwner; //이게 이제 맞은놈 누구인지
			if (pMonster == nullptr)
				continue;

			// 데미지 이벤트 발행
			_float3 vAttackDir = {};
			_vector vLook = m_pTransformCom->Get_State(DIRECTION::LOOK);
			XMStoreFloat3(&vAttackDir, XMVector3Normalize(vLook));

			DAMAGE_EVENT damageEvent = {};
			damageEvent.iAttackerID = m_iObjectID;
			damageEvent.iTargetID = pUserData->iObjectID;
			damageEvent.fDamage = m_pGameInstance->RandomValue(40.f, 52.f); // 랜덤값 
			damageEvent.vHitPosition = vecHitPositions[i]; // sweep/overlap 결과의 실제 접촉 위치 사용
			damageEvent.vHitDirection = vAttackDir;
			damageEvent.fKnockbackForce = m_fKnockbackForce;
			damageEvent.eDamagePower = m_pStateMachineCom->Get_CurrentState_DamagePower();
			damageEvent.pDamageOwner = this;

			m_pGameInstance->Publish<DAMAGE_EVENT>(damageEvent);
			m_pGameInstance->Play_Sound("Action_Renketsu_SummonBlade_Hit_00", 0.1f); //데미지 발행 성공하면 타격 이펙트 사운드 
		}

		m_fYeongeomTime = 0.f;
	}





}

INPUT_DIR Client::Player::Calculate_Dir(_fvector vCurrent, _fvector vDesired)
{
	INPUT_DIR InputDir = {INPUT_DIR::INPUT_END};

	_vector current = XMVectorSet(XMVectorGetX(vCurrent), 0.f, XMVectorGetZ(vCurrent), 0.f);
	_vector desired = XMVectorSet(XMVectorGetX(vDesired), 0.f, XMVectorGetZ(vDesired), 0.f);

	_float fDot = XMVectorGetX(XMVector3Dot(current, XMVector3Normalize(desired))); //이 내적을 통해서 1이면 서로 같은 방향 -1이면 반대방향 ,cos

	_vector vCross = XMVector3Cross(current, XMVector3Normalize(desired));
	_float fCrossY = XMVectorGetY(vCross);

	if (fDot > 0.75f) // Forward
	{
		InputDir = INPUT_DIR::UP;
	}
	else if (fDot < -0.75f) // Back
	{
		InputDir = INPUT_DIR::DOWN;
	}
	else if (fCrossY > 0.f) //right  기준
	{
		if (fDot > 0.25f) InputDir = INPUT_DIR::RIGHT_UP; //정면오른쪽에서 맞으면 BR
		else if (fDot < -0.25f) InputDir = INPUT_DIR::RIGHT_DOWN; //뒤오른쪽에서 맞으면 FR
		else InputDir = INPUT_DIR::RIGHT; //오른쪽 // R
	}
	else if (fCrossY<0.f) //left 기준
	{
		if (fDot > 0.25f) InputDir = INPUT_DIR::LEFT_UP; //정면왼쪽 BR
		else if (fDot < -0.25f) InputDir = INPUT_DIR::LEFT_DOWN; //뒤왼족 FR
		else InputDir = INPUT_DIR::LEFT;//왼쪽 // R 
	}

	return InputDir;
}
INPUT_DIR Client::Player::Calculate_LockOnDir(_fvector vMoveDir, GameObject* pGameObject)
{
	INPUT_DIR InputDir = { INPUT_DIR::INPUT_END };

	if (pGameObject == nullptr)
		return InputDir;

	if (XMVectorGetX(XMVector3LengthSq(vMoveDir)) < 1e-6f)
		return InputDir;

	_vector movedir = XMVectorSet(XMVectorGetX(vMoveDir), 0.f, XMVectorGetZ(vMoveDir), 0.f);

	_vector vMyPos = m_pTransformCom->Get_State(DIRECTION::POSITION);
	_vector vTargetPos = pGameObject->Get_Position();

	_vector vDir = vTargetPos - vMyPos;

	vDir = XMVectorSet(XMVectorGetX(vDir), 0.f, XMVectorGetZ(vDir), 0.f);
	vDir = XMVector3Normalize(vDir);

	_vector vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.0f, 0.f, 0.f), vDir));//외적해서 right 뽑아주고

	_vector vMyMoveDir = XMVector3Normalize(movedir);

	_float fDotFoward = XMVectorGetX(XMVector3Dot(vMyMoveDir, vDir)); // 정면,반대 
	_float fDotRight = XMVectorGetX(XMVector3Dot(vMyMoveDir, vRight)); //우측 , 좌측 

	if (fDotFoward > 0.75f) return InputDir = INPUT_DIR::UP; //F
	if (fDotFoward < -0.75f) return InputDir = INPUT_DIR::DOWN; //B

	if (fDotRight > 0.f)
	{
		if (fDotFoward > 0.25f) return INPUT_DIR::RIGHT_UP;//FR
		if (fDotFoward < -0.25f) return INPUT_DIR::RIGHT_DOWN; //BR
		return INPUT_DIR::RIGHT; //R
	}
	else
	{
		if (fDotFoward > 0.25f) return INPUT_DIR::LEFT_UP; //FL
		if (fDotFoward < -0.25f) return INPUT_DIR::LEFT_DOWN; //BL
		return INPUT_DIR::LEFT; //L
	}

	return InputDir;

}
Player::DAMAGE_LEVEL Client::Player::Calculate_DamageLevel(_float fLoseHpRatio, DAMAGEPOWER eDamagePower)
{
	_bool bHigh = (fLoseHpRatio >= 20.f); //20보다 클경우

	if (eDamagePower == DAMAGEPOWER::NORMALPOWER)
		return bHigh ? Player::DAMAGE2 : Player::DAMAGE1;

	//우선 강공격
	return bHigh ? Player::DAMAGE4 : Player::DAMAGE3;
		
}
INPUT_DIR Client::Player::Convert8To4Dir(INPUT_DIR eInputDir)
{
	switch (eInputDir)
	{
	case INPUT_DIR::LEFT_UP:
	case INPUT_DIR::LEFT_DOWN:
		return INPUT_DIR::LEFT;

	case INPUT_DIR::RIGHT_UP:
	case INPUT_DIR::RIGHT_DOWN:
		return INPUT_DIR::RIGHT;

	case INPUT_DIR::UP:
	case INPUT_DIR::DOWN:
	case INPUT_DIR::LEFT:
	case INPUT_DIR::RIGHT:
		return eInputDir;
	default: return eInputDir;
	}
}
void Client::Player::Sync_BoneMatrix(WEAPON_TYPE _type)
{
	m_pWeaponBoneMatrix = const_cast<_float4x4*>(m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr("TrailStartSocket"));	// 콜라이더용
	m_pWeaponBoneMatrixEnd = const_cast<_float4x4*>(m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr("TrailEndSocket"));	// 콜라이더용
	//m_pWeaponTrailRoot = const_cast<_float4x4*>(m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr("TrailEndSocket"));
	//m_pWeaponTrailTip = const_cast<_float4x4*>(m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr("TrailStartSocket"));
}

void Client::Player::Change_State(_uint _state)
{
	m_pStateMachineCom->Change_State(_state);
}

void Client::Player::OnDamaged(const DAMAGE_EVENT& DamageEvent)
{
	//죽는 상태 + Hp 0이하라면 데미지 안받도록 애니메이션에 무적이벤트 넣어놓긴했는데 아직 디졸브 처리가 안돼서 넣어놓긴함
	if (m_bIsSkillAttempt)
	{
		Player_Skill* pSkillState = dynamic_cast<Player_Skill*>(m_pStateMachineCom->Find_State(SKILL));
		if (pSkillState == nullptr)
			return;

		Skill_Base* pBase = pSkillState->Get_CurrentSkill();

		if (pBase)
			pBase->Cancle_Request();

		m_bIsSkillAttempt = false;
	}


	if (m_pStateMachineCom->Get_CurStateType() == DEATH || m_pPlayerStatCom->Get_Hp() <=0.99f) 
		return; 

	if (m_bPlayCineamtic) //시네마틱 재생중에는 피격 발생안하도록
		return;

	if (m_bSuccessBackStab)
		return; 

	if (m_tPlayerRunTimeEvent.bCanParry()) // 만약에 패링이 활성화 되어있고 피격(OnDamaged) 일반몹이라면 (DamageEvent.pDamageOwner의 Get_MonsterType()해서
	{
		_vector vPlayerLook = m_pTransformCom->Get_State(DIRECTION::LOOK);
		
		_float fDot = XMVectorGetX(XMVector3Dot(vPlayerLook, XMVector3Normalize(m_vEnemyHitDirection))); //이 내적을 통해서 1이면 서로 같은 방향 -1이면 반대방향 ,cos

		if (fDot < -0.8f) // 내적해서
		{
			// 만약에 패링이 활성화 되어있고 피격(OnDamaged를 상대 콜이더 활성화 + )을 당했더라면
		
			// 몬스터가 자체적으로 함수 실행하도록
			m_pTargetMonster = DCAST(Monster*)(DamageEvent.pDamageOwner);
			if (m_pTargetMonster == nullptr)
				return;
			if (!m_pTargetMonster->Can_Parry())
				return;

			m_pTargetMonster->Change_State(Monster::MS_REPEL);

			//_vector vDir = DamageEvent.pDamageOwner->Get_Position() - m_pTransformCom->Get_State(DIRECTION::POSITION);
			//XMVector3Normalize(vDir);
			//m_pTransformCom->LookDir(vDir);
			m_bPlayCineamtic = true;
			//m_pStateMachineCom->Change_State(SPECIALSUCK);
			m_tCinematicAlarm.Elapsed = 0.f;
			m_tCinematicAlarm.On();
			
			// 카메라 흔들림 이벤트 발행
			// 카메라 이벤트도 발행
			CameraEvent shakeevent;
			shakeevent.eCameraAction = CAMERA_ACTION::SHAKE;
			shakeevent.tShake = ShakePreset::SPSuck();
			m_pGameInstance->Publish(shakeevent);

			// 데미지 이벤트 발행
			//_float3 vAttackDir = {};
			//_vector vLook = m_pTransformCom->Get_State(DIRECTION::LOOK);
			//XMStoreFloat3(&vAttackDir, XMVector3Normalize(vLook));

			return;
		}
	}

	if (m_tPlayerRunTimeEvent.bCanInvincible()) //무적은 피격애니메이션 재생 x 데미지 x 
	{
		if (m_pStateMachineCom->Get_CurStateType() == ROLL || m_pStateMachineCom->Get_CurStateType() == BACKSTEP)
		{
			m_pPlayerStatCom->Add_FocusGauge(25.f, m_iObjectID);
		}
		COUT("무적상태");
		return;
	}

	CameraEvent shakeevent;
	shakeevent.eCameraAction = CAMERA_ACTION::SHAKE;
	shakeevent.tShake = ShakePreset::HitStop_PlayerDamage();
	m_pGameInstance->Publish(shakeevent);

	_float fLoseHpRatio = {};

	//슈퍼하트 + 집중 게이지 상태 
	if (m_pPlayerStatCom->Check_RoyalHeartSuperArmor()) //3분의2 확률로 슈퍼아머와 동일하게 처리
	{
		m_pPlayerStatCom->Apply_Damage((_int)RandomDamage(DamageEvent.fDamage)); //플레이어 Hp 데미지 적용 
		m_pPlayerStatCom->Add_FocusGauge(10.f, m_iObjectID);

		if (m_pPlayerStatCom->Get_Hp() <= 0.f)
		{
			Setting_Damage();
		}

		//COUT("로얄하트 슈퍼아머상태");
		return;
	}

	if (Check_FocusStateSuperArmor()) //집중 상태에서 확률
	{
		m_pPlayerStatCom->Apply_Damage((_int)RandomDamage(DamageEvent.fDamage)); //플레이어 Hp 데미지 적용 
		//포커스 게이지 증가 안해야하므로
		if (m_pPlayerStatCom->Get_Hp() <= 0.f)
		{
			Setting_Damage();
		}

		//COUT("집중상태에서 슈퍼아머 적용");
		return;
	}

	if (m_tPlayerRunTimeEvent.bCanSuperArmor()) //슈퍼아머는 데미지 적용 o 피격 애니메이션 재생 x 
	{
		m_pPlayerStatCom->Apply_Damage((_int)RandomDamage(DamageEvent.fDamage)); //플레이어 Hp 데미지 적용 
		m_pPlayerStatCom->Add_FocusGauge(10.f, m_iObjectID);

		if (m_pPlayerStatCom->Get_Hp() <= 0.f)
		{
			Setting_Damage();
		}

		//COUT("슈퍼아머상태");
		return; 
	}
	else if (m_bOnGuard)
	{
		fLoseHpRatio = m_pPlayerStatCom->ApplyDamageAndGetLoseHpRatio((_int)RandomDamage(DamageEvent.fDamage / 2.5f));
		//Hp도 상대방의 공격파워에 따라서 적용해도되고
		//스태미나는 상대방의 공격파워에 따라서 적용해도될듯
		m_pPlayerStatCom->Decrease_Stamina(30.f);
	}
	else
	{
		m_fLoseHpRatio = m_pPlayerStatCom->ApplyDamageAndGetLoseHpRatio((_int)RandomDamage(DamageEvent.fDamage*0.2f)); //플레이어 Hp 데미지 적용 
	}


	if (m_bOnGuard)
	{
		if (m_pPlayerStatCom->Get_Stamina() <= 0.0f) //얘는 조건을 bZeroStamina 말고 현재 스태미나가 값을 기준으로 해야겠는데
		{
			m_pMasterRig->Request_ClearAnimationUpper(0.1f);
			m_pPlayerStatCom->Add_FocusGauge(10.f, m_iObjectID);
			m_pStateMachineCom->Change_State(GUARD_BREAK);
			return;
		}
		else
		{
			m_pMasterRig->Request_ClearAnimationUpper(0.1f);
			m_pPlayerStatCom->Add_FocusGauge(5.f, m_iObjectID);
			m_pStateMachineCom->Change_State(GUARDHIT); //가드상태일때 피격모션

			return;
		}
	}
	else 
	{	
		Setting_Damage();
	}

	//가드 히트 상태 진입 + 일때 스태미나 감소 + HP 감소 (이거 감소는 상대 공격력에 따라 비율 또는 랜덤값 비율로 해야할듯) + 가드 히트일때는 AttackStrong처럼 재생 막아주고 + 무기마다 가드성능에 따라서

	// 출혈 이펙트
	ParticleSystem* pBloodEffect = CAST(ParticleSystem*)(POOLING->Acquire_Effect(POOL_ID::EFFECT_HIT_BLOOD));
	pBloodEffect->Set_WorldPosition(DamageEvent.vHitPosition);
	pBloodEffect->OnSpawn(nullptr);
}

void Client::Player::Find_LockOnTarget()
{
	Layer* pMonsterLayer = m_pGameInstance->Get_Layer(L"Layer_Monster");
	if (pMonsterLayer == nullptr)
		return;

	vector<GameObject*>& vecGameObjects = pMonsterLayer->Get_VecGameObjects();

	Camera* pPlayerCamera = m_pGameInstance->Get_Camera(static_cast<_uint>(CAMERA::PLAYER_FOCUS_01));
	if (pPlayerCamera == nullptr)
		return;

	Transform* pCameraTransformCom = pPlayerCamera->Get_Transform();

	_vector vCameraLook = pCameraTransformCom->Get_State(DIRECTION::LOOK);

	_vector vCamerPos = pPlayerCamera->Get_Position();

	_float3 vStoreCamPos;
	XMStoreFloat3(&vStoreCamPos, vCamerPos);

	_float fCamFov = cosf(XMConvertToRadians(30.f)); //카메라 Fov값 60으로 기억해서 30으로 맞춰놓음
	_float fBsetDivide = -FLT_MAX; //조건에 맞는 놈 구분 

	GameObject* pBsetGameObject = nullptr; //제일 시야각에 직선인놈 찿아주기

	PxVec3 vPxCamPos = ToPxVec3(vStoreCamPos);

	for (auto& pMonster : vecGameObjects) //나중에 이거 모든 몬스터 순회하는것도 수정해야할수도
	{
		if (pMonster->Is_Active() == false)
			continue;

		Transform* pMonsterTransformCom = pMonster->Get_Transform();
				
		_vector vDir = pMonsterTransformCom->Get_State(DIRECTION::POSITION) - vCamerPos;
	
		_float fDist = XMVectorGetX(XMVector3Length(vDir));

		if (fDist > m_fMaxLockOnTargetDist)
			continue;

		vDir = XMVector3Normalize(vDir);

		_float fDot = XMVectorGetX(XMVector3Dot(vCameraLook, vDir));
		if (fDot < fCamFov) continue;

		_float3 vStoreDir;
		XMStoreFloat3(&vStoreDir, vDir);

		PxVec3 vPxDir = ToPxVec3(vStoreDir);

		PHYSX_RAYCAST_RESULT PxResult = m_pGameInstance->PhysX_Raycast(vPxCamPos, vPxDir, m_fMaxLockOnTargetDist);
		
		if (!PxResult.bHit) //안맞은경우
			continue;

		if (PxResult.pUserData == nullptr) //없는경우 
			continue;

		_float fDivide = fDot * 10.0f - (fDist / m_fMaxLockOnTargetDist);

		if (fDivide > fBsetDivide) //제일 직선에 있는놈 검사
		{
			fBsetDivide = fDivide;
			pBsetGameObject = pMonster;
		}
	}

	Camera_Player* pRealPlayerCamera = dynamic_cast<Camera_Player*>(pPlayerCamera);
	if (pBsetGameObject == nullptr)
		return;

	pRealPlayerCamera->Set_LockOnTarget(pBsetGameObject);

	//여기까지오면 락온
	m_tInputDesc.bLockOn = true;

	m_pLockOnTarget = pBsetGameObject;

}
void Client::Player::Find_BestLockOnTarget()
{
	Camera* pPlayerCamera = m_pGameInstance->Get_Camera(static_cast<_uint>(CAMERA::PLAYER_FOCUS_01));
	if (pPlayerCamera == nullptr)
		return;

	Transform* pCameraTransformCom = pPlayerCamera->Get_Transform();

	_vector vCameraLook = pCameraTransformCom->Get_State(DIRECTION::LOOK);
	_vector vCamerPos = pPlayerCamera->Get_Position();

	_float3 vStoreCamPos;
	XMStoreFloat3(&vStoreCamPos, vCamerPos);

	_float fCamFov = cosf(XMConvertToRadians(30.f)); //카메라 Fov값 60으로 기억해서 30으로 맞춰놓음
	_float fBsetDivide = -FLT_MAX; //조건에 맞는 놈 구분 

	GameObject* pBsetGameObject = nullptr; //제일 시야각에 직선인놈 찿아주기

	PxVec3 vPxCamPos = ToPxVec3(vStoreCamPos);

	PxTransform pose(vPxCamPos);
	PHYSX_OVERLAP_RESULT overlapResult = m_pGameInstance->PhysX_Overlap_Sphere(m_fMaxLockOnTargetDist, pose);

	if (!overlapResult.bHit)
		return;

	for (_uint i = 0; i < overlapResult.vecUserData.size(); ++i)
	{
		PHYSX_USER_DATA* pUserData = overlapResult.vecUserData[i];
		if (pUserData == nullptr)
			continue;
		if (pUserData->eActorType != PX_ACTOR_TYPE::MONSTER)
			continue;
		GameObject* pMonster = pUserData->pOwner;
		if (pMonster == nullptr || !pMonster->Is_Active())
			continue;

		_vector vDir = pMonster->Get_Transform()->Get_State(DIRECTION::POSITION) - vCamerPos;
		_float fDist = XMVectorGetX(XMVector3Length(vDir));
		vDir = XMVector3Normalize(vDir);
		_float fDot = XMVectorGetX(XMVector3Dot(vCameraLook, vDir));

		if (fDot < fCamFov) //내적결과가 카메라 시야각보다 작은경우
			continue;

		_float fScore = fDot * 10.0f - (fDist / m_fMaxLockOnTargetDist);
		if (fScore > fBsetDivide)
		{
			fBsetDivide = fScore;
			pBsetGameObject = pMonster;
		}
	}

	if (pBsetGameObject == nullptr)
		return;

	//여기서 한번더 레이캐스트를 이용해서 몬스터인지 체크
	_vector vDir = pBsetGameObject->Get_Transform()->Get_State(DIRECTION::POSITION) - vCamerPos;
	_float fMonsterDist = XMVectorGetX(XMVector3Length(vDir)); //몬스터 거리 가져와서
	vDir = XMVector3Normalize(vDir);
	_float3 vStoreDir;
	XMStoreFloat3(&vStoreDir, vDir);
	PxVec3 vPxDir = ToPxVec3(vStoreDir);
	PHYSX_RAYCAST_RESULT PxResult = m_pGameInstance->PhysX_Raycast(vPxCamPos, vPxDir, m_fMaxLockOnTargetDist);

	if (!PxResult.bHit) //레이캐스트 안맞았으면 성공 / 맞았으면 엄폐물 있다고 판단 
	{
		Camera_Player* pRealPlayerCamera = dynamic_cast<Camera_Player*>(pPlayerCamera);
		pRealPlayerCamera->Set_LockOnTarget(pBsetGameObject);
		m_tInputDesc.bLockOn = true;
		m_pLockOnTarget = pBsetGameObject;
	}
	else if (PxResult.fDistance >= fMonsterDist - 0.5f) //레이 결과 거리가 몬스터의 거리보다 크다면 몬스터 맞았다고 판단 , 대부분 여기에서 true
	{
		Camera_Player* pRealPlayerCamera = dynamic_cast<Camera_Player*>(pPlayerCamera);
		pRealPlayerCamera->Set_LockOnTarget(pBsetGameObject);
		m_tInputDesc.bLockOn = true;
		m_pLockOnTarget = pBsetGameObject;
	}
	else
	{
		return; 
	}


}
void Client::Player::Find_BackStabTarget()
{
	_vector vMyPos = m_pTransformCom->Get_State(DIRECTION::POSITION);

	PxTransform pose(PxVec3(
		XMVectorGetX(vMyPos),
		XMVectorGetY(vMyPos),
		XMVectorGetZ(vMyPos)));

	// 거리에 따라 Overlap 또는 Sweep 선택
	vector<PHYSX_USER_DATA*> vecHitData;
	vector<_float3> vecHitPositions; // 접촉 위치 저장용

	PHYSX_OVERLAP_RESULT overlapResult = m_pGameInstance->PhysX_Overlap_Sphere(2.0f, pose); //스피어사이즈2정도줌 널널한가? 

	_float fBsetDivide = -FLT_MAX; //조건에 맞는 놈 구분 
	GameObject* pBsetGameObject = nullptr; //조건에 맞는 놈 구분 

	if (overlapResult.bHit) //맞은놈
	{
		vecHitData = overlapResult.vecUserData; //맞은놈의 데이터
		vecHitPositions = overlapResult.vecHitData;
	}
	
	for (_uint i = 0; i < vecHitData.size(); ++i)
	{
		PHYSX_USER_DATA* pUserData = vecHitData[i];

		if (pUserData == nullptr) 
			continue;
		if (pUserData->iObjectID == m_iObjectID) //같은놈
			continue;
		if (pUserData->eActorType != PX_ACTOR_TYPE::MONSTER) //몬스터 아니라면
			continue;
		if (m_setHitTargets.count(pUserData->iObjectID) > 0) //카운트체크
			continue;
		m_setHitTargets.insert(pUserData->iObjectID); //몬스터 ID저장

		Monster* pMonster = CAST(Monster*)(pUserData->pOwner); //이게 이제 맞은놈 누구인지
		if (pMonster == nullptr)
			continue;
		if (!pMonster->Can_BackAttack()) // 백어택 가능한 몬스터가 아니면
			continue;

		//여기서 이제 조건에 맞는놈을 찾아줘야겠지
		_vector vToMonster = pMonster->Get_Position() - m_pTransformCom->Get_State(DIRECTION::POSITION);
		_float fDist = XMVectorGetX(XMVector3Length(vToMonster));

		_vector vPlayerLook = XMVector3Normalize(m_pTransformCom->Get_State(DIRECTION::LOOK));
		_vector vMonsterLook = XMVector3Normalize(pMonster->Get_Transform()->Get_State(DIRECTION::LOOK));
		_float fDot = XMVectorGetX(XMVector3Dot(vPlayerLook, vMonsterLook));

		// 플레이어가 몬스터 방향을 보고 있는지
		_vector vDirToMonster = XMVector3Normalize(vToMonster);
		_float fDotDir = XMVectorGetX(XMVector3Dot(vPlayerLook, vDirToMonster));

		if (fDot < 0.9f) continue;  // 플레이어 Look과 몬스터 Look 같은 방향 인지
		if (fDotDir < 0.7f) continue;   // 플레이어가 몬스터 쪽을 안 보고 있으면 건너뛰고 

		_float fScore = fDotDir * 10.f - fDist; // 정면에 가까울수록 + 거리 가까울수록 높은 점수
		if (fScore > fBsetDivide)
		{
			fBsetDivide = fScore;
			pBsetGameObject = pMonster;

			//여기까지오면 뒤잡성공인데 만약 보스몬스터같은거는 하면안되니까 pMonster의 타입검사까지 진행해야함.
			m_bSuccessBackStab = true;

			if (m_bSuccessBackStab)
			{
				//m_pBackStabTarget = pBsetGameObject;
				
				m_pTargetMonster = CAST(Monster*)(pBsetGameObject);
				m_pTargetMonster->Set_StunState(Monster::STUN_BACKSTAB);
				m_pTargetMonster->Change_State(Monster::MS_STUN);

				_vector playerPos = Get_Position();
				_vector playerLook = Get_Look();
				_vector monsterPos = playerPos + playerLook * 1.5f;
				_vector monsterLookAt = playerPos + playerLook * 3.f;
				m_pTargetMonster->Teleport(monsterPos);
				m_pTargetMonster->LookAt(monsterLookAt);
			}
		}
	}




}
void Client::Player::Find_NearestMonsterAndRotation(_float fMaxAngle, _float fLerpDuration)
{
	_vector vMyPos = m_pTransformCom->Get_State(DIRECTION::POSITION);
	_float3 vStoreMyPos;
	XMStoreFloat3(&vStoreMyPos, vMyPos);

	PxTransform pose(ToPxVec3(vStoreMyPos));
	PHYSX_OVERLAP_RESULT overlapResult = m_pGameInstance->PhysX_Overlap_Sphere(m_fAttackAutoRotationDist, pose); 

	if (!overlapResult.bHit)
		return;

	_float fBestDist = FLT_MAX;
	GameObject* pBestMonster = nullptr;

	_vector vPlayerLook = XMVector3Normalize(m_pTransformCom->Get_State(DIRECTION::LOOK));

	for (_uint i = 0; i < overlapResult.vecUserData.size(); ++i)
	{
		PHYSX_USER_DATA* pUserData = overlapResult.vecUserData[i];
		if (pUserData == nullptr)
			continue;
		if (pUserData->iObjectID == m_iObjectID)
			continue;
		if (pUserData->eActorType != PX_ACTOR_TYPE::MONSTER)
			continue;

		GameObject* pMonster = pUserData->pOwner;
		if (pMonster == nullptr || !pMonster->Is_Active())
			continue;

		_vector vDir = pMonster->Get_Transform()->Get_State(DIRECTION::POSITION) - vMyPos;
		_float fDist = XMVectorGetX(XMVector3Length(vDir));
		vDir = XMVector3Normalize(vDir);

		_float fDot = XMVectorGetX(XMVector3Dot(vPlayerLook, vDir));
		if (fDot < cosf(XMConvertToRadians(120.f))) // 시야각 밖이면 무시
			continue;

		if (fDist < fBestDist) // 제일 가까운 놈
		{
			fBestDist = fDist;
			pBestMonster = pMonster;
		}
	}

	if (pBestMonster == nullptr)
		return;

	_vector vToMonster = pBestMonster->Get_Transform()->Get_State(DIRECTION::POSITION) - vMyPos;
	vToMonster = XMVectorSet(XMVectorGetX(vToMonster), 0.f, XMVectorGetZ(vToMonster), 0.f);
	vToMonster = XMVector3Normalize(vToMonster);

	m_vCacheAttackMoveDir = vToMonster;
	Start_AttackDir_RotationLerp(fMaxAngle, fLerpDuration);

}
void Client::Player::Update_LockOn(_float fTimeDelta)
{
	if (m_tInputDesc.bLockOn)
	{
		Camera_Player* pPlayerCamera = CAST(Camera_Player*)(m_pGameInstance->Get_Camera(static_cast<_uint>(CAMERA::PLAYER_FOCUS_01)));
		if (pPlayerCamera == nullptr)
			return;

		Monster* pMonster = CAST(Monster*)(m_pLockOnTarget);
		if (pMonster == nullptr)
			return;

		if (pMonster->Is_Killed())
		{
			m_tInputDesc.bLockOn = false;
			pPlayerCamera->Clear_LockOnTarget();
			m_pLockOnTarget = nullptr;
		}

		if (m_pGameInstance->KeyDown(DIK_Q) || m_pGameInstance->MouseDown(MOUSEKEYSTATE::WHEEL))
		{
			m_tInputDesc.bLockOn = false;
			pPlayerCamera->Clear_LockOnTarget();
			m_pLockOnTarget = nullptr;

		}
	}
	else if (m_pGameInstance->KeyDown(DIK_Q) || m_pGameInstance->MouseDown(MOUSEKEYSTATE::WHEEL))//여기서 이제 락온 함수 실행 
	{
		//Find_LockOnTarget();
		Find_BestLockOnTarget();
	}

		//거리멀어져도 락온해제 정도 ? 

}
/******************************************************* 무기 변경 함수 *******************************************************/



//////////////////////////////////////////////////////// 컨테이너 추가 함수 ////////////////////////////////////////////////////////
HRESULT Client::Player::Ready_Components()
{
	CHECK_FAILED(Add_Component(m_iLevel, Proto_Com_StateMachine, Com_StateMachine, RCAST(Component**)(&m_pStateMachineCom)), E_FAIL);
	CHECK_FAILED(Add_Component(m_iLevel, Proto_Com_StateMachine, L"UpperStateMachine_Component",RCAST(Component**)(&m_pUpperStateMachineCom)), E_FAIL);

	MinimapRenderComponent::MINIMAPDESC MinimapDesc;
	MinimapDesc.pOwner = this;
	MinimapDesc.eType = MinimapRenderComponent::ICON_TYPE::PLAYER;
	MinimapDesc.m_bIsTrace = true;	//발자국남김
	MinimapDesc.m_bUseRotation = true;

	CHECK_FAILED(Add_Component(_UINT(LEVEL::STATIC), Proto_Com_Minimap, Com_Minimap, RCAST(Component**)(&m_pMinimapRendercom), &MinimapDesc), E_FAIL);

	Player_Stat::PLAYERSTAT_DESC PlayerStatDesc;
	PlayerStatDesc.eBloodCode = BLOODCODE::BLOODCODE_END; 
	PlayerStatDesc.fMaxHp = 845.f;
	PlayerStatDesc.fCurrentHp = PlayerStatDesc.fMaxHp;
	PlayerStatDesc.iAttack = 100;
	PlayerStatDesc.iDefense = 20;
	PlayerStatDesc.iMaxMyeonghyeol = 30.f;
	PlayerStatDesc.iCurrentMyeonghyeol = PlayerStatDesc.iMaxMyeonghyeol;
	PlayerStatDesc.iHaze = 0;
	PlayerStatDesc.fMaxStamina = 200;
	PlayerStatDesc.fCurrentStamina = PlayerStatDesc.fMaxStamina;
	PlayerStatDesc.fLowStamina = 20;
	PlayerStatDesc.fMaxFocusGauge = 120;
	PlayerStatDesc.fCurrentFocusGauge =0;

	//공유하는 스탯컴포넌트 중 플레이어 임을 표시
	PlayerStatDesc.m_bPlayer = true;

	CHECK_FAILED(Add_Component(m_iLevel, Proto_Com_PlayerStat, Com_Stat, RCAST(Component**)(&m_pStatCom), &PlayerStatDesc), E_FAIL);

	m_pPlayerStatCom = dynamic_cast<Player_Stat*>(m_pStatCom);
	if (m_pPlayerStatCom == nullptr)
		return E_FAIL;

	Safe_AddRef(m_pPlayerStatCom);

	//인벤토리매니저 헤이즈갱신
	InventoryManager::GetInstance()->Set_Haze(PlayerStatDesc.iHaze);



	Bounding_AABB::BOUNDAABB_DESC Desc;
	Desc.vCenter = _float3(0.f, 1.0f, 0.f); // 플레이어 몸통 위치
	Desc.vExtents = _float3(0.5f, 1.0f, 0.5f); // 적당한 크기

	Add_Collider(COLLISION_GROUP::PLAYER, COLLIDER::AABB, &m_pColliderCom, &Desc);

	return S_OK;
}

HRESULT Client::Player::Ready_PartObjects()
{
	//MasterRig == 마스터 본(파츠들의 중심이 되는 뼈)
	Player_MasterRig::MASTERRIG_DESC MasterRigDesc = {};
	MasterRigDesc.pParentMatrix = Get_RenderWorldMatrixPtr();
	MasterRigDesc.pPlayer = this;
	CHECK_FAILED(__super::Add_PartObject(m_iLevel, Proto_GameObject(L"Player_MasterRig"), TEXT("Part_MasterRig"), &MasterRigDesc), E_FAIL);

	m_pMasterRig = dynamic_cast<Player_MasterRig*>(Find_PartObject(TEXT("Part_MasterRig")));
	if (m_pMasterRig == nullptr)
		return E_FAIL;
	m_pMasterModel = m_pMasterRig->Get_Model();
	Safe_AddRef(m_pMasterModel);
	Safe_AddRef(m_pMasterRig);

	//바디
	Player_Body::BODY_DESC BodyDesc = {};
	BodyDesc.pParentMatrix = Get_RenderWorldMatrixPtr();
	BodyDesc.pMasterRig = m_pMasterRig;
	BodyDesc.pDissolveTime = &m_fTimeElapsed;
	CHECK_FAILED(__super::Add_PartObject(m_iLevel, Proto_GameObject(L"Player_Body"), TEXT("Part_Body"), &BodyDesc, true), E_FAIL);


	//아우터
	Player_Outer::OUTER_DESC OuterDesc = {};
	OuterDesc.pParentMatrix = Get_RenderWorldMatrixPtr();
	OuterDesc.pMasterRig = m_pMasterRig;
	OuterDesc.pDissolveTime = &m_fTimeElapsed;
	CHECK_FAILED(__super::Add_PartObject(m_iLevel, Proto_GameObject(L"Player_Outer"), TEXT("Part_Outer"), &OuterDesc), E_FAIL);

	////헤어
	Player_Hair::HAIR_DESC HairDesc = {};
	HairDesc.pParentMatrix = Get_RenderWorldMatrixPtr();
	HairDesc.pMasterRig = m_pMasterRig;
	HairDesc.pDissolveTime = &m_fTimeElapsed;
	CHECK_FAILED(__super::Add_PartObject(m_iLevel, Proto_GameObject(L"Player_Hair"), TEXT("Part_Hair"), &HairDesc, true), E_FAIL);

	//헤드
	Player_Head::HEAD_DESC HeadDesc = {};
	HeadDesc.pParentMatrix = Get_RenderWorldMatrixPtr();
	HeadDesc.pMasterRig = m_pMasterRig;
	HeadDesc.pDissolveTime = &m_fTimeElapsed;
	CHECK_FAILED(__super::Add_PartObject(m_iLevel, Proto_GameObject(L"Player_Head"), TEXT("Part_Head"), &HeadDesc, true), E_FAIL);

	//블러드웨폰
	Player_BloodWeapon::BLOODWEAPON_DESC BloodWeaponDesc = {};
	BloodWeaponDesc.pParentMatrix = Get_RenderWorldMatrixPtr();
	BloodWeaponDesc.pMasterRig = m_pMasterRig;
	CHECK_FAILED(__super::Add_PartObject(m_iLevel, Proto_GameObject(L"Player_BloodWeapon"), TEXT("Part_BloodWeapon"), &BloodWeaponDesc), E_FAIL);

	//총검 무기
	PWeapon_Bayonet::WEAPONBAYONET_DESC BayonetDesc = {};
	BayonetDesc.iSocketIndex = m_pMasterRig->Get_SocketIndex("RightHandAttachSocket");
	BayonetDesc.pMasterRig = m_pMasterRig;
	BayonetDesc.pSocketMatrix = m_pMasterRig->Get_SocketMatrix("RightHandAttachSocket");
	BayonetDesc.pParentMatrix = Get_RenderWorldMatrixPtr();
	BayonetDesc.eWeaponType = WEAPON_TYPE::BAYONET;
	BayonetDesc.pDissolveTime = &m_fTimeElapsed;
	CHECK_FAILED(__super::Add_PartObject(m_iLevel, Proto_GameObject(L"Player_Weapon_Bayonet"), TEXT("Part_Weapon_Bayonet"), &BayonetDesc), E_FAIL);

	//소드 무기
	PWeapon_Sword::WEAPONSWORD_DESC SwordDesc = {};
	SwordDesc.iSocketIndex = m_pMasterRig->Get_SocketIndex("RightHandAttachSocket");
	SwordDesc.pMasterRig = m_pMasterRig;
	SwordDesc.pSocketMatrix = m_pMasterRig->Get_SocketMatrix("RightHandAttachSocket");
	SwordDesc.pParentMatrix = Get_RenderWorldMatrixPtr();
	SwordDesc.eWeaponType = WEAPON_TYPE::SWORD;
	SwordDesc.pDissolveTime = &m_fTimeElapsed;
	CHECK_FAILED(__super::Add_PartObject(m_iLevel, Proto_GameObject(L"Player_Weapon_Sword"), TEXT("Part_Weapon_Sword"), &SwordDesc), E_FAIL);

	//그레이트소드 무기
	PWeapon_GreatSword::WEAPONGREATSWORD_DESC GreatSwordDesc = {};
	GreatSwordDesc.iSocketIndex = m_pMasterRig->Get_SocketIndex("RightHandAttachSocket");
	GreatSwordDesc.pMasterRig = m_pMasterRig;
	GreatSwordDesc.pSocketMatrix = m_pMasterRig->Get_SocketMatrix("RightHandAttachSocket");
	GreatSwordDesc.pParentMatrix = Get_RenderWorldMatrixPtr();
	GreatSwordDesc.eWeaponType = WEAPON_TYPE::GREATESWORD;
	GreatSwordDesc.pDissolveTime = &m_fTimeElapsed;
	CHECK_FAILED(__super::Add_PartObject(m_iLevel, Proto_GameObject(L"Player_Weapon_GreatSword"), TEXT("Part_Weapon_GreatSword"), &GreatSwordDesc), E_FAIL);

	//해머 무기
	PWeapon_Hammer::WEAPONHAMMER_DESC HammerDesc = {};
	HammerDesc.iSocketIndex = m_pMasterRig->Get_SocketIndex("RightHandAttachSocket");
	HammerDesc.pMasterRig = m_pMasterRig;
	HammerDesc.pSocketMatrix = m_pMasterRig->Get_SocketMatrix("RightHandAttachSocket");
	HammerDesc.pParentMatrix = Get_RenderWorldMatrixPtr();
	HammerDesc.eWeaponType = WEAPON_TYPE::HAMMER;
	HammerDesc.pDissolveTime = &m_fTimeElapsed;
	CHECK_FAILED(__super::Add_PartObject(m_iLevel, Proto_GameObject(L"Player_Weapon_Hammer"), TEXT("Part_Weapon_Hammer"), &HammerDesc), E_FAIL);

	//핼버드 무기
	PWeapon_Halberd::tagWeaponHalberdDesc HalberdDesc = {};
	HalberdDesc.iSocketIndex = m_pMasterRig->Get_SocketIndex("RightHandAttachSocket");
	HalberdDesc.pMasterRig = m_pMasterRig;
	HalberdDesc.pSocketMatrix = m_pMasterRig->Get_SocketMatrix("RightHandAttachSocket");
	HalberdDesc.pParentMatrix = Get_RenderWorldMatrixPtr();
	HalberdDesc.eWeaponType = WEAPON_TYPE::HALBERD;
	HalberdDesc.pDissolveTime = &m_fTimeElapsed;
	CHECK_FAILED(__super::Add_PartObject(m_iLevel, Proto_GameObject(L"Player_Weapon_Halberd"), TEXT("Part_Weapon_Halberd"), &HalberdDesc), E_FAIL);

	//주사기
	Player_Injection::INJECTION_DESC InjectionDesc = {};
	InjectionDesc.iSocketIndex = m_pMasterRig->Get_SocketIndex("LeftHandAttachSocket");
	InjectionDesc.pMasterRig = m_pMasterRig;
	InjectionDesc.pSocketMatrix = m_pMasterRig->Get_SocketMatrix("LeftHandAttachSocket");
	InjectionDesc.pParentMatrix = Get_RenderWorldMatrixPtr();
	CHECK_FAILED(__super::Add_PartObject(m_iLevel, Proto_GameObject(L"Player_Injection"), TEXT("Part_Injection"), &InjectionDesc), E_FAIL);


	//기본 무기
	m_pWeapons[_UINT(WEAPON_TYPE::BAYONET)] = dynamic_cast<Client::Player_Weapon*>(Find_PartObject(TEXT("Part_Weapon_Bayonet")));
	m_pWeapons[_UINT(WEAPON_TYPE::SWORD)] = dynamic_cast<Client::Player_Weapon*>(Find_PartObject(TEXT("Part_Weapon_Sword")));
	m_pWeapons[_UINT(WEAPON_TYPE::GREATESWORD)] = dynamic_cast<Client::Player_Weapon*>(Find_PartObject(TEXT("Part_Weapon_GreatSword")));
	m_pWeapons[_UINT(WEAPON_TYPE::HALBERD)] = dynamic_cast<Client::Player_Weapon*>(Find_PartObject(TEXT("Part_Weapon_Halberd")));
	m_pWeapons[_UINT(WEAPON_TYPE::HAMMER)] = dynamic_cast<Client::Player_Weapon*>(Find_PartObject(TEXT("Part_Weapon_Hammer")));

	for (_uint i = 0; i < _UINT(WEAPON_TYPE::WP_END); ++i)
	{
		if (m_pWeapons[i] == nullptr)
			return E_FAIL;

		m_pWeapons[i]->Set_PartActive(false);

		Safe_AddRef(m_pWeapons[i]);

	}
	m_pActiveWeapon = m_pWeapons[_UINT(WEAPON_TYPE::BAYONET)];
	m_pWeapons[_UINT(WEAPON_TYPE::BAYONET)]->Set_PartActive(true);
	//특수무기
	m_pActiveBloodWeapon = dynamic_cast<Client::Player_BloodWeapon*>(Find_PartObject(TEXT("Part_BloodWeapon")));
	Safe_AddRef(m_pActiveBloodWeapon);
	m_pActiveBloodWeapon->Set_PartActive(false);
	//아우터
	m_pActiveOuter = dynamic_cast<Client::Player_Outer*>(Find_PartObject(TEXT("Part_Outer")));
	Safe_AddRef(m_pActiveOuter);
	//인젝션(주사기)
	m_pActiveInjection = dynamic_cast<Client::Player_Injection*>(Find_PartObject(TEXT("Part_Injection")));
	Safe_AddRef(m_pActiveInjection);
	m_pActiveInjection->Set_PartActive(false);

	/* 마스터본 가져와서 Player에도 등록 */
	m_pMasterRigModel = m_pMasterRig->Get_Model();
	m_eCurrentWeaponType = WEAPON_TYPE::BAYONET;

	/* 무기 매트릭스 */
	m_pWeaponBoneMatrix = const_cast<_float4x4*>(m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr("TrailStartSocket"));		// 콜라이더용
	m_pWeaponBoneMatrixEnd = const_cast<_float4x4*>(m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr("TrailEndSocket"));		// 콜라이더용
	m_pBayonetMuzzleMatrix = const_cast<_float4x4*>(m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr("Sword_00_nouse"));		// 바요넷 총구


	return S_OK;
}

HRESULT Client::Player::Ready_States()
{
	//기본 행동
	CHECK_FAILED(m_pStateMachineCom->Add_State(IDLE, Player_Idle::Create(this, m_pMasterRig)), E_FAIL);
	CHECK_FAILED(m_pStateMachineCom->Add_State(RUN, Player_Run::Create(this, m_pMasterRig)), E_FAIL);
	CHECK_FAILED(m_pStateMachineCom->Add_State(WALK, Player_Walk::Create(this, m_pMasterRig)), E_FAIL); 
	CHECK_FAILED(m_pStateMachineCom->Add_State(DASH, Player_Dash::Create(this, m_pMasterRig)), E_FAIL);
	CHECK_FAILED(m_pStateMachineCom->Add_State(ROLL, Player_Roll::Create(this, m_pMasterRig)), E_FAIL);
	CHECK_FAILED(m_pStateMachineCom->Add_State(BACKSTEP, Player_BackStep::Create(this, m_pMasterRig)), E_FAIL);
	CHECK_FAILED(m_pStateMachineCom->Add_State(CHECKPOINT, Player_CheckPoint::Create(this, m_pMasterRig)), E_FAIL);
	CHECK_FAILED(m_pStateMachineCom->Add_State(PICKITEM, Player_PickItem::Create(this, m_pMasterRig)), E_FAIL);
	CHECK_FAILED(m_pStateMachineCom->Add_State(OPENBOX, Player_OpenBox::Create(this, m_pMasterRig)), E_FAIL);
	CHECK_FAILED(m_pStateMachineCom->Add_State(DEATH, Player_Death::Create(this, m_pMasterRig)), E_FAIL);
	CHECK_FAILED(m_pStateMachineCom->Add_State(LADDER, Player_Ladder::Create(this, m_pMasterRig)), E_FAIL);

	//무기 공격
	CHECK_FAILED(m_pStateMachineCom->Add_State(ATTACKNORMAL, Player_AttackNormal::Create(this, m_pMasterRig)), E_FAIL);
	CHECK_FAILED(m_pStateMachineCom->Add_State(ATTACKSTRONG, Player_AttackStrong::Create(this, m_pMasterRig)), E_FAIL);
	CHECK_FAILED(m_pStateMachineCom->Add_State(ATTACKSTRONGSTART, Player_AttackStrongStart::Create(this, m_pMasterRig)), E_FAIL);
	CHECK_FAILED(m_pStateMachineCom->Add_State(ATTACKSTRONGCHARGE, Player_AttackStrongCharge::Create(this, m_pMasterRig)), E_FAIL);

	CHECK_FAILED(m_pStateMachineCom->Add_State(ATTACKDODGEF, Player_AttackDodgeF::Create(this, m_pMasterRig)), E_FAIL);
	CHECK_FAILED(m_pStateMachineCom->Add_State(ATTACKDODGEB, Player_AttackDodgeB::Create(this, m_pMasterRig)), E_FAIL);
	CHECK_FAILED(m_pStateMachineCom->Add_State(SPECIALATTACK, Player_SpecialAttack::Create(this, m_pMasterRig)), E_FAIL);

	//가드 (상체 블렌드용) 상태머신도 상체 전용
	CHECK_FAILED(m_pUpperStateMachineCom->Add_State(GUARDLOOP, Player_GuardLoop::Create(this, m_pMasterRig)), E_FAIL);
	CHECK_FAILED(m_pUpperStateMachineCom->Add_State(GUARDSTART, Player_GuardStart::Create(this, m_pMasterRig)), E_FAIL);
	CHECK_FAILED(m_pUpperStateMachineCom->Add_State(GUARDEND, Player_GuardEnd::Create(this, m_pMasterRig)), E_FAIL);

	CHECK_FAILED(m_pStateMachineCom->Add_State(GUARDWALK, Player_GuardWalk::Create(this, m_pMasterRig)), E_FAIL);
	CHECK_FAILED(m_pStateMachineCom->Add_State(GUARDHIT, Player_GuardHit::Create(this, m_pMasterRig)), E_FAIL);
	CHECK_FAILED(m_pStateMachineCom->Add_State(GUARD_BREAK, Player_GuardBreak::Create(this, m_pMasterRig)), E_FAIL);

	//피격 전용 
	CHECK_FAILED(m_pStateMachineCom->Add_State(DAMAGE, Player_Damage::Create(this, m_pMasterRig)), E_FAIL);
	CHECK_FAILED(m_pStateMachineCom->Add_State(DAMAGEBLOW, Player_DamageBlow::Create(this, m_pMasterRig)), E_FAIL);
	CHECK_FAILED(m_pStateMachineCom->Add_State(DAMAGEEND, Player_DamageEnd::Create(this, m_pMasterRig)), E_FAIL);

	//아이템 사용 (상체 애니메이션) + 무기 교체도 (상체 애니메이션)
	CHECK_FAILED(m_pUpperStateMachineCom->Add_State(USEITEM, Player_UseItem::Create(this, m_pMasterRig)), E_FAIL);
	CHECK_FAILED(m_pUpperStateMachineCom->Add_State(WEAPONCHANGE, Player_WeaponChange::Create(this, m_pMasterRig)), E_FAIL);

	//블러드웨폰전용
	CHECK_FAILED(m_pStateMachineCom->Add_State(ATTACKPARRY, Player_AttackParry::Create(this, m_pMasterRig)), E_FAIL);
	CHECK_FAILED(m_pStateMachineCom->Add_State(SPECIALSUCK, Player_SpecialSuck::Create(this, m_pMasterRig)), E_FAIL);
	CHECK_FAILED(m_pStateMachineCom->Add_State(BACKSTAB, Player_BackStab::Create(this, m_pMasterRig)), E_FAIL);
	/***********************************************************잠깐 뒤잡 비활성화***********************************************************/

	//상체 더미
	CHECK_FAILED(m_pUpperStateMachineCom->Add_State(PLAYERUPPERSTATE_END, Player_UpperDummy::Create(this, m_pMasterRig)), E_FAIL);

	//스킬
	CHECK_FAILED(m_pStateMachineCom->Add_State(SKILL, Player_Skill::Create(this, m_pMasterRig)), E_FAIL);
	//커스텀
	CHECK_FAILED(m_pStateMachineCom->Add_State(CUSTOM, Player_Custom::Create(this, m_pMasterRig)), E_FAIL);


	m_pStateMachineCom->Set_State(IDLE);
	m_pUpperStateMachineCom->Set_State(PLAYERUPPERSTATE_END);

	return S_OK;
}

HRESULT Client::Player::Ready_Event()
{
	/* 애니메이션 상태 제어 이벤트 */
	m_iPlayerAnimEventHandle = GameObject::Subscribe_Event<PLAYERANIM_EVENT>(
		[this](const PLAYERANIM_EVENT& Event)
		{
			if (Event.iOwnerId != m_iObjectID)
				return;

			switch (Event.ePhase)
			{
			case ANIM_FRAMEPHASE::START:
				Apply_AnimEventCount(Event, +1);
				Decrease_Stamina(Event);
				Update_UseItem(Event);
				Create_Projectile(Event);
				Update_ChangeWeapon(Event);
				Update_AnimationSpeed(Event);
				Update_SkillEvent(Event);
				if (Event.bBlockRegenStamina)
					m_tPlayerRunTimeEvent.bBlockRegenStamina = true;
				if (Event.bLockOnHomingRotation)
					m_tPlayerRunTimeEvent.fHomingRotationSpeedRatio = Event.fHomingRotationSpeedRatio;
				break;
			case ANIM_FRAMEPHASE::UPDATE:

				break;
			case ANIM_FRAMEPHASE::END:
				Apply_AnimEventCount(Event, -1);
				if (Event.bBlockRegenStamina)
					m_tPlayerRunTimeEvent.bBlockRegenStamina = false;
				m_eAnimPhase = Event.ePhase;
				if (Event.bLockOnHomingRotation)
					m_tPlayerRunTimeEvent.fHomingRotationSpeedRatio = 0.f;
				break;
			}

		});

	// ParticleSystem 구독
	Subscribe_Event<PARTICLE_EVENT>([this](const PARTICLE_EVENT& e) {
		if (e.iOwnerId != m_iObjectID)
			return E_FAIL;

		// 이벤트 매니저에서 ParticleSystem 찾기
		ParticleSystem* pSystem = CAST(ParticleSystem*)(POOLING->Acquire_Effect(e.ePoolId));
		if (pSystem == nullptr)
			return E_FAIL;

		// 뼈 행렬 계산 람다
		auto fnCalcBoneMatrix = [this, socketName = e.SocketName, endSocketName = e.EndSocketName, bAttached = e.bAttached, bOnlyPosition = e.bOnlyPosition]() -> _float4x4
			{
				if (endSocketName.empty())
				{
					// 뼈의 위치 가져오기
					const _float4x4* pMatBone = { nullptr };
					_float4x4 matFinalPos = {};
					XMStoreFloat4x4(&matFinalPos, XMMatrixIdentity());
					_matrix matPlayerWorld = m_pTransformCom->Get_WorldMatrix();

					if (bAttached)
					{
						// 무기 뼈
						pMatBone = m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr(socketName);
						if (pMatBone != nullptr)
						{
							_float3 vBoneWorldPos = _float3(pMatBone->_41, pMatBone->_42, pMatBone->_43);

							const _float4x4* pSocket = m_pMasterRig->Get_SocketMatrix("RightHandAttachSocket");
							_matrix matRotation = XMMatrixRotationX(XMConvertToRadians(90.f)) * XMLoadFloat4x4(pSocket) * matPlayerWorld;

							// 스케일 제거 => TransformCoord 하면서 Range 값이 너무 작아짐 방지
							matRotation.r[0] = XMVector3Normalize(matRotation.r[0]);
							matRotation.r[1] = XMVector3Normalize(matRotation.r[1]);
							matRotation.r[2] = XMVector3Normalize(matRotation.r[2]);
							matRotation.r[3] = XMVectorSet(vBoneWorldPos.x, vBoneWorldPos.y, vBoneWorldPos.z, 1.f);

							if (!bOnlyPosition)
								XMStoreFloat4x4(&matFinalPos, matRotation);
							else
								XMStoreFloat4x4(&matFinalPos, XMMatrixTranslation(vBoneWorldPos.x, vBoneWorldPos.y, vBoneWorldPos.z));
						}
					}
					else
					{
						// 플레이어 뼈
						pMatBone = m_pMasterModel->Get_SocketBoneMatrixPtr_Index(socketName);
						if (pMatBone != nullptr)
						{
							_matrix matBone = XMLoadFloat4x4(pMatBone);
							_matrix matWorld = matBone * matPlayerWorld;

							// 스케일 제거 => TransformCoord 하면서 Range 값이 너무 작아짐 방지
							matWorld.r[0] = XMVector3Normalize(matWorld.r[0]);
							matWorld.r[1] = XMVector3Normalize(matWorld.r[1]);
							matWorld.r[2] = XMVector3Normalize(matWorld.r[2]);

							if (!bOnlyPosition)
								XMStoreFloat4x4(&matFinalPos, matWorld);
							else
							{
								_float3 vPos = {};
								vPos.x = XMVectorGetX(matWorld.r[3]);
								vPos.y = XMVectorGetY(matWorld.r[3]);
								vPos.z = XMVectorGetZ(matWorld.r[3]);
								XMStoreFloat4x4(&matFinalPos, XMMatrixTranslation(vPos.x, vPos.y, vPos.z));
							}
						}
					}

					return matFinalPos;
				}
				else
				{
					// 2개 소켓 사용하는 로직
					const _float4x4* pMatStart = m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr(socketName);
					const _float4x4* pMatEnd = m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr(endSocketName);
					if (pMatStart == nullptr || pMatEnd == nullptr)
						return _float4x4{};

					// 소켓 행렬에서 위치 가져오기
					_vector vStart = XMVectorSet(pMatStart->_41, pMatStart->_42, pMatStart->_43, 1.f);
					_vector vEnd = XMVectorSet(pMatEnd->_41, pMatEnd->_42, pMatEnd->_43, 1.f);

					// 이펙트 위치를 일단 중점으로
					_vector vMid = (vStart + vEnd) * 0.5f;

					// Start => End 방향 + 거리
					_vector vDir = vEnd - vStart;
					_float fDist = XMVectorGetX(XMVector3Length(vDir));
					_vector vUp = XMVector3Normalize(vDir);

					// Right, Look 구하기
					_vector vWorldForward = XMVectorSet(0.f, 0.f, 1.f, 0.f);
					if (fabsf(XMVectorGetX(XMVector3Dot(vUp, vWorldForward))) > 0.99f)
						vWorldForward = XMVectorSet(1.f, 0.f, 0.f, 0.f);

					_vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vWorldForward));
					_vector vLook = XMVector3Cross(vRight, vUp);

					// 행렬 만들기 => Y축 길이가 fDist
					_matrix matResult = XMMatrixIdentity();
					matResult.r[0] = vRight;						// Right
					matResult.r[1] = vUp * fDist;					// Up = 방향 * 거리
					matResult.r[2] = vLook;							// Look
					matResult.r[3] = XMVectorSetW(vMid, 1.f);		// 위치는 중점

					_float4x4 matFinal;
					XMStoreFloat4x4(&matFinal, matResult);
					return matFinal;
				}
			};

		// 초기 위치 설정
		_float4x4 matInitial = fnCalcBoneMatrix();
		pSystem->Set_WorldMatrix(matInitial);

		pSystem->Play();

		// Follow 면 콜백에 등록
		if (e.bFollow)
		{
			pSystem->Set_FollowCallback(fnCalcBoneMatrix);
			pSystem->Set_FollowParent(e.bParticleFollow);
		}

		return S_OK;
		});


	// SwordTrail 구독
	Subscribe_Event<TRAIL_EVENT>([this](const TRAIL_EVENT& e) {
		if (e.iOwnerId != m_iObjectID)
			return E_FAIL;

		if (e.ePhase == ANIM_FRAMEPHASE::START)
		{
			// 이펙트 매니저에서 ParticleSystem 찾기
			ParticleSystem* pSystem = CAST(ParticleSystem*)(PoolingManager::Get_Instance()->Acquire(e.ePoolId, nullptr));

			if (pSystem != nullptr)
			{	
				m_vecTrailEffects.clear();
				auto vecEffects = pSystem->Get_Effects();
				for (auto& entry : vecEffects)
				{
					TrailEffect* pTrail = DCAST(TrailEffect*)(entry.pEffect);
					if (pTrail != nullptr)
						m_vecTrailEffects.push_back(pTrail);
				}
			}

			m_bTrailActive = true;
		}
		else if (e.ePhase == ANIM_FRAMEPHASE::END)
		{
			m_bTrailActive = false;
			m_vecTrailEffects.clear();
		}

		return S_OK;
		});

	// Dissolve 이벤트 구독
	Subscribe_Event<DISSOLVE_EVENT>([this](const DISSOLVE_EVENT& e) {
		if (e.iOwnerId != m_iObjectID)
			return;

		if (e.bIsWeaponTarget)
			return;

		m_bDissolving = true;

		if (e.bDissolve)
		{
			// 사라지기
			m_fDissolveDir = 1.f;
			m_fTimeElapsed = 0.f;
		}
		else
		{
			// 나타나기
			m_fDissolveDir = -1.f;
			m_fTimeElapsed = m_fDissolveMax;
		}
		});

	//UIOpenEvent
	Subscribe_Event<INPUT_LOCK_EVENT>([this](const INPUT_LOCK_EVENT& e) {
		m_bOpenUI = e.bLock;

		});

	Subscribe_Event<PlayerCheckPointEvnet>([this](const PlayerCheckPointEvnet& e) {

		m_eSavePointLevel = static_cast<LEVEL>(e.iLevel);
		m_iSavePointIndex = e.iIdx;

		State* pCurState = m_pStateMachineCom->Get_CurrentState();
		Player_CheckPoint* pCheckPointState = dynamic_cast<Player_CheckPoint*>(pCurState);

		if (e.eCheckPointType == CheckPointEventType::START) //스타트는 무조건 처음 일거고
		{
			State* pFindState = m_pStateMachineCom->Find_State(CHECKPOINT);
			Player_CheckPoint* pFindCheckPointState = dynamic_cast<Player_CheckPoint*>(pFindState);
			if (pFindCheckPointState)
				pFindCheckPointState->Set_CheckPointType(Player_CheckPoint::START);
			m_pStateMachineCom->Change_State(CHECKPOINT);
		}
		else if (e.eCheckPointType == CheckPointEventType::END) //End의 경우에는 이미 상태가 CheckPoint인경우 + 죽었을때의 경우가 있으므로
		{

			if (pCheckPointState)				
			{
				pCheckPointState->Set_CheckPointType(Player_CheckPoint::END);
				pCheckPointState->Enter_State(); 
			}
			else
			{
				State* pFindState = m_pStateMachineCom->Find_State(CHECKPOINT);
				Player_CheckPoint* pFindCheckPointState = dynamic_cast<Player_CheckPoint*>(pFindState);
				if (pFindCheckPointState)
					pFindCheckPointState->Set_CheckPointType(Player_CheckPoint::END);
				m_pStateMachineCom->Change_State(CHECKPOINT);
			}
		}

		});


	///인벤스킬정보얻어오는 
	Subscribe_Event<tagPlayerSkillInfo>([this](const tagPlayerSkillInfo& e) {


		//wstring debugMessage = L"클릭한 슬롯 인덱스: " + to_wstring(e.idx) +
		//	L"\n아이템 ID: " + to_wstring(e.m_ItemInfo->ItemID);

		//// MessageBox(윈도우핸들, 내용, 제목, 버튼타입)
		//MessageBox(g_hWnd, debugMessage.c_str(), L"아이템 장착 디버깅", MB_OK | MB_ICONINFORMATION);

		SkillManager::GetInstance()->Equip_SkillToSlot(e.idx, e.m_ItemInfo->ItemID);

		});

	Subscribe_Event<PlayerIntercation>([this](const PlayerIntercation& e) {
		if (e.bEKeyDown)
		{
			switch (e.eInteractionType)
			{
			case INTERACTION_TYPE::ITEM:
				m_pStateMachineCom->Change_State(PICKITEM);
				break;
			case INTERACTION_TYPE::ITEM_BOX:
				m_pStateMachineCom->Change_State(OPENBOX);
				break;
			case INTERACTION_TYPE::LADDER:
				Enter_LadderState(e);
				break;
			default:
				break;
			}
		}

		});

	Subscribe_Event<CINEMATIC_CHARACTER_CONTROLL>([this](const CINEMATIC_CHARACTER_CONTROLL& _event) {

		switch (_event.iNumber)
		{
		case CP_OLIVER_TELEPORT:
			Teleport_Oliver();
			break;
		case CP_WOLF_TELEPORT:
			Teleport_Wolf();
			break;
		}

		});


	return S_OK;
}

HRESULT Client::Player::Ready_PhysXEvent()
{
	m_pMasterRigModel->Set_OwnerId(m_iObjectID);

	Subscribe_Event<DAMAGE_EVENT>([this](DAMAGE_EVENT _event) {
		if (_event.iTargetID != m_iObjectID)
			return;

		m_vEnemyHitDirection = XMVectorSet(_event.vHitDirection.x, _event.vHitDirection.y, _event.vHitDirection.z, 0.f);
		m_eEnemyDamagePower = _event.eDamagePower;


		OnDamaged(_event);
	});

	m_iColliderEventHandle = GameObject::Subscribe_Event<COLLIDER_EVENT>(
		[this](const COLLIDER_EVENT& _event)
		{
			if (_event.iOwnerId != m_iObjectID)
				return;

			switch (_event.ePhase)
			{
			case ANIM_FRAMEPHASE::START:
				m_bAttackSweepActive = true;
				m_fAttackRadius = _event.fAttackRadius;
				m_fAttackDamage = _event.fAttackDamage ;
				m_fAttackHalfHeight = _event.fAttackHalfHeight;
				m_setHitTargets.clear();
				m_vPrevWeaponPos = m_vCurrentWeaponPos;
				break;

			case ANIM_FRAMEPHASE::UPDATE:
				break;

			case ANIM_FRAMEPHASE::END:
				m_bAttackSweepActive = false;
				m_setHitTargets.clear();
				break;
			}
		});

	return S_OK;
}
HRESULT Client::Player::Ready_Skills()
{
	Skill_Base::SKILL_DESC KangRyongDesc;
	//Create_Skill_Prototype(L"블러드 샷", "KetsugiIcon/T_Ketsugi004", 1, 1, L"피의 힘을 거대한 탄환으로 바꿔 날린다.", L"양", L"액티브", 2, 2, L"관통");

	KangRyongDesc.bIsLoop = false;
	KangRyongDesc.eDamagePower = DAMAGEPOWER::SPECIALPOWER;
	KangRyongDesc.fCooldown = 15.f;
	KangRyongDesc.fMyeonghyeolCost = 6.f;
	KangRyongDesc.iAnimIndex = 235;
	KangRyongDesc.iItemCount = 1;
	KangRyongDesc.iMaxItemSize = 1;
	KangRyongDesc.iSkillID = 4000;
	KangRyongDesc.strTexKey = "KetsugiIcon/T_ActionKetsugi006";
	KangRyongDesc.vecWeaponType = { WEAPON_TYPE::GREATESWORD, WEAPON_TYPE::HAMMER };
	KangRyongDesc.wstrAttackType = L"-";
	KangRyongDesc.wstrDescription = L"전방으로 단숨에 뛰어들어 강력한 참격을 꽂아넣는다 양손검, 대형 해머용 공격 연혈";
	KangRyongDesc.wstrLineageType = L"기술";
	KangRyongDesc.wstrSkillName = L"강룡복호";
	KangRyongDesc.wstrSkillType = L"액티브";
	KangRyongDesc.fSkillDuration = 0.f;

	Skill_KangRyong* pKangRyong = Skill_KangRyong::Create(this, m_pMasterRig, &KangRyongDesc);
	CHECK_FAILED(SkillManager::GetInstance()->Register_Skill(KangRyongDesc.iSkillID, pKangRyong),E_FAIL);
	/*ItemInfo* pKangRyongSkillItemInfo = ItemManager::GetInstance()->Get_ItemInfo(L"강룡복호");
	InventoryManager::GetInstance()->Add_Item(pKangRyongSkillItemInfo,false);
	Safe_Delete(pKangRyongSkillItemInfo);*/


	Skill_Base::SKILL_DESC ShadowAssaultDesc;
	ShadowAssaultDesc.bIsLoop = false;
	ShadowAssaultDesc.eDamagePower = DAMAGEPOWER::SPECIALPOWER;
	ShadowAssaultDesc.fCooldown = 6.f;
	ShadowAssaultDesc.fMyeonghyeolCost = 4.f;
	ShadowAssaultDesc.iAnimIndex = 234;
	ShadowAssaultDesc.iItemCount = 1;
	ShadowAssaultDesc.iMaxItemSize = 1;
	ShadowAssaultDesc.iSkillID = 4001;
	ShadowAssaultDesc.strTexKey = "KetsugiIcon/T_ActionKetsugi012";
	ShadowAssaultDesc.vecWeaponType = { WEAPON_TYPE::SWORD, WEAPON_TYPE::HALBERD,WEAPON_TYPE::BAYONET};
	ShadowAssaultDesc.wstrAttackType = L"-";
	ShadowAssaultDesc.wstrDescription = L"단거리를 고속 돌진하는 이동 공격 한손검,도끼창,총검용 공격 연혈";
	ShadowAssaultDesc.wstrLineageType = L"기술";
	ShadowAssaultDesc.wstrSkillName = L"섀도우 어설트";
	ShadowAssaultDesc.wstrSkillType = L"액티브";
	ShadowAssaultDesc.fSkillDuration = 0.f;

	Skill_ShadowAssault* pShadowAssault = Skill_ShadowAssault::Create(this, m_pMasterRig, &ShadowAssaultDesc);
	CHECK_FAILED(SkillManager::GetInstance()->Register_Skill(ShadowAssaultDesc.iSkillID, pShadowAssault), E_FAIL);
	//ItemInfo* pShadowAssaultItemInfo = ItemManager::GetInstance()->Get_ItemInfo(L"섀도우 어설트");
	//InventoryManager::GetInstance()->Add_Item(pShadowAssaultItemInfo, false);
	//Safe_Delete(pShadowAssaultItemInfo);

	Skill_Base::SKILL_DESC Yeomgeomjeonmu;
	Yeomgeomjeonmu.bIsLoop = false;
	Yeomgeomjeonmu.eDamagePower = DAMAGEPOWER::NORMALPOWER;
	Yeomgeomjeonmu.fCooldown = 10.f;
	Yeomgeomjeonmu.fMyeonghyeolCost = 5.f;
	Yeomgeomjeonmu.iAnimIndex = 240;
	Yeomgeomjeonmu.iItemCount = 1;
	Yeomgeomjeonmu.iMaxItemSize = 1;
	Yeomgeomjeonmu.iSkillID = 4002;
	Yeomgeomjeonmu.strTexKey = "KetsugiIcon/T_Ketsugi169";
	Yeomgeomjeonmu.vecWeaponType = {};
	Yeomgeomjeonmu.wstrAttackType = L"절단";
	Yeomgeomjeonmu.wstrDescription = L"자기 주변에 회전하는 화염검을 소환한다";
	Yeomgeomjeonmu.wstrLineageType = L"음";
	Yeomgeomjeonmu.wstrSkillName = L"염검전무";
	Yeomgeomjeonmu.wstrSkillType = L"액티브";
	Yeomgeomjeonmu.fSkillDuration = 7.f;
	Yeomgeomjeonmu.eSkillType = Skill_Base::SKILL_TYPE::DURATION_SKILL;

	Skill_Yeomgeom* pYeomgeomjeonmu = Skill_Yeomgeom::Create(this, m_pMasterRig, &Yeomgeomjeonmu);
	CHECK_FAILED(SkillManager::GetInstance()->Register_Skill(Yeomgeomjeonmu.iSkillID, pYeomgeomjeonmu), E_FAIL);
	/*ItemInfo* ppYeomgeomjeonmuItemInfo = ItemManager::GetInstance()->Get_ItemInfo(L"염검전무");
	InventoryManager::GetInstance()->Add_Item(ppYeomgeomjeonmuItemInfo, false);
	Safe_Delete(ppYeomgeomjeonmuItemInfo);*/

	Skill_Base::SKILL_DESC StrikeRiser;
	StrikeRiser.bIsLoop = false;
	StrikeRiser.eDamagePower = DAMAGEPOWER::POWER_END;
	StrikeRiser.fCooldown = 5.f;
	StrikeRiser.fMyeonghyeolCost = 5.f;
	StrikeRiser.iAnimIndex = 238;
	StrikeRiser.iItemCount = 1;
	StrikeRiser.iMaxItemSize = 1;
	StrikeRiser.iSkillID = 4003;
	StrikeRiser.strTexKey = "KetsugiIcon/T_Ketsugi002";
	StrikeRiser.vecWeaponType = {};
	StrikeRiser.wstrAttackType = L"-";
	StrikeRiser.wstrDescription = L"일정 시간, 공격력이 증가한다";
	StrikeRiser.wstrLineageType = L"양";
	StrikeRiser.wstrSkillName = L"스트라이크 라이저";
	StrikeRiser.wstrSkillType = L"액티브";
	StrikeRiser.fSkillDuration = 300.f; 
	StrikeRiser.eSkillType = Skill_Base::SKILL_TYPE::DURATION_SKILL;

	Skill_StrikeRiser* pStrikeRiser = Skill_StrikeRiser::Create(this, m_pMasterRig, &StrikeRiser);
	CHECK_FAILED(SkillManager::GetInstance()->Register_Skill(StrikeRiser.iSkillID, pStrikeRiser), E_FAIL);
	/*ItemInfo* pStrikeRiserItemInfo = ItemManager::GetInstance()->Get_ItemInfo(L"스트라이크 라이저");
	InventoryManager::GetInstance()->Add_Item(pStrikeRiserItemInfo, false);
	Safe_Delete(pStrikeRiserItemInfo);*/

	Skill_Base::SKILL_DESC RoyalHeart;
	RoyalHeart.bIsLoop = false;
	RoyalHeart.eDamagePower = DAMAGEPOWER::POWER_END;
	RoyalHeart.fCooldown = 5.f;
	RoyalHeart.fMyeonghyeolCost = 5.f;
	RoyalHeart.iAnimIndex = 237;
	RoyalHeart.iItemCount = 1;
	RoyalHeart.iMaxItemSize = 1;
	RoyalHeart.iSkillID = 4004;
	RoyalHeart.strTexKey = "KetsugiIcon/T_Ketsugi010";
	RoyalHeart.vecWeaponType = {};
	RoyalHeart.wstrAttackType = L"-";
	RoyalHeart.wstrDescription = L"일정 시간, 쉽게 밀리지 않는다";
	RoyalHeart.wstrLineageType = L"양";
	RoyalHeart.wstrSkillName = L"로열 하트";
	RoyalHeart.wstrSkillType = L"액티브";
	RoyalHeart.fSkillDuration = 90.f;
	RoyalHeart.eSkillType = Skill_Base::SKILL_TYPE::DURATION_SKILL;

	Skill_RoyalHeart* pRoyalHeart = Skill_RoyalHeart::Create(this, m_pMasterRig, &RoyalHeart);
	CHECK_FAILED(SkillManager::GetInstance()->Register_Skill(RoyalHeart.iSkillID, pRoyalHeart), E_FAIL);
	/*ItemInfo* pRoyalHeartItemInfo = ItemManager::GetInstance()->Get_ItemInfo(L"로열 하트");
	InventoryManager::GetInstance()->Add_Item(pRoyalHeartItemInfo, false);
	Safe_Delete(pRoyalHeartItemInfo);*/

	Skill_Base::SKILL_DESC RadiantBarrel;
	RadiantBarrel.bIsLoop = false;
	RadiantBarrel.eDamagePower = DAMAGEPOWER::SPECIALPOWER;
	RadiantBarrel.fCooldown = 15.f;
	RadiantBarrel.fMyeonghyeolCost = 10.f;
	RadiantBarrel.iAnimIndex = 241;
	RadiantBarrel.iItemCount = 1;
	RadiantBarrel.iMaxItemSize = 1;
	RadiantBarrel.iSkillID = 4005;
	RadiantBarrel.strTexKey = "KetsugiIcon/T_ActionKetsugi017";
	RadiantBarrel.vecWeaponType = { WEAPON_TYPE::BAYONET };
	RadiantBarrel.wstrAttackType = L"-";
	RadiantBarrel.wstrDescription = L"관통 성능을 가진 강력한 탄환을 사출하는 총검용 공격 연혈";
	RadiantBarrel.wstrLineageType = L"기술";
	RadiantBarrel.wstrSkillName = L"레이디언트 배럴";
	RadiantBarrel.wstrSkillType = L"액티브";
	Skill_RadiantBarrel* pRadiantBarrel = Skill_RadiantBarrel::Create(this, m_pMasterRig, &RadiantBarrel);
	CHECK_FAILED(SkillManager::GetInstance()->Register_Skill(RadiantBarrel.iSkillID, pRadiantBarrel), E_FAIL);
	//ItemInfo* pRadiantBarrelItemInfo = ItemManager::GetInstance()->Get_ItemInfo(L"레이디언트 배럴");
	//InventoryManager::GetInstance()->Add_Item(pRadiantBarrelItemInfo, false);
	//Safe_Delete(pRadiantBarrelItemInfo);

	Skill_Base::SKILL_DESC IndraCoilDesc;
	IndraCoilDesc.bIsLoop = false;
	IndraCoilDesc.eDamagePower = DAMAGEPOWER::SPECIALPOWER;
	IndraCoilDesc.fCooldown = 10.f;
	IndraCoilDesc.fMyeonghyeolCost = 7.f;
	IndraCoilDesc.iAnimIndex = 236;
	IndraCoilDesc.iItemCount = 1;
	IndraCoilDesc.iMaxItemSize = 1;
	IndraCoilDesc.iSkillID = 4006;
	IndraCoilDesc.strTexKey = "KetsugiIcon/T_Ketsugi049";
	IndraCoilDesc.vecWeaponType = {};
	IndraCoilDesc.wstrAttackType = L"관통";
	IndraCoilDesc.wstrDescription = L"겨냥한 적의 발 밑에, 3연속으로 강력한 전기 기둥을 발사한다";
	IndraCoilDesc.wstrLineageType = L"음";
	IndraCoilDesc.wstrSkillName = L"인드라 코일";
	IndraCoilDesc.wstrSkillType = L"액티브";
	Skill_IndraCoil* pIndraCoil = Skill_IndraCoil::Create(this, m_pMasterRig, &IndraCoilDesc);
	CHECK_FAILED(SkillManager::GetInstance()->Register_Skill(IndraCoilDesc.iSkillID, pIndraCoil), E_FAIL);
	/*ItemInfo* pIndraCoilItemInfo = ItemManager::GetInstance()->Get_ItemInfo(L"인드라 코일");
	InventoryManager::GetInstance()->Add_Item(pIndraCoilItemInfo, false);
	Safe_Delete(pIndraCoilItemInfo);*/

	Skill_Base::SKILL_DESC EnchantDesc;
	EnchantDesc.bIsLoop = false;
	EnchantDesc.eDamagePower = DAMAGEPOWER::POWER_END;
	EnchantDesc.fCooldown = 5.f;
	EnchantDesc.fMyeonghyeolCost = 5.f;
	EnchantDesc.iAnimIndex = 239;
	EnchantDesc.iItemCount = 1;
	EnchantDesc.iMaxItemSize = 1;
	EnchantDesc.iSkillID = 4007;
	EnchantDesc.strTexKey = "KetsugiIcon/T_Ketsugi014";
	EnchantDesc.vecWeaponType = {};
	EnchantDesc.wstrAttackType = L"-";
	EnchantDesc.wstrDescription = L"일정 시간, 자신과 동행자의 무기에 화염을 휘감아 상대의 몸속부터 태워버린다";
	EnchantDesc.wstrLineageType = L"양";
	EnchantDesc.wstrSkillName = L"플레임 웨폰";
	EnchantDesc.wstrSkillType = L"액티브";
	EnchantDesc.fSkillDuration = 30.f;
	EnchantDesc.eSkillType = Skill_Base::SKILL_TYPE::DURATION_SKILL;

	Skill_Enchant* pEnchant = Skill_Enchant::Create(this, m_pMasterRig, &EnchantDesc);
	CHECK_FAILED(SkillManager::GetInstance()->Register_Skill(EnchantDesc.iSkillID, pEnchant), E_FAIL);
	//ItemInfo* pEnchantItemInfo = ItemManager::GetInstance()->Get_ItemInfo(L"플레임 웨폰");
	//InventoryManager::GetInstance()->Add_Item(pEnchantItemInfo, false);
	//Safe_Delete(pEnchantItemInfo);

	return S_OK;	
}
/******************************************************* 컨테이너 추가 함수 *******************************************************/



//////////////////////////////////////////////////////// 플레이어에서 한번에 던지기 위해서 모으는 함수 ////////////////////////////////////////////////////////
void Client::Player::Update_PlayerCustomDesc()
{
	if (m_umapPartObjects.size() >= 4.f && !m_bOnetime) //임시로 1번만 받게
	{
		m_tPlayerShaderDesc.m_tBodyDesc = static_cast<Player_Body*>(m_umapPartObjects[TEXT("Part_Body")])->Get_PlayerBodyShaderDesc();
		m_tPlayerShaderDesc.m_tHairDesc = static_cast<Player_Hair*>(m_umapPartObjects[TEXT("Part_Hair")])->Get_PlayerHairShaderDesc();
		m_tPlayerShaderDesc.m_tBrowDesc = static_cast<Player_Head*>(m_umapPartObjects[TEXT("Part_Head")])->Get_PlayerBrowShaderDesc();
		m_tPlayerShaderDesc.m_tEyeDesc = static_cast<Player_Head*>(m_umapPartObjects[TEXT("Part_Head")])->Get_PlayerEyeShaderDesc();


		m_tPlayerMeshTex.pPlayerHairNum = static_cast<Player_Hair*>(m_umapPartObjects[TEXT("Part_Hair")])->Get_HairMeshNumPrt();
		m_tPlayerMeshTex.tHeadTexture = static_cast<Player_Head*>(m_umapPartObjects[TEXT("Part_Head")])->Get_HeadTexturePtr();
		m_tPlayerMeshTex.pPlayerBodyNum = static_cast<Player_Body*>(m_umapPartObjects[TEXT("Part_Body")])->Get_BodyMeshNumPtr();
		m_vInnerColor = static_cast<Player_Body*>(m_umapPartObjects[TEXT("Part_Body")])->Get_BodyColorSetUp();
		//메쉬, 헤어 바뀔거 정보가져오기
		m_bOnetime = true;
	}
	//if (m_iPrevBodyNum != *m_tPlayerMeshTex.pPlayerBodyNum) //옷의 번호가 이전꺼랑 다르면 색상변경 시켜주기
	//{
	//	memcpy(m_tPlayerShaderDesc.m_tBodyDesc.vInnerColor, m_vInnerColor[*m_tPlayerMeshTex.pPlayerBodyNum].data(), sizeof(_float4) * 6);
	//}
	m_iPrevBodyNum = *m_tPlayerMeshTex.pPlayerBodyNum;
}
/******************************************************* 플레이어에서 한번에 던지기 위해서 모으는 함수 *******************************************************/



//////////////////////////////////////////////////////// 바인딩 함수 ////////////////////////////////////////////////////////
HRESULT Client::Player::Bind_ShaderResources()
{
	return S_OK;
}
/******************************************************* 바인딩 함수 *******************************************************/



//////////////////////////////////////////////////////// PhysX 이동, 전투 함수 ////////////////////////////////////////////////////////
Model* Client::Player::Get_MasterRigModel()
{
	return m_pMasterRigModel ? m_pMasterRigModel : nullptr;
}

void Client::Player::Update_WeaponPosition()
{
	// Readback에서 무기 본 Combined 행렬을 가져와서 월드 좌표 계산
	if (m_pMasterRigModel == nullptr)
		return;

	if (m_pWeaponBoneMatrix == nullptr)
		return;

	m_vPrevWeaponPos = m_vCurrentWeaponPos;
	m_vPrevWeaponPosEnd = m_vCurrentWeaponPosEnd;

	// Combined 행렬(모델 공간)에 월드 행렬을 곱해서 실제 월드 위치 계산
	//_matrix matWeaponWorld = XMLoadFloat4x4(m_pWeaponBoneMatrix);
	//	//* XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());

	//XMStoreFloat3(&m_vCurrentWeaponPos, matWeaponWorld.r[3]);

	// 월드위치 계산돼있으니까 그냥 대입 그래도 위에 주석처리된거 지우지마
	m_vCurrentWeaponPos.x = m_pWeaponBoneMatrix->_41;
	m_vCurrentWeaponPos.y = m_pWeaponBoneMatrix->_42;
	m_vCurrentWeaponPos.z = m_pWeaponBoneMatrix->_43;

	m_vCurrentWeaponPosEnd.x = m_pWeaponBoneMatrixEnd->_41;
	m_vCurrentWeaponPosEnd.y = m_pWeaponBoneMatrixEnd->_42;
	m_vCurrentWeaponPosEnd.z = m_pWeaponBoneMatrixEnd->_43;

}

void Client::Player::Process_AttackSweep()
{
	// 궤적 Sweep 공격 판정
	if (!m_bAttackSweepActive)
		return;

	_float3 vSweepDir = {
		m_vCurrentWeaponPos.x - m_vPrevWeaponPos.x,
		m_vCurrentWeaponPos.y - m_vPrevWeaponPos.y,
		m_vCurrentWeaponPos.z - m_vPrevWeaponPos.z
	};

	_float fSweepDist = sqrtf(
		vSweepDir.x * vSweepDir.x +
		vSweepDir.y * vSweepDir.y +
		vSweepDir.z * vSweepDir.z);

	PxTransform pose(PxVec3(
		m_vPrevWeaponPos.x,
		m_vPrevWeaponPos.y,
		m_vPrevWeaponPos.z));

	// 거리에 따라 Overlap 또는 Sweep 선택
	vector<PHYSX_USER_DATA*> vecHitData;
	vector<_float3> vecHitPositions; // 접촉 위치 저장용

	if (fSweepDist < 0.01f)
	{
		// 무기가 거의 안 움직였으면 Overlap
		pose.p = PxVec3(m_vCurrentWeaponPos.x, m_vCurrentWeaponPos.y, m_vCurrentWeaponPos.z);
		PHYSX_OVERLAP_RESULT overlapResult = m_pGameInstance->PhysX_Overlap_Sphere(m_fAttackRadius, pose);

		if (overlapResult.bHit)
		{
			vecHitData = overlapResult.vecUserData;
			vecHitPositions = overlapResult.vecHitData;
		}
	}
	else
	{
		// 궤적 Sweep
		PxVec3 dir(vSweepDir.x, vSweepDir.y, vSweepDir.z);
		PHYSX_MULTI_SWEEP_RESULT sweepResult = m_pGameInstance->PhysX_Sweep_Sphere_Multi(m_fAttackRadius, pose, dir, fSweepDist);

		if (sweepResult.bHit)
		{
			vecHitData = sweepResult.vecUserData;
			vecHitPositions = sweepResult.vecPositions; // sweep 결과의 실제 접촉 위치 사용
		}
	}

	// 히트 처리 (공통)
	for (_uint i = 0; i < vecHitData.size(); ++i)
	{
		PHYSX_USER_DATA* pUserData = vecHitData[i];

		if (pUserData == nullptr)
			continue;
		if (pUserData->iObjectID == m_iObjectID)
			continue;
		if (pUserData->eActorType != PX_ACTOR_TYPE::MONSTER)
			continue;
		if (m_setHitTargets.count(pUserData->iObjectID) > 0)
			continue;
		m_setHitTargets.insert(pUserData->iObjectID);

		// 데미지 이벤트 발행
		_float3 vAttackDir = {};
		_vector vLook = m_pTransformCom->Get_State(DIRECTION::LOOK);
		XMStoreFloat3(&vAttackDir, XMVector3Normalize(vLook));

		DAMAGE_EVENT damageEvent = {};
		damageEvent.iAttackerID = m_iObjectID;
		damageEvent.iTargetID = pUserData->iObjectID;
		damageEvent.fDamage = m_fAttackDamage * m_pPlayerStatCom->Get_DamageMultiplier() + m_pPlayerStatCom->Apply_EnchantDamage() + m_fAdditionalDamage;
		damageEvent.vHitPosition = vecHitPositions[i]; // sweep/overlap 결과의 실제 접촉 위치 사용
		damageEvent.vHitDirection = vAttackDir;
		damageEvent.fKnockbackForce = m_fKnockbackForce;
		damageEvent.eDamagePower = m_pStateMachineCom->Get_CurrentState_DamagePower();
		damageEvent.pDamageOwner = this;

		m_pGameInstance->Publish<DAMAGE_EVENT>(damageEvent);

		m_bIsDealingDamage = true;
		
		// 카메라 이벤트도 발행
		// 카메라 이벤트도 발행
		CameraEvent shakeevent;
		shakeevent.eCameraAction = CAMERA_ACTION::SHAKE;
		shakeevent.tShake = ShakePreset::HitStop_PlayerDamage();
		m_pGameInstance->Publish(shakeevent);
	}

}

void Client::Player::Process_AttackSweep_Capsule()
{
	// 궤적 Sweep 공격 판정
	if (!m_bAttackSweepActive)
		return;

	// 두 소켓 위치로 캡슐 파라미터 계산
	_vector vStart = XMLoadFloat3(&m_vCurrentWeaponPos);
	_vector vEnd = XMLoadFloat3(&m_vCurrentWeaponPosEnd);

	// 캡슐 중점
	_vector vCenter = XMVectorScale(XMVectorAdd(vStart, vEnd), 0.5f);
	_float3 vCenterF3 = {};
	XMStoreFloat3(&vCenterF3, vCenter);

	// 캡슐 halfHeight = 두 소켓 간 거리의 절반
	_vector vAxis = XMVectorSubtract(vEnd, vStart);
	_float fCapsuleLength = 0.f;
	XMStoreFloat(&fCapsuleLength, XMVector3Length(vAxis));
	_float fHalfHeight = fCapsuleLength * 0.5f;

	// 캡슐 축 방향 쿼터니언 (PhysX 캡슐은 X축 정렬)
	_vector vAxisNorm = XMVector3Normalize(vAxis);
	_vector vDefaultAxis = XMVectorSet(1.f, 0.f, 0.f, 0.f);
	_vector vRotQuat = {};

	_float fDot = 0.f;
	XMStoreFloat(&fDot, XMVector3Dot(vDefaultAxis, vAxisNorm));

	if (fDot < -0.9999f)
		vRotQuat = XMQuaternionRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XM_PI);
	else if (fDot > 0.9999f)
		vRotQuat = XMQuaternionIdentity();
	else
	{
		_vector vCross = XMVector3Cross(vDefaultAxis, vAxisNorm);
		vRotQuat = XMVectorSetW(vCross, 1.f + fDot);
		vRotQuat = XMQuaternionNormalize(vRotQuat);
	}

	_float4 fQuat = {};
	XMStoreFloat4(&fQuat, vRotQuat);

	PxTransform currentPose(
		PxVec3(vCenterF3.x, vCenterF3.y, vCenterF3.z),
		PxQuat(fQuat.x, fQuat.y, fQuat.z, fQuat.w));

	// 이전 프레임 캡슐 중점 계산
	_float3 vPrevCenter = {};
	_vector vPrevStart = XMLoadFloat3(&m_vPrevWeaponPos);
	_vector vPrevEnd = XMLoadFloat3(&m_vPrevWeaponPosEnd);
	XMStoreFloat3(&vPrevCenter, XMVectorScale(XMVectorAdd(vPrevStart, vPrevEnd), 0.5f));

	_float3 vSweepDir = {
		vCenterF3.x - vPrevCenter.x,
		vCenterF3.y - vPrevCenter.y,
		vCenterF3.z - vPrevCenter.z
	};
	_float fSweepDist = sqrtf(
		vSweepDir.x * vSweepDir.x +
		vSweepDir.y * vSweepDir.y +
		vSweepDir.z * vSweepDir.z);

	PxTransform sweepPose(
		PxVec3(vPrevCenter.x, vPrevCenter.y, vPrevCenter.z),
		currentPose.q);

	// 거리에 따라 Overlap 또는 Sweep 선택
	vector<PHYSX_USER_DATA*> vecHitData;
	vector<_float3> vecHitPositions; // 접촉 위치 저장용

	if (fSweepDist < 0.01f)  
	{
		// 무기가 거의 안 움직였으면 Overlap
		// 반경을 캡슐 길이 고려해서 확장
		PHYSX_OVERLAP_RESULT overlapResult = m_pGameInstance->PhysX_Overlap_Sphere(
			m_fAttackRadius + fHalfHeight, currentPose);
		if (overlapResult.bHit)
		{
			vecHitData = overlapResult.vecUserData;
			vecHitPositions = overlapResult.vecHitData;
		}
	}
	else
	{
		// Sphere -> Capsule Multi Sweep
		PxVec3 dir(vSweepDir.x, vSweepDir.y, vSweepDir.z);
		PHYSX_MULTI_SWEEP_RESULT sweepResult = m_pGameInstance->PhysX_Sweep_Capsule_Multi(
			m_fAttackRadius, fHalfHeight, sweepPose, dir, fSweepDist);
		if (sweepResult.bHit)
		{
			vecHitData = sweepResult.vecUserData;
			vecHitPositions = sweepResult.vecPositions; // sweep 결과의 실제 접촉 위치 사용
		}
	}

	_bool bPlayedWeaponClash = false;

	// 히트 처리 (공통)
	for (_uint i = 0; i < vecHitData.size(); ++i)
	{
		PHYSX_USER_DATA* pUserData = vecHitData[i];
		if (pUserData == nullptr)
			continue;
		if (pUserData->iObjectID == m_iObjectID)
			continue;
		if (pUserData->eActorType != PX_ACTOR_TYPE::MONSTER)
			continue;
		if (m_setHitTargets.count(pUserData->iObjectID) > 0)
			continue;

		m_setHitTargets.insert(pUserData->iObjectID);

		// 데미지 이벤트 발행
		_float3 vAttackDir = {};
		_vector vLook = m_pTransformCom->Get_State(DIRECTION::LOOK);
		XMStoreFloat3(&vAttackDir, XMVector3Normalize(vLook));

		DAMAGE_EVENT damageEvent = {};
		damageEvent.iAttackerID = m_iObjectID;
		damageEvent.iTargetID = pUserData->iObjectID;
		damageEvent.fDamage = m_fAttackDamage * m_pPlayerStatCom->Get_DamageMultiplier() + m_pPlayerStatCom->Apply_EnchantDamage() + m_pPlayerStatCom->Get_Attack() + m_fAdditionalDamage;
		damageEvent.vHitPosition = vecHitPositions[i];
		damageEvent.vHitDirection = vAttackDir;
		damageEvent.fKnockbackForce = m_fKnockbackForce;
		damageEvent.eDamagePower = m_pStateMachineCom->Get_CurrentState_DamagePower();
		damageEvent.pDamageOwner = this;
		m_pGameInstance->Publish<DAMAGE_EVENT>(damageEvent);
		m_pPlayerStatCom->Apply_Myeonghyeol(1.0f); //명혈 추가
		bPlayedWeaponClash = true; //무기 사운드 재생해야한다.

		//COUT("데미지 :" << damageEvent.fDamage);

		// 카메라 이벤트도 발행
		CameraEvent shakeevent;
		shakeevent.eCameraAction = CAMERA_ACTION::SHAKE;
		shakeevent.tShake = ShakePreset::HitStop_PlayerDamage();
		m_pGameInstance->Publish(shakeevent);


		// 타격 이펙트 생성
		ParticleSystem* pEffect = CAST(ParticleSystem*)(PoolingManager::Get_Instance()->Acquire_Effect(POOL_ID::EFFECT_HIT));
		pEffect->Set_WorldPosition(vecHitPositions[i]);
		pEffect->OnSpawn(nullptr);

		// 불꽃 이펙트 테스트
		if (m_pPlayerStatCom->Get_EnchnatActive()==true)
		{
			pEffect = CAST(ParticleSystem*)(POOLING->Acquire_Effect(POOL_ID::EFFECT_HIT_FLAME));
			pEffect->Set_WorldPosition(vecHitPositions[i]);
			pEffect->OnSpawn(nullptr);
		}
	}

	if (bPlayedWeaponClash && m_pActiveWeapon !=nullptr)
	{
		m_pActiveWeapon->Playing_WeaponClash_Sound(); //현재 장착중인 무기의 사운드 재생
	}

	//if (m_bAttackSweepActive)
	//{
	//	COUT("[Sweep] Active | Prev(" << m_vPrevWeaponPos.x << ", " << m_vPrevWeaponPos.y << ", " << m_vPrevWeaponPos.z
	//		<< ") Cur(" << m_vCurrentWeaponPos.x << ", " << m_vCurrentWeaponPos.y << ", " << m_vCurrentWeaponPos.z
	//		<< ") Dist: " << fSweepDist);
	//}
}

void Client::Player::Teleport_Oliver()
{
	_vector pos = XMVectorSet(-178.082f, 7.471f, 103.251, 1.f);
	_float4 rot = { 0.f, 100.f, 0.f, 0.f };

	Teleport(pos, rot);
}

void Client::Player::Teleport_Wolf()
{
	_vector pos = XMVectorSet(-55.839f, 9.004f, -85.858f, 1.f);
	_float4 rot = { 0.f, 0.f, 0.f, 0.f };

	Teleport(pos, rot);
}
/******************************************************* PhysX 이동, 전투 함수 *******************************************************/

//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Player* Client::Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	Player* pInstance = new Player(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"Player 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* Client::Player::Clone(void* pArg)
{
	Player* pInstance = new Player(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"Player 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::Player::Free()
{
	__super::Free();
	if (m_pController)
		m_pController = nullptr;
	Safe_Release(m_pMasterModel);

	Safe_Release(m_pPlayerStatCom);

	//Safe_Release(m_pMasterRig);
	
	//for (_uint i = 0; i < _UINT(WEAPON_TYPE::WP_END); ++i)
	//{
	//	Safe_Release(m_pWeapons[i]);
	//}
}

/******************************************************* 객체 반환 함수 *******************************************************/
