#include "ShaderTool_Define.h"
#include "Player.h"
#include "Player_MasterRig.h"
#include "Player_Body.h"
#include "Player_Hair.h"
#include "Player_Head.h"
#include "Player_Weapon.h"
#include "PWeapon_Bayonet.h"

#include "Player_Stat.h"

#include "Player_Idle.h"
#include "Player_Run.h"

#include "PhysX_Function.h"

#pragma region EFFECTS_HEADER
#include "ParticleSystem.h"
#include "TrailEffect.h"
#pragma endregion

#include "Bone.h"
#include "Model.h"

ShaderTool::Player::Player()
{
}

ShaderTool::Player::Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:ContainerObject(pDevice, pContext)
{
}

ShaderTool::Player::Player(const Player& original)
	:ContainerObject(original)
{
}

ShaderTool::Player::~Player()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



void ShaderTool::Player::Reset_PlayerAnimEventDesc()
{
	m_tPlayerRunTimeEvent.iInputAreaCount = 0;
	m_tPlayerRunTimeEvent.iCanMoveCount = 0;
	m_tPlayerRunTimeEvent.iCanComboCount = 0;
	m_tPlayerRunTimeEvent.iCanEscapeCount = 0;

}

void ShaderTool::Player::Reset_PlayerMoveInputDesc()
{
	m_tInputDesc.InputDir = INPUT_DIR::INPUT_END;
	m_tInputDesc.vMoveDir = {};
}

void ShaderTool::Player::Reset_PlayerPreesedInputDesc()
{
	m_tInputDesc.bShiftPressed = { false };
	m_tInputDesc.bAltPressed = { false };
}

void ShaderTool::Player::Reset_PlayerConsumeDesc()
{
	m_tInputDesc.bSpaceDown = false;
	m_tInputDesc.bAltDown = false;
	m_tInputDesc.bAltUp = false;
	m_tInputDesc.eLastMouseKeyState = MOUSEKEYSTATE::END;
}

//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT ShaderTool::Player::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT ShaderTool::Player::Initialize(void* arg)
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
		pArgDesc->tControllerDesc.iObjectID = m_iObjectID;

		m_pController = m_pGameInstance->PhysX_Create_Controller(pArgDesc->tControllerDesc);
	}

	/*				필수				 */
	m_pGameInstance->Add_Player(this);
	//// 플레이어의 피직스상 몸체? 생성
	//if (pArgDesc && pArgDesc->pControllerManager)
	//{
	//	physx::PxCapsuleControllerDesc capsuleDesc;
	//	capsuleDesc.height      = 1.0f;  // 키
	//	capsuleDesc.radius      = 0.2f; // 둘레
	//	capsuleDesc.stepOffset  = 0.3f; // 탈수있는 언덕? 몇정도 높이까지 탈수있나 
	//	capsuleDesc.contactOffset = 0.1f; // 충돌 여유공간. 바닥에 끼지않게
	//	capsuleDesc.material    = pArgDesc->pMaterial; //재질. 사실 잘모르겠음...

	//	// 초기 위치
	//	capsuleDesc.position    = physx::PxExtendedVec3(pArgDesc->vPosition.x, pArgDesc->vPosition.y, pArgDesc->vPosition.z);
	//	// 컨트롤러에 물리 월드에다가 캐릭터 생성하도록
	//	m_pController = pArgDesc->pControllerManager->createController(capsuleDesc);
	//}

	pArgDesc->wstrName = L"Player_" + to_wstring(namenum++);

	CHECK_FAILED(GameObject::Initialize(pArgDesc), E_FAIL);
	CHECK_FAILED(Ready_Components(), E_FAIL);
	CHECK_FAILED(Ready_PartObjects(), E_FAIL);
	CHECK_FAILED(Ready_States(), E_FAIL);
	CHECK_FAILED(Ready_Event(), E_FAIL);
	CHECK_FAILED(Ready_PhysXEvent(), E_FAIL);


	Reset_PlayerMoveInputDesc();
	Reset_PlayerPreesedInputDesc();
	Reset_PlayerConsumeDesc();
	m_pTransformCom->Rotation(0.f, XMConvertToRadians(180.f), 0.f);
	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 최초 업데이트 함수 ////////////////////////////////////////////////////////
