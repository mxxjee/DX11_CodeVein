#include "Client_Define.h"
#include "Yakumo.h"
#include "PhysX_Function.h"
#include "Yakumo_Weapon.h"
#include "PoolingManager.h"
#include "TrailEffect.h"
#include "Player.h"
#include "Monster.h"
#include "Layer.h"
#include "Player_Stat.h"
#include "Stat.h"

#pragma region State
#include "Yakumo_Idle.h"
#include "Yakumo_Walk.h"
#include "Yakumo_Run.h"
#include "Yakumo_Dash.h"
#include "Yakumo_AttackNormal.h"
#include "Yakumo_AttackStrong.h"
#include "Yakumo_SpecialAttack.h"
#include "Yakumo_Roll.h"
#include "Yakumo_BackStep.h"
#include "Yakumo_AttackDodgeF.h"
#include "Yakumo_AttackDodgeB.h"
#include "Yakumo_Damage.h"
#include "Yakumo_DamageBlow.h"
#include "Yakumo_DamageEnd.h"
#include "Yakumo_Buddy.h"
#include "Yakumo_GiftHeal.h"
#include "Yakumo_Death.h"

#pragma endregion

#include "SoundManager.h"

Client::Yakumo::Yakumo()
{
}

Client::Yakumo::Yakumo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:ContainerObject(pDevice,pContext)
{
}

Client::Yakumo::Yakumo(const Yakumo& original)
	:ContainerObject(original)
{
}

Client::Yakumo::~Yakumo()
{
}

void Client::Yakumo::Reset_RunTimeAnimEventDesc()
{
	m_tAnimRunTimeEvent.iInputAreaCount = 0;
	m_tAnimRunTimeEvent.iCanMoveCount = 0;
	m_tAnimRunTimeEvent.iCanComboCount = 0;
	m_tAnimRunTimeEvent.iCanEscapeCount = 0;
	m_tAnimRunTimeEvent.iCanSuperArmorCount = 0;
	m_tAnimRunTimeEvent.iCanInvincible = 0;
	m_bAttackSweepActive = false;
}

HRESULT Client::Yakumo::Initialize_Prototype(LEVEL _level)
{
	m_iLevel = _UINT(_level);

	return S_OK;
}

HRESULT Client::Yakumo::Initialize(void* arg)
{
	static _uint namenum = 0;

	YAKUMO_DESC*  pArgDesc = CAST(YAKUMO_DESC*)(arg);
	
	//컨트롤러
	PHYSX_CONTROLLER_DESC controllerDesc;
	controllerDesc.vPosition.x = pArgDesc->vPosition.x;
	controllerDesc.vPosition.y = pArgDesc->vPosition.y;
	controllerDesc.vPosition.z = pArgDesc->vPosition.z;

	controllerDesc.fRadius = 0.5f;
	controllerDesc.fHeight = 0.6f;
	controllerDesc.fSlopeLimit = 50.f;
	controllerDesc.eActorType = PHYSX_ACTOR_TYPE::COMPANION;
	controllerDesc.iObjectID = m_iObjectID;
	controllerDesc.pOwner = this;

	pArgDesc->fRotationSpeed = 360.f;

	// 피직스 컨트롤러 만들기
	m_pController = Create_Controller(controllerDesc);

	pArgDesc->wstrName = L"Yakumo_" + to_wstring(namenum++);

	CHECK_FAILED(__super::Initialize(pArgDesc), E_FAIL);

	m_pTransformCom->Set_Scale(1.1f, 1.1f, 1.1f);

	CHECK_FAILED(Ready_Components(), E_FAIL);
	CHECK_FAILED(m_pModelCom->Register_SocketBoneName("RightHandAttachSocket"), E_FAIL);
	CHECK_FAILED(m_pModelCom->Register_SocketBoneName("LeftHandAttachSocket"), E_FAIL);
	CHECK_FAILED(m_pModelCom->Register_SocketBoneName("Spine"), E_FAIL);
	m_pModelCom->Set_Animation_CS(1, true);
	m_pModelCom->Play_Animation_CS(0.f);

	CHECK_FAILED(Ready_PartObjects(), E_FAIL);
	CHECK_FAILED(Ready_States(), E_FAIL);
	CHECK_FAILED(Ready_Event(), E_FAIL);
	CHECK_FAILED(Ready_PhysXEvent(), E_FAIL);

	// 컨트롤러 위치 맞춰주기
	m_bEnablePhysics = true;
	m_pController->setFootPosition(ToPxExtendedVec3(Get_Position()));

	m_pGameInstance->Add_Companion(this);

	//사운드 객체 세팅
	m_pGameInstance->SoundMgr()->Set_VolumeModifier(
		[this](_uint iOwnerId, _float fOriginalVolume) -> _float
		{
			if (iOwnerId != m_iObjectID)
				return fOriginalVolume; 

			_float fDist = m_tAITargetInfo.fDistToPlayer;
			_float fRatio = 1.f - clamp(fDist / m_fMaxSoundDist, 0.f, 1.f);
			return fOriginalVolume * fRatio;
		});

	return S_OK;
}

_int Client::Yakumo::Update_Priority(const _float fTimeDelta)
{
	if (!m_bIsActive)
		return -1;

	__super::Update_Priority(fTimeDelta);

	return 0;
}

_int Client::Yakumo::Update_Parallel(const _float fTimeDelta)
{
	if (!m_bIsActive)
		return -1;

	__super::Update_Parallel(fTimeDelta);

	m_pModelCom->Update_Socket();

	// 시네마틱 상태면 RootPos 업데이트 막음
	if (m_bCinematic == true)
	{
		return 0;
	}

	XMStoreFloat3(&m_vRootMotionDelta, Calculate_RootPos(true, true, true));

	return 0;
}