_int ShaderTool::Player::Update_Priority(const _float fTimeDelta)
{
	__super::Update_Priority(fTimeDelta);

	//if (m_pGameInstance->KeyPress(DIK_LALT))
	//{
	//	if (m_pGameInstance->KeyDown(DIK_SLASH))
	//	{
	//		m_pMasterRigModel->Toggle_DebugBone("RightHandAttachSocket");
	//		COUT("오");
	//	}
	//}

	return 0;
}
/******************************************************* 최초 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
_int ShaderTool::Player::Update_Parallel(const _float fTimeDelta)
{
	__super::Update_Parallel(fTimeDelta);

	//플레이어 중력 부여하는 키. 꼭 맵 다 로딩하고 누르기!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! 안그러면 저아래로 꺼짐 
	if (m_pGameInstance->KeyDown(DIK_UP))
	{
		m_bEnablePhysics = !m_bEnablePhysics;

		if (!m_bEnablePhysics)
			m_fGravity = 0.f;

		if (m_pGameInstance->KeyPress(DIK_LALT))
			m_fGravity = -5.f;
	}

	if (m_pGameInstance->KeyDown(DIK_K))
	{
		_float fHp = m_pStatCom->Get_Hp();
		COUT("Hp : " << fHp);
	}

	ChangeWeapon(); //무기 체인지 확인용

	return 0;
}

_int ShaderTool::Player::Update(const _float fTimeDelta)
{
	if (m_pGameInstance->Get_DIKeyState(DIK_H))
	{
		m_bFix = !m_bFix;
	}
	Gather_Input();
	m_pStateMachineCom->Update_State(fTimeDelta);	// 상태머신 업데이트
	m_pUpperStateMachineCom->Update_State(fTimeDelta);

	Apply_MoveAndRotation(fTimeDelta);
	Update_RotationLerp(fTimeDelta);

	__super::Update(fTimeDelta);

	// 무기 본 월드 위치 갱신
	Update_WeaponPosition();

	// 공격 Sweep 판정
	Process_AttackSweep();

	// Trail 재생
	if (m_bTrailActive &&
		m_vecTrailEffects.empty() == false)
	{
		_float3 vFinalRoot = {}, vFinalTip = {};

		vFinalRoot.x = m_pWeaponTrailRoot->_41;
		vFinalRoot.y = m_pWeaponTrailRoot->_42;
		vFinalRoot.z = m_pWeaponTrailRoot->_43;

		vFinalTip.x = m_pWeaponTrailTip->_41;
		vFinalTip.y = m_pWeaponTrailTip->_42;
		vFinalTip.z = m_pWeaponTrailTip->_43;

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
#endif
	Update_PlayerCustomDesc();
	m_pGameInstance->Follow_ShadowCascade_Matrix(m_pTransformCom->Get_State(DIRECTION::POSITION)); //
	return 0;
}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 레이트 업데이트 함수 ////////////////////////////////////////////////////////
_int ShaderTool::Player::Update_Late(const _float fTimeDelta)
{
	__super::Update_Late(fTimeDelta);

	//디버그일때 네비게이션이랑 콜라이더 넣을예정이면 플레이어도 렌더그룹에 추가해줘야함 

	if (m_bAttackSweepActive)
	{
		// 무기 위치에 디버그 구체 그리기
		BoundingSphere debugSphere;
		debugSphere.Center = m_vCurrentWeaponPos;
		debugSphere.Radius = m_fAttackRadius;

#ifdef _DEBUG
		m_pGameInstance->Add_Debug_Sphere(debugSphere);
#endif // _DEBUG

	}

#ifdef _DEBUG
	// m_pMasterRigModel->Render_DebugBones(m_pTransformCom->Get_WorldMatrix());
#endif // _DEBUG


	return 0;
}
/******************************************************* 레이트 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 렌더 함수 ////////////////////////////////////////////////////////
HRESULT ShaderTool::Player::Render(const _float fTimeDelta)
{
	return S_OK;
}
/******************************************************* 렌더 함수 *******************************************************/