_int Client::Yakumo::Update(const _float fTimeDelta)
{
	if (!m_bIsActive)
	{
		return -1;
	}

	// 첫 만남 안 했으면 다 막기
	if (m_bFirstCinematic == false)
		return -1;

	// 시네마틱 상태면
	if (m_bCinematic)
	{
		m_pTransformCom->Move_Forward(fTimeDelta);
		m_pModelCom->Play_Animation_CS(fTimeDelta);
		return 0;
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

	//_uint iCurState = m_pStateMachineCom->Get_CurStateType();
	//COUT("현재 상태:" << iCurState);
	//_float fStamina = m_pYakumoStatCom->Get_Stamina();
	//COUT("스태미나 : " <<fStamina);
	//_int iAreaCount = m_tAnimRunTimeEvent.iInputAreaCount;
	//COUT("아리아 카운트 : " << iAreaCount);
	//COUT("힐 카운트:" << m_iGiftHealCount);
	//COUT("플레이어 사이즈 카운트 : " << m_dequePlayerPosition.size());

	Update_AITarget();
	Update_SavePlayerPos();

	Move_Smooth(fTimeDelta);
	Update_Rotation(fTimeDelta);
	m_pStateMachineCom->Update_State(fTimeDelta);	// 상태머신 업데이트

	m_pModelCom->Play_Animation_CS(fTimeDelta);

	__super::Update(fTimeDelta);

	Update_WeaponPosition(); // 무기 본 월드 위치 갱신
	Process_AttackSweep_Capsule(); //피직스 공격 충돌
	Update_Trail();  //트레일 업데이트

#ifdef _DEBUG
	Add_Debug_Controller_Capsule(fTimeDelta);
	if (m_bAttackSweepActive)
	{
		Add_Debug_WeaponCapsule();
	}
	Add_Debug_LookLine();
#endif
	return 0;
}

_int Client::Yakumo::Update_Late(const _float fTimeDelta)
{
	if (!m_bIsActive)
		return -1;

	if (m_bFirstCinematic == false)
		return -1;

	// 시네마틱 상태면
	if (m_bCinematic)
	{
		goto cinematic;
	}

	if (m_fEvaluateCoolTime > 0.f)
		m_fEvaluateCoolTime -= fTimeDelta;

	if (m_bPendingTeleport)
	{
		m_fTeleportDelay -= fTimeDelta;
		if (m_fTeleportDelay <= 0.f)
		{
			m_bPendingTeleport = false;

			if (Ensure_PlayerCached())
			{
				Teleport_ToPlayer();
			}
		}

	}

	Update_Stamina(fTimeDelta);
	Update_HealToPlayer();
	Update_TeleportToPlayer();

	cinematic:

	__super::Update_Late(fTimeDelta);

	Add_RenderGroup(RENDER_GROUP::NONBLEND);
	Add_RenderGroup(RENDER_GROUP::SHADOW);

	return 0;
}

HRESULT Client::Yakumo::Render(const _float fTimeDelta)
{
	if (!m_bIsActive)
		return -1;

	CHECK_FAILED(Bind_ShaderResources(), E_FAIL);

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

HRESULT Client::Yakumo::Render_Shadow(const _float fTimeDelta, _int iCascadeNum)
{


	return S_OK;
}

void Client::Yakumo::Set_Position_ByController(_fvector vWorldPos)
{
	if(m_pController)
		m_pController->setFootPosition(ToPxExtendedVec3(vWorldPos));

	m_pTransformCom->Set_State(DIRECTION::POSITION, vWorldPos);
}

void Client::Yakumo::Move_Smooth(const _float fTimeDelta)
{
	if (nullptr == m_pController) return;

	if (!m_bDead)
	{
		_float3 vDir = {};
		XMStoreFloat3(&vDir, m_tAICommandDesc.vMoveDir);

		if (!m_bApplyTranslation)
			m_tAICommandDesc.vMoveDir = XMVectorZero();

		physx::PxVec3 vMoveDir(vDir.x, 0.f, vDir.z);
		if (vMoveDir.magnitudeSquared() > 0.0001f)
			vMoveDir.normalize();

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

		m_fSpeed = 5.f;

		_uint iCurState = m_pStateMachineCom->Get_CurStateType();

		if (iCurState == WALK)
			m_fSpeed = 1.5f;
		if (iCurState == RUN)
			m_fSpeed = 3.5f;
		if (iCurState == DASH)
			m_fSpeed = 5.0f;
		if (iCurState == IDLE)
			m_fSpeed = 0.f;

		physx::PxVec3 vFinalDisplacement(
			vMoveDir.x * m_fSpeed * fTimeDelta + m_vRootMotionDelta.x,
			m_fVerticalVelocity * fTimeDelta + m_vRootMotionDelta.y,
			vMoveDir.z * m_fSpeed * fTimeDelta + m_vRootMotionDelta.z);

		physx::PxControllerFilters filters;
		physx::PxControllerCollisionFlags flags = m_pController->move(
			vFinalDisplacement, 0.0001f, fTimeDelta, filters);

		m_bIsGrounded = (flags & physx::PxControllerCollisionFlag::eCOLLISION_DOWN);

		physx::PxExtendedVec3 pxPos = m_pController->getFootPosition();

		m_pTransformCom->Set_State(DIRECTION::POSITION,
			XMVectorSet((_float)pxPos.x, (_float)pxPos.y + m_fYOffset, (_float)pxPos.z, 1.f));
	}

	// 처치 당했을때는 피직스 전부 꺼버리니까 루트모션만 적용
	m_pTransformCom->Translate(m_vRootMotionDelta);

	//if (m_bDead)
	//{
	//	COUT("야쿠모 X: " << Get_Position_Float3().x);
	//	COUT("야쿠모 Y: " << Get_Position_Float3().y);
	//	COUT("야쿠모 Z: " << Get_Position_Float3().z);
	//}
}

void Client::Yakumo::Update_Rotation(const _float fTimeDelta)
{
	_vector vDir = {};

	//전투, 타겟있을때는 타겟방향으로
	if (m_tAITargetInfo.bCombat && m_tAITargetInfo.pBattleTarget != nullptr)
		vDir = m_tAITargetInfo.vDirToTarget;
	else//비전투
		vDir = m_tAICommandDesc.vMoveDir; 

	if (XMVectorGetX(XMVector3LengthSq(vDir)) < 0.0001f)
		return;

	vDir = XMVectorSetY(vDir, 0.f);
	vDir = XMVector3Normalize(vDir);

	m_pTransformCom->LookDir_Smooth(vDir, m_pTransformCom->Get_RotationSpeed(), fTimeDelta);

}

void Client::Yakumo::OnDamaged(const DAMAGE_EVENT& DamageEvent)
{
	COUT("야쿠모 피격 받음");

	if (m_pYakumoStatCom->Get_Hp() <= 0.f)
	{
		COUT("야쿠모 죽어야함");
		//여기서 피격 상태에서 디졸브,비활성화 처리
		m_pGameInstance->PhysX_Disable_Controller(m_pController);
		m_bDead = true;
	}

	if (m_tAnimRunTimeEvent.bCanInvincible())
	{
		//야쿠모는 집중상태 안넣는다 했으므로
		COUT("야쿠모 무적상태");
		return;
	}

	if (m_tAnimRunTimeEvent.bCanSuperArmor()) //슈퍼아머는 데미지 적용 o 피격 애니메이션 재생 x 
	{
		m_pYakumoStatCom->Apply_Damage((_int)RandomDamage(DamageEvent.fDamage));
		COUT("야쿠모 슈퍼아머상태");

		if (m_pYakumoStatCom->Get_Hp() <= 0.f)			// 슈퍼아머여도 HP 0이면 피격 상태로 전환 
		{
			m_pGameInstance->PhysX_Disable_Controller(m_pController);
			m_bDead = true;
			if (m_eEnemyDamagePower == DAMAGEPOWER::HEAVYPOWER || m_eEnemyDamagePower == DAMAGEPOWER::SPECIALPOWER)
				m_pStateMachineCom->Change_State(DAMAGEBLOW);
			else
				m_pStateMachineCom->Change_State(DAMAGE);
		}
		return;

	}
	else
	{
		m_fLoseHpRatio = m_pYakumoStatCom->ApplyDamageAndGetLoseHpRatio((_int)RandomDamage(DamageEvent.fDamage)); //플레이어 Hp 데미지 적용 
	}

	if (m_pYakumoStatCom->Get_Hp() <= 0.f)
	{
		m_pGameInstance->PhysX_Disable_Controller(m_pController);
		m_bDead = true;
	}

	if (m_eEnemyDamagePower == DAMAGEPOWER::HEAVYPOWER || m_eEnemyDamagePower == DAMAGEPOWER::SPECIALPOWER)
		m_pStateMachineCom->Change_State(DAMAGEBLOW);
	else
		m_pStateMachineCom->Change_State(DAMAGE);


}

void Client::Yakumo::Apply_AnimEventCount(const PLAYERANIM_EVENT& PlayerAnimEvent, _int iSign)
{
	auto AddClamp = [](_int& iValue, _int iDelta)
		{
			iValue += iDelta;
			if (iValue < 0) iValue = 0;
		};

	if (PlayerAnimEvent.bInputArea) AddClamp(m_tAnimRunTimeEvent.iInputAreaCount, iSign);
	if (PlayerAnimEvent.bCanCombo) AddClamp(m_tAnimRunTimeEvent.iCanComboCount, iSign);
	if (PlayerAnimEvent.bCanEscape) AddClamp(m_tAnimRunTimeEvent.iCanEscapeCount, iSign);
	if (PlayerAnimEvent.bCanMove) AddClamp(m_tAnimRunTimeEvent.iCanMoveCount, iSign);
	if (PlayerAnimEvent.bInvincible) AddClamp(m_tAnimRunTimeEvent.iCanInvincible, iSign);
	if (PlayerAnimEvent.bSuperArmor) AddClamp(m_tAnimRunTimeEvent.iCanSuperArmorCount, iSign);
}

void Client::Yakumo::Update_AnimationSpeed(const PLAYERANIM_EVENT& PlayerAnimEvent)
{
	if (PlayerAnimEvent.bControlAnimSpeed)
	{
		m_pModelCom->Set_SingleAnimation_Speed(PlayerAnimEvent.fAnimationSpeed);
	}

}

void Client::Yakumo::Update_Stamina(_float fTimeDelta)
{
	_uint iCurrentStateType = m_pStateMachineCom->Get_CurStateType();

	if (iCurrentStateType == ATTACKSTRONG || iCurrentStateType == ATTACKNORMAL || iCurrentStateType == ROLL || iCurrentStateType == BACKSTEP
		|| iCurrentStateType == ATTACKDODGEF || iCurrentStateType == ATTACKDODGEB || iCurrentStateType == SPECIALATTACK)
		return;

	m_pYakumoStatCom->Regen_Stamina(m_fRegenPerSecond, fTimeDelta);

}

void Client::Yakumo::Create_Projectile(const PLAYERANIM_EVENT& PlayerAnimEvent)
{
	State* pCurState = m_pStateMachineCom->Get_CurrentState();
	if (pCurState == nullptr)
		return;

	if (PlayerAnimEvent.bCreateProjectile)
		pCurState->On_ProjectileEvent();
}

void Client::Yakumo::Update_HealToPlayer()
{
	if (m_bDead)
		return;

	if (m_bProjectileFlying)
		return;

	if (m_iGiftHealCount == m_iMaxGiftHealCount)
		return;

	if (!Ensure_PlayerCached())
		return;

	_float fPlayerHpRatio = m_pPlayer->Get_PlayerStatCom()->Get_Hp() / m_pPlayer->Get_PlayerStatCom()->Get_MaxHp();

	if (fPlayerHpRatio <= 0.3f)
	{
		m_bProjectileFlying = true;
		++m_iGiftHealCount;
		m_pStateMachineCom->Change_State(GIFTHEAL);
	}

}

void Client::Yakumo::Update_SavePlayerPos()
{
	//약간의 최적화는 플레이어의 위치를 일정 개수 넘기면 앞에있는거부터 날려버리고
	//플레이어의 위치를 담고 야쿠모의 이동방향벡터를 수정하고
	//이때 위치는 제일 최신꺼 back으로 가져오고 되지않을까 
	//만약 전투중이면 리셋해버리고
	if (!Ensure_PlayerCached())
		return;

	if (m_tAITargetInfo.bCombat) //전투중이면
	{
		m_dequePlayerPosition.clear(); //다 날려버리고
		return;
	}

	_vector vPlayerPos = m_pPlayer->Get_Position();

	if (!m_dequePlayerPosition.empty()) //비어있지않을때
	{
		_vector vLastPlayerPos = m_dequePlayerPosition.back(); //마지막(제일 최신) 위치를 가져와서
		_float fDistSq = XMVectorGetX(XMVector3LengthSq(vPlayerPos - vLastPlayerPos)); //현재 플레이어 위치 와 마지막 위치의 길이를 가져와서

		if (fDistSq < 1.5f * 1.5f) //설정한 범위보다 작으면 넣지않게
			return;
	}

	m_dequePlayerPosition.push_back(vPlayerPos);

	if (m_dequePlayerPosition.size() > 20) //사이즈 20개 넘으면
		m_dequePlayerPosition.pop_front(); //제일 앞에있는거부터 삭제시켜서 시간 지난것들 날려버리고

}

void Client::Yakumo::Update_TeleportToPlayer()
{
	//비전투시에만 텔레포트 하도록 
	if (m_tAITargetInfo.bCombat || m_tAITargetInfo.pBattleTarget != nullptr)
		return;

	if (!Ensure_PlayerCached())
		return;

	if (m_bDead)
		return;

	if (m_tAITargetInfo.fDistToPlayer > m_fMaxPlayerDist)
	{
		Teleport_ToPlayer();
	}

}

Yakumo::FOUR_DIR Client::Yakumo::Calculate_FourDir(_fvector vCurrent, _fvector vDesired)
{
	_vector current = XMVectorSet(XMVectorGetX(vCurrent), 0.f, XMVectorGetZ(vCurrent), 0.f);
	_vector desired = XMVectorSet(XMVectorGetX(vDesired), 0.f, XMVectorGetZ(vDesired), 0.f);

	_float fDot = XMVectorGetX(XMVector3Dot(current, XMVector3Normalize(desired))); //이 내적을 통해서 1이면 서로 같은 방향 -1이면 반대방향 ,cos

	_vector vCross = XMVector3Cross(current, XMVector3Normalize(desired));
	_float fCrossY = XMVectorGetY(vCross);

	Yakumo::FOUR_DIR Dir = {};

	if (fDot > 0.75f) // Forward
	{
		Dir = Yakumo::FOUR_DIR::FRONT;
	}
	else if (fDot < -0.75f) // Back
	{
		Dir = Yakumo::FOUR_DIR::BACK;
	}
	else if (fCrossY > 0.f) //right  기준
	{
		Dir = Yakumo::FOUR_DIR::RIGHT;
	}
	else if (fCrossY < 0.f) //left 기준
	{
		Dir = Yakumo::FOUR_DIR::LEFT;
	}

	return Dir;
}

void Client::Yakumo::Enter_DeadState()
{
	Set_Active(false); //비활성화
	m_iGiftHealCount = 0;
	m_tAICommandDesc.Reset();
	m_tAITargetInfo.bCombat = false;
	m_tAITargetInfo.pBattleTarget = nullptr;
	m_fEvaluateCoolTime = 0.f;
	m_dequePlayerPosition.clear();

	//그리고 동행자 스폰시에 Set_active()시켜야함 (동행자는 사라질떄도 있으니ㅏㄲ 스폰할때마다 함수만들어서 이거호출해야함)
	UI_MasterEvent Event;
	Event.m_EventTarget = UI_EVENT_TARGET::SYSTEM;
	Event.m_bFlag = false;		//끄고싶으면 false값 넘겨주기
	Event.m_bPersistent = true;
	Event.m_ActionName = "ActiveUI";
	Event.m_Text = L"Buddy_Statusbar";

	m_pGameInstance->Publish(Event);
}

void Client::Yakumo::Enter_AppearSetting()
{
	if (Is_Active())
	{
		m_pYakumoStatCom->Heal_Hp(m_pYakumoStatCom->Get_MaxHp());
		m_pYakumoStatCom->FullRestore_Stamina();
		return;
	}
	else
	{

		Set_Active(true); //활성화
		// 나타나기
		m_bDissolving = true;
		m_fDissolveDir = -1.f;
		m_fTimeElapsed = m_fDissolveMax;

		m_pStateMachineCom->Change_State(IDLE);
		m_iGiftHealCount = 0;
		m_tAICommandDesc.Reset();
		m_tAITargetInfo.bCombat = false;
		m_tAITargetInfo.pBattleTarget = nullptr;
		m_pYakumoStatCom->Heal_Hp(m_pYakumoStatCom->Get_MaxHp()); //체력도 초기화해주고
		m_fEvaluateCoolTime = 0.f;
		m_dequePlayerPosition.clear();
		m_bProjectileFlying = false;
		m_bDead = false;
		m_bAttackSweepActive = false;

		Reset_RunTimeAnimEventDesc();

		if (!Ensure_PlayerCached())
			return;

		
		_vector vPlayerPos = m_pPlayer->Get_Position();
		_vector vPlayerLook = m_pPlayer->Get_Transform()->Get_State(DIRECTION::LOOK);
		vPlayerLook = XMVectorSetY(vPlayerLook, 0.f);
		vPlayerLook = XMVector3Normalize(vPlayerLook);
		_vector vTeleportPos = vPlayerPos - vPlayerLook * m_fAppearDist;

		m_pGameInstance->PhysX_Enable_Controller(m_pController, ToPxExtendedVec3(vTeleportPos), PHYSX_ACTOR_TYPE::COMPANION);
		m_pTransformCom->Set_State(DIRECTION::POSITION, vTeleportPos);


		//그리고 동행자 스폰시에 Set_active()시켜야함 (동행자는 사라질떄도 있으니ㅏㄲ 스폰할때마다 함수만들어서 이거호출해야함)
		UI_MasterEvent Event;
		Event.m_EventTarget = UI_EVENT_TARGET::SYSTEM;
		Event.m_bFlag = true;		//끄고싶으면 false값 넘겨주기
		Event.m_bPersistent = true;
		Event.m_ActionName = "ActiveUI";
		Event.m_Text = L"Buddy_Statusbar";
		m_pGameInstance->Publish(Event);
	}

}

void Client::Yakumo::Update_Trail()
{
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
}

_bool Client::Yakumo::Ensure_PlayerCached()
{
	// 플레이어 등록 + null 체크 통합
	if (nullptr == m_pPlayer)
		m_pPlayer = CAST(Player*)(m_pGameInstance->Get_Player());

	return (nullptr != m_pPlayer);
}

void Client::Yakumo::Reset_AICommand()
{
	m_tAICommandDesc.Reset();
}

void Client::Yakumo::Update_AITarget()
{
	if (!Ensure_PlayerCached())
		return;

	// 플레이어와의 거리,방향 갱신
	_vector vToPlayer = m_pPlayer->Get_Position() - Get_Position();
	vToPlayer = XMVectorSetY(vToPlayer, 0.f);

	_float fDistSq = XMVectorGetX(XMVector3LengthSq(vToPlayer));
	m_tAITargetInfo.fDistToPlayer = sqrtf(fDistSq);

	if (fDistSq > 0.0001f)
		m_tAITargetInfo.vDirToPlayer = XMVector3Normalize(vToPlayer);
	else
		m_tAITargetInfo.vDirToPlayer = XMVectorZero();

	//우선 보스몬스터에서만 활용할 계획이긴하지만 혹시 모르니 일반 몬스터들도
	
	//락온일때 첫순위로 두고
	if (m_pPlayer->Get_PlayerInputDesc().bLockOn && m_pPlayer->Get_LockOnTarget() != nullptr)
	{
		m_tAITargetInfo.pBattleTarget = m_pPlayer->Get_LockOnTarget();
		m_tAITargetInfo.bCombat = true;
	}
	else //여기서 락온 아닐때 일반 몬스터들 탐색해주고
	{
		if(m_tAITargetInfo.pBattleTarget==nullptr) //타겟없을때만 몬스터들 탐색
		Find_AITarget();
	}

	if (m_tAITargetInfo.bCombat && m_tAITargetInfo.pBattleTarget != nullptr) //전투
	{
		Monster* pMonster = dynamic_cast<Monster*>(m_tAITargetInfo.pBattleTarget);
		if (pMonster != nullptr && pMonster->Is_Killed()) //몬스터 죽었으면 초기화
		{
			m_tAITargetInfo.pBattleTarget = nullptr;
			m_tAITargetInfo.bCombat = false;
			m_tAITargetInfo.fDistToTarget = 0.f;
			m_tAITargetInfo.vDirToTarget = XMVectorZero();
			return;
		}

		_float fYDifference = fabsf(XMVectorGetY(m_tAITargetInfo.pBattleTarget->Get_Position()) - XMVectorGetY(Get_Position()));
		if (fYDifference > m_fMaxYDifferece)
		{
			m_tAITargetInfo.pBattleTarget = nullptr;
			m_tAITargetInfo.bCombat = false;
			m_tAITargetInfo.fDistToTarget = 0.f;
			m_tAITargetInfo.vDirToTarget = XMVectorZero();
			return;
		}

		_float fTargetRadius = pMonster->Get_ControllerRadius();
		m_fAdjustAttackRange = m_fAttackRange + fTargetRadius;
		m_fAdjustDodgeRange = m_fDodgeRange + fTargetRadius;
		m_fAdjustFrontRange = m_fFrontRange + fTargetRadius;
		m_fAdjustApproachRange = m_fApproachRange + fTargetRadius;

		//몬스터 거리,방향 갱신
		_vector vToTarget = m_tAITargetInfo.pBattleTarget->Get_Position() - Get_Position();
		vToTarget = XMVectorSetY(vToTarget, 0.f);

		_float fTargetDistSq = XMVectorGetX(XMVector3LengthSq(vToTarget));
		m_tAITargetInfo.fDistToTarget = sqrtf(fTargetDistSq);

		if (fTargetDistSq > 0.0001f)
			m_tAITargetInfo.vDirToTarget = XMVector3Normalize(vToTarget);
		else
			m_tAITargetInfo.vDirToTarget = XMVectorZero();
	}
}

void Client::Yakumo::Find_AITarget()
{
	Layer* pMonsterLayer = m_pGameInstance->Get_Layer(L"Layer_Monster");
	if (pMonsterLayer == nullptr)
		return;

	vector<GameObject*>& vecGameObjects = pMonsterLayer->Get_VecGameObjects();

	_float fBestDistSq = m_fDetectRangeSq; //탐지범위 제한
	GameObject* pBestTarget = nullptr; 

	_vector vMyPos = Get_Position();

	for (auto& pGameObject : vecGameObjects)
	{
		if (pGameObject->Is_Active() == false)
			continue;

		Monster* pMonster = dynamic_cast<Monster*>(pGameObject);
		if (pMonster == nullptr)
			continue;

		if (pMonster->Is_Killed())
			continue;

		_vector vDir = pMonster->Get_Position() - vMyPos;
	
		if (XMVectorGetY(vDir) > m_fMaxYDifferece)
		{
			continue;
		}

		vDir = XMVectorSetY(vDir, 0.f);

		_float fDistSq = XMVectorGetX(XMVector3LengthSq(vDir));

		if (fDistSq < fBestDistSq)
		{
			fBestDistSq = fDistSq;
			pBestTarget = pMonster;
		}

	}

	if (pBestTarget != nullptr)
	{
		m_tAITargetInfo.pBattleTarget = pBestTarget;
		m_tAITargetInfo.bCombat = true;
	}

}

void Client::Yakumo::Teleport_ToPlayer()
{
	_vector vPlayerPos = m_pPlayer->Get_Position();
	_vector vPlayerLook = m_pPlayer->Get_Transform()->Get_State(DIRECTION::LOOK);
	vPlayerLook = XMVectorSetY(vPlayerLook, 0.f);
	vPlayerLook = XMVector3Normalize(vPlayerLook);
	_vector vTeleportPos = vPlayerPos - vPlayerLook * m_fAppearDist;

	Set_Position_ByController(vTeleportPos);
	m_dequePlayerPosition.clear();

}

void Client::Yakumo::Evaluate_AI()
{
	if (m_tAICommandDesc.bHasCommand) //예약 있으면 return 
		return;

	if (m_fEvaluateCoolTime > 0.f) //쿨타임 안돌았으면 return 
		return;

	if (!m_tAITargetInfo.bCombat) //전투 상태아닐때는 플레이어 따라가기 
	{
		Evaluate_Follow_Player();
		m_fEvaluateCoolTime = 0.1f;
		return;
	}

	if (Evaluate_Dodge())
	{
		m_fEvaluateCoolTime = m_pGameInstance->RandomValue(0.75f, 1.25f);
		return;
	}

	if (Evalute_Attack())
	{
		m_fEvaluateCoolTime = m_pGameInstance->RandomValue(0.75f, 1.25f);
		return;
	}

	if (Evaluate_Approach())
	{
		m_fEvaluateCoolTime = m_pGameInstance->RandomValue(0.8f, 1.0f);
		return;
	}

}

_bool Client::Yakumo::Evaluate_Dodge()
{
	if (m_pYakumoStatCom->Get_Stamina() < 20.f)
		return false;

	if (m_tAITargetInfo.fDistToTarget > m_fAdjustDodgeRange)
		return false;

	_float fDodgeProbability = 10.f; //기본 Base 회피 확률

	_float fHpRatio = m_pYakumoStatCom->Get_Hp() / m_pYakumoStatCom->Get_MaxHp(); 
	//1이 풀피고 낮을 수록 
	if (fHpRatio <= 0.25f)
		fDodgeProbability += 15.f;
	else if (fHpRatio <= 0.5f)
		fDodgeProbability += 10.f;
	else if (fHpRatio <= 0.75f)
		fDodgeProbability += 5.f;

	Monster* pMonster = dynamic_cast<Monster*>(m_tAITargetInfo.pBattleTarget);
	if (pMonster != nullptr && pMonster->Is_Attacking()) //몬스터가 공격중이라면
	{
		_vector vMonsterLook = XMVector3Normalize(pMonster->Get_Transform()->Get_State(DIRECTION::LOOK));
		_vector vMyLook = XMVector3Normalize(m_pTransformCom->Get_State(DIRECTION::LOOK));

		_float fDotForward = XMVectorGetX(XMVector3Dot(vMonsterLook, vMyLook));

		if (fDotForward < -0.5f) // 서로 마주보고 있을때 가중치를 주도록 , 이렇게 안하면 몬스터가 플레이어를 공격할때도 야쿠모의 가중치가 올라가니까
			fDodgeProbability += 25.f;
	}

	fDodgeProbability = min(fDodgeProbability, 50.f); //최대 확률은 50으로 잡아주고

	_float fProbability = m_pGameInstance->RandomValue(0.f, 100.f);
	if (fProbability > fDodgeProbability)
		return false;


	COUT("회피 AI 들어옴");

	_vector vLook = m_tAITargetInfo.vDirToTarget;
	_vector vUp = XMVectorSet(0.f, 1.0f, 0.f, 0.f);
	_vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));

	_float fRollProbability = m_pGameInstance->RandomValue(0.f, 100.f);

	if (fRollProbability < 15.f)
	{
		m_tAICommandDesc.eReserveAction = ROLL;
		m_tAICommandDesc.bHasCommand = true;
		m_tAICommandDesc.vMoveDir = vRight; //우측 
		return true;
	}
	else if (fRollProbability < 30.f)
	{
		m_tAICommandDesc.eReserveAction = ROLL;
		m_tAICommandDesc.bHasCommand = true;
		m_tAICommandDesc.vMoveDir = XMVectorNegate(vRight); //좌측
		return true;
	}
	else if (fRollProbability < 45.f)
	{
		m_tAICommandDesc.eReserveAction = ROLL;
		m_tAICommandDesc.bHasCommand = true;
		m_tAICommandDesc.vMoveDir = XMVectorNegate(m_tAITargetInfo.vDirToTarget); //후방
		return true;
	}
	else if (fRollProbability < 60.f)
	{
		if (m_tAITargetInfo.fDistToTarget > m_fAdjustFrontRange)
		{
			m_tAICommandDesc.eReserveAction = ROLL;
			m_tAICommandDesc.bHasCommand = true;
			m_tAICommandDesc.vMoveDir = m_tAITargetInfo.vDirToTarget;
		}
		else
		{
			m_tAICommandDesc.eReserveAction = ROLL;
			m_tAICommandDesc.bHasCommand = true;
			m_tAICommandDesc.vMoveDir = XMVectorNegate(m_tAITargetInfo.vDirToTarget);
		}
		return true;
	}
	else if(fRollProbability <80.f) //백스탭
	{
		m_tAICommandDesc.eReserveAction = BACKSTEP;
		m_tAICommandDesc.bHasCommand = true;
		m_tAICommandDesc.vMoveDir = m_tAITargetInfo.vDirToTarget; //전방
		return true;
	}
	else //나머지 확률은 공격으로
	{
		return false;
	}

	return true;
}