void ShaderTool::Player::Gather_Input()
{
	Reset_PlayerMoveInputDesc();
	Reset_PlayerPreesedInputDesc();

	State* pCurState = m_pStateMachineCom->Get_CurrentState();

	if (pCurState->Get_HasAnimEvents() == true) //애니메이션 이벤트가 있는 상태 객체
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
			}

			if (m_pGameInstance->KeyUp(DIK_LALT))
				m_tInputDesc.bAltUp = true;

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

		if (m_pGameInstance->KeyDown(DIK_LALT))
		{
			m_bOnGuard = true;
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
	}

	if (m_bFix)
	{
		_bool bW = m_pGameInstance->KeyPress(DIK_W);
		_bool bA = m_pGameInstance->KeyPress(DIK_A);
		_bool bS = m_pGameInstance->KeyPress(DIK_S);
		_bool bD = m_pGameInstance->KeyPress(DIK_D);

		if (bW)m_tInputDesc.vMoveDir += XMVectorSet(0.f, 0.f, 1.f, 1.f);
		if (bA)m_tInputDesc.vMoveDir += XMVectorSet(-1.f, 0.f, 0.f, 1.f);
		if (bS)m_tInputDesc.vMoveDir += XMVectorSet(0.f, 0.f, -1.f, 1.f);
		if (bD)m_tInputDesc.vMoveDir += XMVectorSet(1.f, 0.f, 0.f, 1.f);

		if (bW && bA) m_tInputDesc.InputDir = INPUT_DIR::LEFT_UP;
		else if (bW && bD) m_tInputDesc.InputDir = INPUT_DIR::RIGHT_UP;
		else if (bS && bA) m_tInputDesc.InputDir = INPUT_DIR::LEFT_DOWN;
		else if (bS && bD) m_tInputDesc.InputDir = INPUT_DIR::RIGHT_DOWN;
		else if (bW)m_tInputDesc.InputDir = INPUT_DIR::UP;
		else if (bA)m_tInputDesc.InputDir = INPUT_DIR::LEFT;
		else if (bS)m_tInputDesc.InputDir = INPUT_DIR::DOWN;
		else if (bD)m_tInputDesc.InputDir = INPUT_DIR::RIGHT;
	}
}