_bool Client::Yakumo::Evalute_Attack()
{
	if (m_tAITargetInfo.fDistToTarget > m_fAdjustAttackRange)
		return false;

	if (m_pYakumoStatCom->Get_Stamina() < 30.f)
		return false;

	COUT("공격 AI 들어옴");

	//여기서 플레이어 상태 가져오고
	_float fPlayerAgrroValue = 1.f; //플레이어 어그로 값으로 데미지 받고 있으면 높게 , 플레이어가 공격중면 약하게
	_uint iPlayerCurState = m_pPlayer->Get_PlayerStateMachine()->Get_CurStateType();
	if (iPlayerCurState == Player::DAMAGE || iPlayerCurState == Player::DAMAGEBLOW)
		fPlayerAgrroValue = 2.f;
	else if (iPlayerCurState == Player::ATTACKNORMAL || iPlayerCurState == Player::ATTACKSTRONG || iPlayerCurState == Player::SPECIALATTACK ||
		iPlayerCurState == Player::ATTACKDODGEF || iPlayerCurState == Player::ATTACKDODGEB)
		fPlayerAgrroValue = 0.75f;

	_float fBaseProbability = 80.0f; //기본 확률
	_float fFinalProbability = min(fBaseProbability * fPlayerAgrroValue, 90.f); //최종확률로 최댓값은 90

	_float iProbability = m_pGameInstance->RandomValue(0.f, 100.f); 

	if (iProbability > fFinalProbability) 
		return false;

	_float fAttackProbability = m_pGameInstance->RandomValue(0.f, 100.f); //무슨 공격 할건지 확률

	if (fAttackProbability < 45.f)
	{
		m_tAICommandDesc.eReserveAction = ATTACKNORMAL;
	}
	else if (fAttackProbability < 75.f)
	{
		m_tAICommandDesc.eReserveAction = ATTACKSTRONG;
		m_tAICommandDesc.iProbability = m_pGameInstance->RandomValue_int(0, 100) < 50 ? 0 : 1;
	}
	else
	{
		m_tAICommandDesc.eReserveAction = SPECIALATTACK;
		m_tAICommandDesc.iProbability = m_pGameInstance->RandomValue_int(0, 100) < 50 ? 0 : 1;
	}

	m_tAICommandDesc.bHasCommand = true;
	m_tAICommandDesc.vMoveDir = m_tAITargetInfo.vDirToTarget;

	return true;
}

_bool Client::Yakumo::Evaluate_Approach()
{
	if (m_tAITargetInfo.pBattleTarget == nullptr)
		return false;

	if (m_tAITargetInfo.fDistToTarget < m_fAdjustAttackRange) //공격범위안에 있으면 그래도 접근하는것보다는 다시 회피 또는 공격을 하는게 (false)보다는 그냥 차라리 공격AI를 호출하는게 자연스럽나
		return false;

	COUT("접근 AI 들어옴");

	if (m_tAITargetInfo.fDistToTarget > m_fAdjustApproachRange) //전방으로 직진만
	{
		_float fMoveProbability = m_pGameInstance->RandomValue(0.f, 100.f);

		if (fMoveProbability < 50.f) 
			m_tAICommandDesc.eReserveAction = DASH;
		else
			m_tAICommandDesc.eReserveAction = RUN;

		m_tAICommandDesc.bHasCommand = true;
		m_tAICommandDesc.vMoveDir = m_tAITargetInfo.vDirToTarget;
		return true;

	}

	_float fMoveProbability = m_pGameInstance->RandomValue(0.f, 100.f);

	if (fMoveProbability < 70.f)
		m_tAICommandDesc.eReserveAction = RUN;
	else
		m_tAICommandDesc.eReserveAction = WALK;

	_vector vLook = m_tAITargetInfo.vDirToTarget;
	_vector vUp = XMVectorSet(0.f, 1.0f, 0.f, 0.f);
	_vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));

	_float fProbability = m_pGameInstance->RandomValue(0.f, 100.f);

	if (fProbability < 33.0f) //우측
		m_tAICommandDesc.vMoveDir = vRight;
	else if (fProbability < 66.0f)//좌측
		m_tAICommandDesc.vMoveDir = XMVectorNegate(vRight);
	else if (fProbability < 90.0f) //뒤로
		m_tAICommandDesc.vMoveDir = XMVectorNegate(m_tAITargetInfo.vDirToTarget);
	else //제자리
	{
		m_tAICommandDesc.eReserveAction = IDLE;
		m_tAICommandDesc.vMoveDir = XMVectorZero();
	}


	m_tAICommandDesc.bHasCommand = true;

	return true;
}