void ShaderTool::Player::Apply_MoveAndRotation(_float fTimeDelta)
{

	// 물리 이동 로직
	if (m_pController)
	{
		if (!m_bApplyTranslation)
			m_tInputDesc.vMoveDir = XMVectorZero();

		physx::PxVec3 vMoveDir = ToPxVec3(m_tInputDesc.vMoveDir);

		_float fSpeed = 5.f;
		if (m_pGameInstance->KeyPress(DIK_LSHIFT))
			fSpeed = 7.f;

		if (m_tInputDesc.bAltDown)
			fSpeed = 0.f;
		if (m_tInputDesc.bAltPressed)
			fSpeed = 1.f;
		// y축이동막음. 비행 구현할거라면..?
		vMoveDir.y = 0.f;
		if (vMoveDir.magnitudeSquared() > 0.0001f)
			vMoveDir.normalize(); //대각선 이동 속도 정규화

		physx::PxVec3 vFinalVelocity = vMoveDir * fSpeed * fTimeDelta;


		// 중력 처리
		// 바닥에 닿아있을때의 중력 / 공중에 있을때의 중력(가속 붙음)
		if (m_bEnablePhysics)
		{
			if (m_bIsGrounded)
				m_fGravity = -0.5f; // 바닥 접착력 (경사면에서 떨어지지 않게)
			else
				m_fGravity -= 1.f * fTimeDelta; // 공중: 중력 가속
		}

		_vector vRootWorldDelta = m_pMasterRig->Get_RootWorldDelta();
		PxVec3 RootWorldDelta = ToPxVec3(vRootWorldDelta);

		vFinalVelocity.y = m_fGravity;
		vFinalVelocity += RootWorldDelta; //루트모션 적용

		// 부딪힐 물체 필터링(아군이나 무기같은거랑 부딪혀서 끼지않게)
		physx::PxControllerFilters filters;

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

		// 모델 중앙기준점과 엔진의 캡슐 중앙기준점이 맞지않아서 보정. 캐릭터가 너무 떠있거나 박혀있으면 조정
		_float fYOffset = (1.0f * 0.5f) + 0.2f;

		_vector vPos = XMVectorSet((_float)pxPos.x, (_float)pxPos.y - fYOffset, (_float)pxPos.z, 1.f);
		m_pTransformCom->Set_State(DIRECTION::POSITION, vPos);
	}

	//if (bInputChange)
	//	Start_Dir_RotationLerp();

	if (XMVectorGetX(XMVector3LengthSq(m_tInputDesc.vMoveDir)) > 0.0001f)
	{
		if (m_bOnGuard)
		{
			m_pTransformCom->LookDir(m_vSaveCameraLook);
		}
		else
		{
			Start_Dir_RotationLerp();
		}
	}

}

void ShaderTool::Player::Start_Dir_RotationLerp()
{
	//여기서 이제 내가 바라보는 방향으로 회전시작하라고 m_pTransformCom에 명령
	m_pTransformCom->Statrt_Rotation_Lerp(m_tInputDesc.vMoveDir, 0.1f);
}

void ShaderTool::Player::Update_RotationLerp(_float fTimeDelta)
{
	m_pTransformCom->Update_Rotation_Lerp(fTimeDelta);
}

void ShaderTool::Player::Apply_AnimEventCount(const PLAYERANIM_EVENT& PlayerAnimEvent, _int iSign)
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

}

void ShaderTool::Player::Update_Playerstat(const PLAYERANIM_EVENT& PlayerAnimEvent)
{
	Player_Stat* pPlayerStatCom = dynamic_cast<Player_Stat*>(m_pStatCom);

	//pPlayerStatCom->Apply_Stamina(-PlayerAnimEvent.fStaminaCost);
	_float fMaxStamina = pPlayerStatCom->Get_MaxStamina();
	_float fCurrentStamina = pPlayerStatCom->Get_Stamina();
	if (PlayerAnimEvent.bBlockRegenStamina) //스태미나 재생 못할때 프레임구간 
	{

	}
	else //스태미나 재생 가능할때 프레임 구간 
	{
		if (fCurrentStamina < fMaxStamina)
		{


			//pPlayerStatCom->Apply_Stamina();
		}
		//여기서는 최대치 클램프해서 시간에 따라 ? 


	}




	//COUT("Player_CurrentStamina : " << fCurrentStamina);
}