Yakumo::FOUR_DIR Client::Yakumo::Calculate_FourMoveDir(_fvector vMoveDir)
{
	//여기서 이제 4방향을 구해줘야겠지

	_vector vLook = m_tAITargetInfo.vDirToTarget;
	_vector vUp = XMVectorSet(0.f, 1.0f, 0.f, 0.f);
	_vector vRight = XMVector3Normalize(XMVector3Cross(vUp, vLook));

	_float fDotFoward = XMVectorGetX(XMVector3Dot(vMoveDir, vLook)); // 정면,반대 
	_float fDotRight = XMVectorGetX(XMVector3Dot(vMoveDir, vRight)); //우측 , 좌측 

	if (fDotFoward > 0.75f) return FOUR_DIR::FRONT;
	if (fDotFoward < -0.75f) return FOUR_DIR::BACK;
	if (fDotRight > 0.f) return FOUR_DIR::RIGHT;
	return FOUR_DIR::LEFT;
}

void Client::Yakumo::Evaluate_Follow_Player()
{
	//여기서 vMoveDir을 바꿔줘야하나
	_vector vTargetPos = {};
	_float fDistToTarget = {};

	if (!m_dequePlayerPosition.empty())//비어있지않을때 1순위 
	{
		_vector vFrontPos = m_dequePlayerPosition.front();
		_float fFrontToPlayer = XMVectorGetX(XMVector3Length(
			XMVectorSetY(m_pPlayer->Get_Position() - vFrontPos, 0.f)));
		_float fMeToPlayer = m_tAITargetInfo.fDistToPlayer;

		if (fMeToPlayer < fFrontToPlayer)
		{
			m_dequePlayerPosition.pop_front();

			while (!m_dequePlayerPosition.empty())
			{
				vFrontPos = m_dequePlayerPosition.front();
				fFrontToPlayer = XMVectorGetX(XMVector3Length(
					XMVectorSetY(m_pPlayer->Get_Position() - vFrontPos, 0.f)));
				if (fMeToPlayer >= fFrontToPlayer)
					break;
				m_dequePlayerPosition.pop_front();
			}
		}

		if (m_dequePlayerPosition.empty())
		{
			fDistToTarget = m_tAITargetInfo.fDistToPlayer;
			vTargetPos = m_pPlayer->Get_Position();
		}
		else
		{
			vTargetPos = m_dequePlayerPosition.front();//제일 앞에꺼 가져와서 

			_vector vDirToTarget = vTargetPos - m_pTransformCom->Get_State(DIRECTION::POSITION);
			vDirToTarget = XMVectorSetY(vDirToTarget, 0.f);
			fDistToTarget = XMVectorGetX(XMVector3Length(vDirToTarget));

			if (fDistToTarget < 4.0f) //제일 앞에꺼와 야쿠모의 거리가 0.5f보다 가까워지면 도착했다고 판정
			{
				m_dequePlayerPosition.pop_front();//이제 빼주고

				if (m_dequePlayerPosition.empty()) //빼고나서 비었으면
				{
					fDistToTarget = m_tAITargetInfo.fDistToPlayer; //원래 플레이어 쫓아가고 
					vTargetPos = m_pPlayer->Get_Position();
				}
				else //비어있지않으면
				{
					vTargetPos = m_dequePlayerPosition.front(); //다시 빼와서
					_vector vToNext = vTargetPos - Get_Position(); //다음 가야할 위치 세팅
					vToNext = XMVectorSetY(vToNext, 0.f);
					fDistToTarget = XMVectorGetX(XMVector3Length(vToNext));
				}
			}

		}
	}
	else //원래 계산
	{
		fDistToTarget = m_tAITargetInfo.fDistToPlayer;
		vTargetPos = m_pPlayer->Get_Position();
	}

	_vector vDir = vTargetPos - Get_Position();
	vDir = XMVectorSetY(vDir, 0.f);
	if (XMVectorGetX(XMVector3LengthSq(vDir)) > 0.0001f)
		vDir = XMVector3Normalize(vDir);
	else
		vDir = XMVectorZero();

	if (m_tAITargetInfo.fDistToPlayer > m_fDashRange)
	{
		m_tAICommandDesc.bHasCommand = true;
		m_tAICommandDesc.eReserveAction = DASH;
		m_tAICommandDesc.vMoveDir = vDir;
		return;
	}
	else if (m_tAITargetInfo.fDistToPlayer > m_fRunRange)
	{
		m_tAICommandDesc.bHasCommand = true;
		m_tAICommandDesc.eReserveAction = RUN;
		m_tAICommandDesc.vMoveDir = vDir;
		return;
	}
	else if (m_tAITargetInfo.fDistToPlayer > m_fWalkRange)
	{
		m_tAICommandDesc.bHasCommand = true;
		m_tAICommandDesc.eReserveAction = WALK;
		m_tAICommandDesc.vMoveDir = vDir;
		return;
	}
	else 
	{
		m_tAICommandDesc.bHasCommand = true;
		m_tAICommandDesc.eReserveAction = IDLE;
		m_tAICommandDesc.vMoveDir = XMVectorZero();
	}


}