//////////////////////////////////////////////////////// 무기 변경 함수 ////////////////////////////////////////////////////////
void ShaderTool::Player::ChangeWeapon()
{
	if (m_pGameInstance->KeyDown(DIK_Y))
	{
		m_pActiveWeapon->Set_PartActive(false);
		m_pActiveWeapon = m_pWeapons[_UINT(WEAPON_TYPE::GREATESWORD)];
		m_pActiveWeapon->Set_PartActive(true);
	}
	if (m_pGameInstance->KeyDown(DIK_U))
	{
		m_pActiveWeapon->Set_PartActive(false);
		m_pActiveWeapon = m_pWeapons[_UINT(WEAPON_TYPE::SWORD)];
		m_pActiveWeapon->Set_PartActive(true);
	}
	if (m_pGameInstance->KeyDown(DIK_I))
	{
		m_pActiveWeapon->Set_PartActive(false);
		m_pActiveWeapon = m_pWeapons[_UINT(WEAPON_TYPE::HALBERD)];
		m_pActiveWeapon->Set_PartActive(true);
	}
	if (m_pGameInstance->KeyDown(DIK_O))
	{
		m_pActiveWeapon->Set_PartActive(false);
		m_pActiveWeapon = m_pWeapons[_UINT(WEAPON_TYPE::HAMMER)];
		m_pActiveWeapon->Set_PartActive(true);
	}
	if (m_pGameInstance->KeyDown(DIK_P))
	{
		m_pActiveWeapon->Set_PartActive(false);
		m_pActiveWeapon = m_pWeapons[_UINT(WEAPON_TYPE::BAYONET)];
		m_pActiveWeapon->Set_PartActive(true);
	}
}
/******************************************************* 무기 변경 함수 *******************************************************/



//////////////////////////////////////////////////////// 컨테이너 추가 함수 ////////////////////////////////////////////////////////
HRESULT ShaderTool::Player::Ready_Components()
{
	CHECK_FAILED(Add_Component(m_iLevel, L"Prototype_Component_StateMachine", Com_StateMachine, RCAST(Component**)(&m_pStateMachineCom)), E_FAIL);
	CHECK_FAILED(Add_Component(m_iLevel, L"Prototype_Component_StateMachine", L"UpperStateMachine_Component", RCAST(Component**)(&m_pUpperStateMachineCom)), E_FAIL);

	Player_Stat::PLAYERSTAT_DESC PlayerStatDesc;
	PlayerStatDesc.eBloodCode = BLOODCODE::BLOODCODE_END;
	PlayerStatDesc.fMaxHp = 1000.f;
	PlayerStatDesc.fCurrentHp = PlayerStatDesc.fMaxHp;
	PlayerStatDesc.iAttack = 50;
	PlayerStatDesc.iDefense = 20;
	PlayerStatDesc.iMaxMyeonghyeol = 30;
	PlayerStatDesc.iCurrentMyeonghyeol = PlayerStatDesc.iMaxMyeonghyeol;
	PlayerStatDesc.iHaze = 0;
	PlayerStatDesc.fMaxStamina = 200;
	PlayerStatDesc.fCurrentStamina = PlayerStatDesc.fMaxStamina;
	PlayerStatDesc.fLowStamina = 20;
	PlayerStatDesc.fMaxFocusGauge = 50;
	PlayerStatDesc.fCurrentFocusGauge = 0;

	//CHECK_FAILED(Add_Component(m_iLevel, L"Prototype_Component_PlayerStat", Com_Stat, RCAST(Component**)(&m_pStatCom), &PlayerStatDesc), E_FAIL);


	return S_OK;
}