void Client::Yakumo::Set_Animation(_uint iAnimationIndex, _bool bIsLoop, _float fLerpDuration, _float fAnimationSpeed)
{
	m_pModelCom->Set_Animation_CS(iAnimationIndex, bIsLoop, fLerpDuration, fAnimationSpeed);
	Reset_RunTimeAnimEventDesc(); //이벤트 값 초기화
	Reset_AICommand(); //상태 바뀌었으므로 AI 예약 행동 리셋
}

HRESULT Client::Yakumo::Ready_Components()
{
	CHECK_FAILED(Add_Shader(Proto_Com_Shader_VTXAnimMesh), E_FAIL);	// 쉐이더

	RESETPOSE_DESC ModelDesc;

	ModelDesc.wstrFilePath = L"../../Resources/Models/OverrideModel/YakumoBase.siho";
	ModelDesc.vecTargetBoneNames =
	{
		"Face_Jaw",
		"Face_LeftBrow1",
		"Face_LeftBrow2",
		"Face_LeftBrow3",
		"Face_LeftUpLid1",
		"Face_LeftUpLid2",
		"Face_LeftUpLid3",
		"Face_LeftLowLid",
		"Face_RightBrow1",
		"Face_RightBrow2",
		"Face_RightBrow3",
		"Face_RightUpLid1",
		"Face_RightUpLid2",
		"Face_RightUpLid3",
		"Face_RightLowLid",
		"Face_UpRightRip",
		"Face_UpTooth",
		"Face_UpLeftRip",
		"Face_LowRightRip",
		"Face_UpCenterRip",
		"Face_LowCenterRip",
		"Face_LowLeftRip",
		"Face_LowLeftRip",
		"Face_RightSideRip",
		"Face_LeftSideRip",
		"Face_LowTooth",
		"Face_Tang1",
		"Face_Tang2",
		"Face_Tang3",
		"Face_Tang4",
	};


	MSG_FAIL(Add_Component(m_iLevel, Proto_Model(L"Yakumo"), Com_Model, RCAST(Component**)(&m_pModelCom),&ModelDesc), L"모델 추가 실패!", L"실패!!!", E_FAIL);
	m_pModelCom->Set_OwnerId(m_iObjectID);
	
	CHECK_FAILED(Add_Component(m_iLevel, Proto_Com_StateMachine, Com_StateMachine, RCAST(Component**)(&m_pStateMachineCom)), E_FAIL);

	Player_Stat::PLAYERSTAT_DESC YakumoStatDesc;
	YakumoStatDesc.eBloodCode = BLOODCODE::BLOODCODE_END;
	YakumoStatDesc.fMaxHp = 1350.f;
	YakumoStatDesc.fCurrentHp = YakumoStatDesc.fMaxHp;
	YakumoStatDesc.iAttack = 100;
	YakumoStatDesc.iDefense = 20;
	YakumoStatDesc.iMaxMyeonghyeol = 20.f;
	YakumoStatDesc.iCurrentMyeonghyeol = YakumoStatDesc.iMaxMyeonghyeol;
	YakumoStatDesc.fMaxStamina = 250;
	YakumoStatDesc.fCurrentStamina = YakumoStatDesc.fMaxStamina;
	//포커스 게이지 우선 사용안할 예정이라 했으므로 주석
	//YakumoStatDesc.fMaxFocusGauge = 120;
	//YakumoStatDesc.fCurrentFocusGauge = 0;

	//공유하는 스탯컴포넌트 중 플레이어 임을 표시
	YakumoStatDesc.m_bPlayer = false;

	CHECK_FAILED(Add_Component(m_iLevel, Proto_Com_PlayerStat, Com_Stat, RCAST(Component**)(&m_pStatCom), &YakumoStatDesc), E_FAIL);

	m_pYakumoStatCom = dynamic_cast<Player_Stat*>(m_pStatCom);
	if (m_pYakumoStatCom == nullptr)
		return E_FAIL;

	Safe_AddRef(m_pYakumoStatCom);

	m_pYakumoStatCom->Get_MaxHp();

	// Dissolve용 텍스처
	CHECK_FAILED(Add_Component(_UINT(LEVEL::STATIC), Proto_Texture("Dissolve_Player"),
		L"Com_DissolveTexture", RCAST(Component**)(&m_pNoiseTextureCom)), E_FAIL);

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();
	Set_VecObjPassSize(iNumMeshes);
	Set_AllPass_VecObjPass(8);

	Set_Pass_VecObjPass(0, 6);
	Set_Pass_VecObjPass(1, 6);

	return S_OK;
}