HRESULT ShaderTool::Player::Ready_PartObjects()
{
	//MasterRig == 마스터 본(파츠들의 중심이 되는 뼈)
	Player_MasterRig::MASTERRIG_DESC MasterRigDesc = {};
	MasterRigDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
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
	BodyDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	BodyDesc.pMasterRig = m_pMasterRig;
	CHECK_FAILED(__super::Add_PartObject(m_iLevel, Proto_GameObject(L"Player_Body"), TEXT("Part_Body"), &BodyDesc), E_FAIL);

	////헤어
	Player_Hair::HAIR_DESC HairDesc = {};
	HairDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	HairDesc.pMasterRig = m_pMasterRig;
	CHECK_FAILED(__super::Add_PartObject(m_iLevel, Proto_GameObject(L"Player_Hair"), TEXT("Part_Hair"), &HairDesc), E_FAIL);

	//헤드
	Player_Head::HEAD_DESC HeadDesc = {};
	HeadDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	HeadDesc.pMasterRig = m_pMasterRig;
	CHECK_FAILED(__super::Add_PartObject(m_iLevel, Proto_GameObject(L"Player_Head"), TEXT("Part_Head"), &HeadDesc), E_FAIL);

	//총검 무기
	PWeapon_Bayonet::WEAPONBAYONET_DESC BayonetDesc = {};
	BayonetDesc.pSocketMatrix = m_pMasterRig->Get_SocketMatrix("RightHandAttachSocket");
	BayonetDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	BayonetDesc.eWeaponType = WEAPON_TYPE::BAYONET;
	CHECK_FAILED(__super::Add_PartObject(m_iLevel, Proto_GameObject(L"Player_Weapon_Bayonet"), TEXT("Part_Weapon_Bayonet"), &BayonetDesc), E_FAIL);


	m_pWeapons[_UINT(WEAPON_TYPE::BAYONET)] = dynamic_cast<ShaderTool::Player_Weapon*>(Find_PartObject(TEXT("Part_Weapon_Bayonet")));

	for (_uint i = 0; i < 1; ++i)
	{
		if (m_pWeapons[i] == nullptr)
			return E_FAIL;

		m_pWeapons[i]->Set_PartActive(false);

		Safe_AddRef(m_pWeapons[i]);

	}

	m_pActiveWeapon = m_pWeapons[_UINT(WEAPON_TYPE::BAYONET)];
	m_pWeapons[_UINT(WEAPON_TYPE::BAYONET)]->Set_PartActive(true);

	/* 마스터본 가져와서 Player에도 등록 */
	m_pMasterRigModel = m_pMasterRig->Get_Model();
	/* 무기 매트릭스 */
	// MSG_FAIL(m_pActiveWeapon->Get_Model()->Register_ReadbackBoneName("Barrel_00_nouse", 0), L"않되", L"ㅠㅠ", E_FAIL);
	m_pWeaponBoneMatrix = const_cast<_float4x4*>(m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr("TrailEndSocket"));	// 콜라이더용
	m_pWeaponTrailRoot = const_cast<_float4x4*>(m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr("TrailEndSocket"));
	m_pWeaponTrailTip = const_cast<_float4x4*>(m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr("TrailStartSocket"));
	/* 무기 본 월드매트릭스 가져와서 등록 */
	// m_pWeaponBoneMatrix = m_pActiveWeapon->Get_Model()->Get_ReadbackBoneMatrixPtr_Nonconst("Barrel_00_nouse");

	return S_OK;
}

HRESULT ShaderTool::Player::Ready_States()
{
	//기본 행동
	CHECK_FAILED(m_pStateMachineCom->Add_State(IDLE, Player_Idle::Create(this, m_pMasterRig)), E_FAIL);
	CHECK_FAILED(m_pStateMachineCom->Add_State(RUN, Player_Run::Create(this, m_pMasterRig)), E_FAIL);

	m_pStateMachineCom->Set_State(IDLE);

	return S_OK;
}