HRESULT Client::Yakumo::Bind_ShaderResources()
{
	m_pTransformCom->Bind_WorldMatrix_ByHandle(m_pShaderCom, g_WorldMatrix);

	// Noise Texture
	_uint bUseNoise = (m_fTimeElapsed > 0.f) ? 1 : 0;
	CHECK_FAILED(m_pShaderCom->Bind_RawValue_ByHandle(g_bUseNoise, &bUseNoise, sizeof(_uint)), E_FAIL);
	if (bUseNoise)
	{
		_float fNormalized = m_fTimeElapsed / m_fDissolveMax;
		CHECK_FAILED(m_pNoiseTextureCom->Bind_ShaderResource_FullSlot(m_pShaderCom, 12, 0), E_FAIL);
		m_pShaderCom->Bind_RawValue_ByHandle(g_fTimeElapsed, &m_fTimeElapsed, sizeof(_float));
	}

	return S_OK;
}

HRESULT Client::Yakumo::Ready_PartObjects()
{
	//무기
	Yakumo_Weapon::YAKUMOWEAPON_DESC WeaponDesc = {};
	WeaponDesc.pSocketMatrix = m_pModelCom->Get_SocketBoneMatrixPtr_Index("RightHandAttachSocket");
	WeaponDesc.pParentMatrix = m_pTransformCom->Get_WorldMatrixPtr();
	WeaponDesc.pDissolveTime = &m_fTimeElapsed;
	CHECK_FAILED(__super::Add_PartObject(m_iLevel, Proto_GameObject(L"YakumoWeapon"), TEXT("Part_YakumoWeapon"), &WeaponDesc, true), E_FAIL);

	m_pActiveWeapon = dynamic_cast<Yakumo_Weapon*>(Find_PartObject(TEXT("Part_YakumoWeapon")));

	/* 무기 매트릭스 */
	m_pWeaponBoneMatrix = const_cast<_float4x4*>(m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr("TrailStartSocket"));		// 콜라이더용
	m_pWeaponBoneMatrixEnd = const_cast<_float4x4*>(m_pActiveWeapon->Get_Model()->Get_BoneMatrixPtr("TrailEndSocket"));		// 콜라이더용
	m_pLeftHandMatrix = const_cast<_float4x4*>(m_pModelCom->Get_SocketBoneMatrixPtr_Index("LeftHandAttachSocket"));

	return S_OK;
}

HRESULT Client::Yakumo::Ready_States()
{
	//상태
	CHECK_FAILED(m_pStateMachineCom->Add_State(IDLE, Yakumo_Idle::Create(this)), E_FAIL);
	CHECK_FAILED(m_pStateMachineCom->Add_State(WALK, Yakumo_Walk::Create(this)), E_FAIL);
	CHECK_FAILED(m_pStateMachineCom->Add_State(RUN, Yakumo_Run::Create(this)), E_FAIL);
	CHECK_FAILED(m_pStateMachineCom->Add_State(DASH, Yakumo_Dash::Create(this)), E_FAIL);
	//공격
	CHECK_FAILED(m_pStateMachineCom->Add_State(ATTACKNORMAL, Yakumo_AttackNormal::Create(this)), E_FAIL);
	CHECK_FAILED(m_pStateMachineCom->Add_State(ATTACKSTRONG, Yakumo_AttackStrong::Create(this)), E_FAIL);
	CHECK_FAILED(m_pStateMachineCom->Add_State(SPECIALATTACK, Yakumo_SpecialAttack::Create(this)), E_FAIL);
	CHECK_FAILED(m_pStateMachineCom->Add_State(ATTACKDODGEF, Yakumo_AttackDodgeF::Create(this)), E_FAIL);
	CHECK_FAILED(m_pStateMachineCom->Add_State(ATTACKDODGEB, Yakumo_AttackDodgeB::Create(this)), E_FAIL);
	//회피
	CHECK_FAILED(m_pStateMachineCom->Add_State(ROLL, Yakumo_Roll::Create(this)), E_FAIL);
	CHECK_FAILED(m_pStateMachineCom->Add_State(BACKSTEP, Yakumo_BackStep::Create(this)), E_FAIL);
	//피격
	CHECK_FAILED(m_pStateMachineCom->Add_State(DAMAGE, Yakumo_Damage::Create(this)), E_FAIL);
	CHECK_FAILED(m_pStateMachineCom->Add_State(DAMAGEBLOW, Yakumo_DamageBlow::Create(this)), E_FAIL);
	CHECK_FAILED(m_pStateMachineCom->Add_State(DAMAGEEND, Yakumo_DamageEnd::Create(this)), E_FAIL);
	//스킬
	//CHECK_FAILED(m_pStateMachineCom->Add_State(BUDDY, Yakumo_Buddy::Create(this)), E_FAIL); 버디는 플레이어와 뭔가 상호작용하는 스킬이였음
	CHECK_FAILED(m_pStateMachineCom->Add_State(GIFTHEAL, Yakumo_GiftHeal::Create(this)), E_FAIL); 
	CHECK_FAILED(m_pStateMachineCom->Add_State(DEATH, Yakumo_Death::Create(this)), E_FAIL);

	m_pStateMachineCom->Set_State(IDLE);

	return S_OK;
}

HRESULT Client::Yakumo::Ready_Event()
{
	m_iYakumoAnimEventHandle = GameObject::Subscribe_Event<PLAYERANIM_EVENT>(
		[this](const PLAYERANIM_EVENT& Event)
		{
			if (Event.iOwnerId != m_iObjectID)
				return;

			switch (Event.ePhase)
			{
			case ANIM_FRAMEPHASE::START:
				Apply_AnimEventCount(Event, +1);
				Update_AnimationSpeed(Event);
				Create_Projectile(Event);
				break;
			case ANIM_FRAMEPHASE::UPDATE:
				break;
			case ANIM_FRAMEPHASE::END:
				Apply_AnimEventCount(Event, -1);
				break;
			}

		});

	Subscribe_Event<PARTICLE_EVENT>([this](const PARTICLE_EVENT& e) {
		if (e.iOwnerId != m_iObjectID)
			return E_FAIL;

		// 이벤트 매니저에서 ParticleSystem 찾기
		ParticleSystem* pSystem = m_pGameInstance->Find_Effect(stringToWstring(e.ParticleSystemName));
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

							const _float4x4* pSocket = m_pModelCom->Get_SocketBoneMatrixPtr_Index("RightHandAttachSocket");
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
						pMatBone = m_pModelCom->Get_SocketBoneMatrixPtr_Index(socketName);
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
								_float3 vPos;
								vPos.x = XMVectorGetX(matWorld.r[3]);
								vPos.y = XMVectorGetY(matWorld.r[3]);
								vPos.z = XMVectorGetZ(matWorld.r[3]);
								XMStoreFloat4x4(&matFinalPos, XMMatrixTranslation(vPos.x, vPos.y, vPos.z));
							}
						}
					}

					return matFinalPos;
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

	Subscribe_Event<Projectile_ARRIVAL_EVENT>([this](const Projectile_ARRIVAL_EVENT& e) {
		if (e.iOwnerID != m_iObjectID)
			return;

		m_bProjectileFlying = false; //투사체 날아가는 거 끝

		});

	// Dissolve 이벤트 구독
	Subscribe_Event<DISSOLVE_EVENT>([this](const DISSOLVE_EVENT& e) {
		if (e.iOwnerId != m_iObjectID)
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

	Subscribe_Event<YAKUMO_ACTIVE_EVENT>([this](const YAKUMO_ACTIVE_EVENT& e) {

		if (e.bSuccess)
		{
			Enter_AppearSetting();
		}

		});

	Subscribe_Event<YAKUMO_TELEPORT_EVENT>([this](const YAKUMO_TELEPORT_EVENT& e) {
		// 시네마틱 상태면 리턴
		if (m_bCinematic == true || m_bFirstCinematic == false)
			return;

		m_fTeleportDelay = e.fDelay;
		m_bPendingTeleport = true;

		});

	Subscribe_Event<CINEMATIC_CHARACTER_CONTROLL>([this](const CINEMATIC_CHARACTER_CONTROLL& _event)
		{
			m_iCinematicNum = _event.iNumber;

			switch(m_iCinematicNum)
			{
			case YC_OLIVER:
				Cinematic_Oliver();
				break;
			case YC_OLIVER_SPEED:
				Cinematic_Change_Speed();
				break;
			case YC_TP_AGAIN:
				Cinematic_Teleport_Again();
				break;
			case YC_OLIVER_END:
				Cinematic_Oliver_End();
				break;
			}
		}
	);


	return S_OK;
}

HRESULT Client::Yakumo::Ready_PhysXEvent()
{
	m_pModelCom->Set_OwnerId(m_iObjectID);

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


#pragma region 시네마틱
void Client::Yakumo::Cinematic_Oliver()
{
	m_bCinematic = true;
	m_bIsActive = true;
	m_bIsDead = false;
	m_bFirstCinematic = true;

	m_bDissolving = false;
	m_fDissolveDir = 1.f;
	m_fTimeElapsed = 0.f;

	m_pTransformCom->Set_Speed(0.f);

	_vector pos = XMVectorSet(-121.91f, 7.87f, 93.6f, 1.f);
	_float4 rot = { 0.f, -81.5f, 0.f, 0.f };

	Teleport(pos, rot);
	m_pModelCom->Set_Animation_CS(6, true, 0.01f, 1.f);

	m_pGameInstance->Play_Sound("Yakumo_Appeared", 0.5f);
}

void Client::Yakumo::Cinematic_Change_Speed()
{
	m_pTransformCom->Set_Speed(5.f);
}

void Client::Yakumo::Cinematic_Teleport_Again()
{
	m_pTransformCom->Set_Speed(3.f);
	_vector pos = XMVectorSet(-121.91f, 7.87f, 93.6f, 1.f);
	_float4 rot = { 0.f, -81.5f, 0.f, 0.f };

	Teleport(pos, rot);
}

void Client::Yakumo::Cinematic_Oliver_End()
{
	m_bCinematic = false;
	m_bIsActive = true;
	m_bIsDead = false;
	m_tAITargetInfo.pBattleTarget = m_pGameInstance->Get_GameObject(_UINT(LEVEL::MAIN), L"Layer_Monster", L"Boss_Oliver_0");
	m_tAITargetInfo.bCombat = true;
}
#pragma endregion 시네마틱



#ifdef _DEBUG
void Client::Yakumo::Add_Debug_WeaponCapsule(_float _radius, _float3 _color)
{
	GameObject::Add_Debug_Capsule(*m_pWeaponBoneMatrix, *m_pWeaponBoneMatrixEnd, m_fAttackRadius, _color);
}
#endif // _DEBUG



void Client::Yakumo::Update_WeaponPosition()
{
	if (m_pModelCom == nullptr)
		return;

	if (m_pWeaponBoneMatrix == nullptr)
		return;

	m_vPrevWeaponPos = m_vCurrentWeaponPos;
	m_vPrevWeaponPosEnd = m_vCurrentWeaponPosEnd;

	m_vCurrentWeaponPos.x = m_pWeaponBoneMatrix->_41;
	m_vCurrentWeaponPos.y = m_pWeaponBoneMatrix->_42;
	m_vCurrentWeaponPos.z = m_pWeaponBoneMatrix->_43;

	m_vCurrentWeaponPosEnd.x = m_pWeaponBoneMatrixEnd->_41;
	m_vCurrentWeaponPosEnd.y = m_pWeaponBoneMatrixEnd->_42;
	m_vCurrentWeaponPosEnd.z = m_pWeaponBoneMatrixEnd->_43;
}

void Client::Yakumo::Process_AttackSweep_Capsule()
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
		damageEvent.fDamage = m_fAttackDamage;
		damageEvent.vHitPosition = vecHitPositions[i];
		damageEvent.vHitDirection = vAttackDir;
		damageEvent.fKnockbackForce = m_fKnockbackForce;
		damageEvent.eDamagePower = m_pStateMachineCom->Get_CurrentState_DamagePower();
		damageEvent.pDamageOwner = this;
		m_pGameInstance->Publish<DAMAGE_EVENT>(damageEvent);
		//COUT("데미지 :" << damageEvent.fDamage);

		// 카메라 이벤트도 발행
		//CameraEvent shakeevent;
		//shakeevent.eCameraAction = CAMERA_ACTION::SHAKE;
		//shakeevent.tShake = ShakePreset::HitStop_PlayerWeapon();
		//m_pGameInstance->Publish(shakeevent);


		// 타격 이펙트 생성
		ParticleSystem* pEffect = CAST(ParticleSystem*)(PoolingManager::Get_Instance()->Acquire_Effect(POOL_ID::EFFECT_HIT));
		pEffect->Set_WorldPosition(vecHitPositions[i]);
		pEffect->OnSpawn(nullptr);

	}

	if (bPlayedWeaponClash && m_pActiveWeapon != nullptr)
	{
		_float fFinalVoulume = 0.f;
		_float fDist = m_tAITargetInfo.fDistToPlayer;
		_float fRatio = 1.f - clamp(fDist / m_fMaxSoundDist, 0.f, 1.f);
		fFinalVoulume = 0.25f * fRatio;

		m_pGameInstance->Play_Sound("469905387", fFinalVoulume, false);
		m_pGameInstance->Play_Sound_RandomInGroup("Weapon_MonsterClash", fFinalVoulume, false);
	}

}

//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Yakumo* Client::Yakumo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL _level)
{
	Yakumo* pInstance = new Yakumo(pDevice, pContext);

	MSG_FAIL(pInstance->Initialize_Prototype(_level), L"Yakumo 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}

GameObject* Client::Yakumo::Clone(void* pArg)
{
	Yakumo* pInstance = new Yakumo(*this);

	MSG_FAIL(pInstance->Initialize(pArg), L"Yakumo 복사 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/

void Client::Yakumo::Free()
{
	__super::Free();
	if (m_pController)
		m_pController = nullptr;

	Safe_Release(m_pYakumoStatCom);

}