HRESULT ShaderTool::Player::Ready_Event()
{
	// ParticleSystem 구독
	Subscribe_Event<PARTICLE_EVENT>([this](const PARTICLE_EVENT& e) {
		// 이벤트 매니저에서 ParticleSystem 찾기
		ParticleSystem* pSystem = m_pGameInstance->Find_Effect(stringToWstring(e.ParticleSystemName));
		_matrix matPlayerWorld = m_pTransformCom->Get_WorldMatrix();
		_matrix matBone = {};
		_float4x4 matFinalPos = {};

		// 뼈의 위치 가져오기
		const _float4x4* pMatBone = { nullptr };
		if (e.bAttached)
		{
			// 무기 뼈
			pMatBone = m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr(e.SocketName);
			if (pMatBone == nullptr)
				return E_FAIL;
			_float3 vBoneWorldPos = _float3(pMatBone->_41, pMatBone->_42, pMatBone->_43);

			const _float4x4* pSocket = m_pMasterRig->Get_SocketMatrix("RightHandAttachSocket");
			_matrix matRotation = XMMatrixRotationX(XMConvertToRadians(90.f)) * XMLoadFloat4x4(pSocket) * matPlayerWorld;

			matRotation.r[3] = XMVectorSet(vBoneWorldPos.x, vBoneWorldPos.y, vBoneWorldPos.z, 1.f);
			XMStoreFloat4x4(&matFinalPos, matRotation);
		}
		else
		{
			// 플레이어 뼈
			pMatBone = m_pMasterModel->Get_ReadbackBoneMatrixPtr(e.SocketName);
			if (pMatBone == nullptr)
				return E_FAIL;

			matBone = XMLoadFloat4x4(pMatBone);
			XMStoreFloat4x4(&matFinalPos, matBone * matPlayerWorld);
		}

		if (pSystem != nullptr)
		{
			// 파티클의 위치를 가져온 뼈의 위치로 설정
			pSystem->Set_WorldMatrix(matFinalPos);

			// 파티클시스템 재생
			pSystem->Play();
		}
	});


	// SwordTrail 구독
	Subscribe_Event<TRAIL_EVENT>([this](const TRAIL_EVENT& e) {
		if (e.ePhase == ANIM_FRAMEPHASE::START)
		{
			// 이펙트 매니저에서 ParticleSystem 찾기
			ParticleSystem* pSystem = m_pGameInstance->Find_Effect(stringToWstring(e.TrailEffectName));

			if (pSystem != nullptr)
			{
				pSystem->Play();
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
	});

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
			Update_Playerstat(Event);
			//m_tPlayerRunTimeEvent.bBlockRegenStamina = Event.bBlockRegenStamina;
			break;

		case ANIM_FRAMEPHASE::UPDATE:
			break;
		case ANIM_FRAMEPHASE::END:
			Apply_AnimEventCount(Event, -1);
			//m_tPlayerRunTimeEvent.bBlockRegenStamina = Event.bBlockRegenStamina;
			break;
		}

	});

	return S_OK;
}

HRESULT ShaderTool::Player::Ready_PhysXEvent()
{
	m_pMasterRigModel->Set_OwnerId(m_iObjectID);

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
			m_fAttackDamage = _event.fAttackDamage;
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
/******************************************************* 컨테이너 추가 함수 *******************************************************/



//////////////////////////////////////////////////////// 바인딩 함수 ////////////////////////////////////////////////////////
HRESULT ShaderTool::Player::Bind_ShaderResources()
{
	return S_OK;
}
/******************************************************* 바인딩 함수 *******************************************************/



//////////////////////////////////////////////////////// PhysX 이동, 전투 함수 ////////////////////////////////////////////////////////
Model* ShaderTool::Player::Get_MasterRigModel()
{
	return m_pMasterRigModel ? m_pMasterRigModel : nullptr;
}

void ShaderTool::Player::Update_WeaponPosition()
{
	// Readback에서 무기 본 Combined 행렬을 가져와서 월드 좌표 계산
	if (m_pMasterRigModel == nullptr)
		return;

	m_vPrevWeaponPos = m_vCurrentWeaponPos;

	if (m_pWeaponBoneMatrix == nullptr)
		return;

	// Combined 행렬(모델 공간)에 월드 행렬을 곱해서 실제 월드 위치 계산
	_matrix matWeaponWorld = XMLoadFloat4x4(m_pWeaponBoneMatrix);
	//* XMLoadFloat4x4(m_pTransformCom->Get_WorldMatrixPtr());

	XMStoreFloat3(&m_vCurrentWeaponPos, matWeaponWorld.r[3]);
}

void ShaderTool::Player::Process_AttackSweep()
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

	if (fSweepDist < 0.01f)
	{
		// 무기가 거의 안 움직였으면 Overlap
		pose.p = PxVec3(m_vCurrentWeaponPos.x, m_vCurrentWeaponPos.y, m_vCurrentWeaponPos.z);
		PHYSX_OVERLAP_RESULT overlapResult = m_pGameInstance->PhysX_Overlap_Sphere(m_fAttackRadius, pose);

		if (overlapResult.bHit)
			vecHitData = overlapResult.vecUserData;
	}
	else
	{
		// 궤적 Sweep
		PxVec3 dir(vSweepDir.x, vSweepDir.y, vSweepDir.z);
		PHYSX_MULTI_SWEEP_RESULT sweepResult = m_pGameInstance->PhysX_Sweep_Sphere_Multi(m_fAttackRadius, pose, dir, fSweepDist);

		if (sweepResult.bHit)
			vecHitData = sweepResult.vecUserData;
	}

	// 히트 처리 (공통)
	for (PHYSX_USER_DATA* pUserData : vecHitData)
	{
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
		damageEvent.fDamage = m_fAttackDamage;
		damageEvent.vHitPosition = m_vCurrentWeaponPos;
		damageEvent.vHitDirection = vAttackDir;
		damageEvent.fKnockbackForce = m_fKnockbackForce;

		m_pGameInstance->Publish<DAMAGE_EVENT>(damageEvent);

#ifdef _DEBUG
		COUT("[Hit] Target: " << pUserData->iObjectID
			<< " Damage: " << damageEvent.fDamage);
#endif
	}


	//if (m_bAttackSweepActive)
	//{
	//	COUT("[Sweep] Active | Prev(" << m_vPrevWeaponPos.x << ", " << m_vPrevWeaponPos.y << ", " << m_vPrevWeaponPos.z
	//		<< ") Cur(" << m_vCurrentWeaponPos.x << ", " << m_vCurrentWeaponPos.y << ", " << m_vCurrentWeaponPos.z
	//		<< ") Dist: " << fSweepDist);
	//}
}
/******************************************************* PhysX 이동, 전투 함수 *******************************************************/



/////////////////////////디버그용유지안할거임///////////////////////////////////////
void ShaderTool::Player::Teleport(_fvector vTargetPos)
{
	m_pTransformCom->Set_State(DIRECTION::POSITION, vTargetPos);

	if (m_pController)
	{
		_float3 vPos;
		XMStoreFloat3(&vPos, vTargetPos);

		_float fYOffset = (1.0f * 0.5f) + 0.2f;

		m_pController->setPosition(physx::PxExtendedVec3(vPos.x, vPos.y + fYOffset, vPos.z));
	}
}



void ShaderTool::Player::Update_PlayerCustomDesc()
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
	if (m_iPrevBodyNum != *m_tPlayerMeshTex.pPlayerBodyNum) //옷의 번호가 이전꺼랑 다르면 색상변경 시켜주기
	{
		memcpy(m_tPlayerShaderDesc.m_tBodyDesc.vInnerColor, m_vInnerColor[*m_tPlayerMeshTex.pPlayerBodyNum].data(), sizeof(_float4) * 6);
	}
	m_iPrevBodyNum = *m_tPlayerMeshTex.pPlayerBodyNum;
}

//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Player* ShaderTool::Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	Player* pInstance = new Player(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"Player 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* ShaderTool::Player::Clone(void* pArg)
{
	Player* pInstance = new Player(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"Player 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void ShaderTool::Player::Free()
{
	__super::Free();
	if (m_pController)
		m_pController = nullptr;
	Safe_Release(m_pMasterModel);
	//Safe_Release(m_pMasterRig);

	//for (_uint i = 0; i < _UINT(WEAPON_TYPE::WP_END); ++i)
	//{
	//	Safe_Release(m_pWeapons[i]);
	//}
}
/******************************************************* 객체 반환 함수 *******************************************************/
