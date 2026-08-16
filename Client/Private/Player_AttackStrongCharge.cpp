#include "Client_Define.h"
#include "Player_AttackStrongCharge.h"
#include "Player_MasterRig.h"
#include "StateMachine.h"
#include "Player.h"
#include "Player_Weapon.h"
#include "PoolingManager.h"
#include "Bayonet_Bullet.h"
#include "Player_Stat.h"

Client::Player_AttackStrongCharge::Player_AttackStrongCharge()
{
}

HRESULT Client::Player_AttackStrongCharge::Initialize(GameObject* pOwner, Player_MasterRig* pPlayer_MasterRig)
{
	__super::Initialize(pOwner);

	m_pPlayerTransformCom = m_pOwner->Get_Transform();
	m_pStateMachine = dynamic_cast<StateMachine*>(m_pOwner->Get_Component_FromName(Com_StateMachine));
	m_pUpperStateMachine = dynamic_cast<StateMachine*>(m_pOwner->Get_Component_FromName(L"UpperStateMachine_Component"));
	m_pPlayerMasterRig = pPlayer_MasterRig;
	m_pPlayer = dynamic_cast<Player*>(m_pOwner);
	m_pPlayerStat = m_pPlayer->Get_PlayerStatCom();

	Set_HasAnimEvents(true);
	Set_DamagePower(DAMAGEPOWER::STRONGPOWER);

	if (m_pPlayerTransformCom == nullptr || m_pStateMachine == nullptr || m_pPlayerMasterRig == nullptr || m_pPlayer == nullptr)
		return E_FAIL;

	return S_OK;
}

void Client::Player_AttackStrongCharge::Enter_State()
{
	if (m_pPlayer == nullptr || m_pPlayerMasterRig == nullptr)
		return;

	Player::INPUT_DESC& Desc = m_pPlayer->Get_PlayerInputDesc();

	//여기서도 이전 마우스를 얼마나 누르고 있어냐에 따라서 이펙트가 달라짐
	auto* ActiveWeapon = m_pPlayer->Get_ActivePlayerWeapon();
	if (ActiveWeapon == nullptr)
		return;
	else
	{
		ANIMREQUEST_DESC AnimDesc = {};
		AnimDesc.eWeaponAction = WEAPON_ANIM_ACTION::ATTACKSTRONGCHARGE;
		_int iAnimIndex = ActiveWeapon->RequestAnimIndex(AnimDesc);

		m_pPlayerMasterRig->Set_Animation(iAnimIndex, false);
	}

	if (!Desc.bLockOn)
	{
		m_pPlayer->Find_NearestMonsterAndRotation(40.f, 0.2f);
	}
}

void Client::Player_AttackStrongCharge::Update_State(_float fTimeDelta)
{
	Player::INPUT_DESC& Desc = m_pPlayer->Get_PlayerInputDesc();
	Player::PLAYERRUNTIMEEVENT_DESC EventDesc = m_pPlayer->Get_PlayerAnimEventDesc();

	//추후에 강공격같은경우는 연혈?과 같은 몬스터 공격시 얻을 수 있는 값이 있는데 그게 0보다 커야 활성화 가능하긴함.
	if (!EventDesc.bCanMove())
	{
		//m_pPlayer->Reset_PlayerMoveInputDesc();
		m_pPlayer->Set_ApplyTranslation(false);
	}

	if (EventDesc.bCanCombo() && Desc.bShiftPressed && (Desc.eLastMouseKeyState == MOUSEKEYSTATE::LB || Desc.eLastMouseKeyState == MOUSEKEYSTATE::RB))
	{
		m_pPlayer->Set_ApplyTranslation(true);
		m_pStateMachine->Change_State(Player::SPECIALATTACK);
		return;
	}
	else if (EventDesc.bCanCombo() && Desc.bCDown && !Desc.bZeroStamina)
	{
		m_pPlayer->Set_ApplyTranslation(true);
		m_pStateMachine->Change_State(Player::ATTACKPARRY);
		return;
	}
	else if (EventDesc.bCanCombo() && m_pPlayer->CanUseSkill())
	{
		m_pPlayer->Set_ApplyTranslation(true);
		return;
	}
	else if (EventDesc.bCanCombo() && Desc.eLastMouseKeyState == MOUSEKEYSTATE::RB)
	{
		m_pPlayer->Set_ApplyTranslation(true);
		Enter_State();
		return;
	}
	else if (EventDesc.bCanCombo() && Desc.eLastMouseKeyState == MOUSEKEYSTATE::LB)
	{
		m_pPlayer->Set_ApplyTranslation(true);
		m_pStateMachine->Change_State(Player::ATTACKNORMAL);
		return;
	}
	else if (EventDesc.bCanEscape() && Desc.bSpaceDown && XMVector3Equal(Desc.vMoveDir, XMVectorZero()))
	{
		m_pPlayer->Set_ApplyTranslation(true);
		m_pStateMachine->Change_State(Player::BACKSTEP); //백스텝  
		return;
	}
	else if (EventDesc.bCanEscape() && Desc.bSpaceDown && !XMVector3Equal(Desc.vMoveDir, XMVectorZero()))
	{
		m_pPlayer->Set_ApplyTranslation(true);
		m_pStateMachine->Change_State(Player::ROLL);
		return;
	}
	else if (EventDesc.bCanMove() && !XMVector3Equal(Desc.vMoveDir, XMVectorZero()))
	{
		m_pPlayer->Set_ApplyTranslation(true);
		m_pStateMachine->Change_State(Player::RUN);
		return;
	}
	else if (EventDesc.bCanMove() && Desc.bAltDown)
	{
		m_pPlayer->Set_ApplyTranslation(true);
		m_pUpperStateMachine->Change_State(Player::GUARDSTART);
		m_pStateMachine->Change_State(Player::GUARDWALK);
		return;
	}
	else if (EventDesc.bCanMove() && Desc.bDownKeyDown)
	{
		m_pPlayer->Set_ApplyTranslation(true);
		m_pUpperStateMachine->Change_State(Player::USEITEM);
		return;
	}
	else if (EventDesc.bCanMove() && Desc.bUpKeyDown)
	{
		m_pPlayer->Set_ApplyTranslation(true);
		m_pUpperStateMachine->Change_State(Player::WEAPONCHANGE);
		return;
	}


	if (m_pPlayerMasterRig->Is_AnimFinished() == true)
	{
		m_pStateMachine->Change_State(Player::IDLE);
	}

	//후반에는 키입력가능하게
}

void Client::Player_AttackStrongCharge::Exit_State()
{
}

void Client::Player_AttackStrongCharge::On_ProjectileEvent()
{
	//더 필요한 총알 있으면 조건 or 분기 

	Fire_BayonetChargeBullet();
}

void Client::Player_AttackStrongCharge::Fire_BayonetChargeBullet()
{

	Bayonet_Bullet::BAYONETBULLET_DESC Desc = {};

	Desc.vPosition.x = m_pPlayer->Get_BayonetMuzzleMatrixPtr()->_41;
	Desc.vPosition.y = m_pPlayer->Get_BayonetMuzzleMatrixPtr()->_42;
	Desc.vPosition.z = m_pPlayer->Get_BayonetMuzzleMatrixPtr()->_43;
	Desc.vPosition.w = 1.f;
	Desc.bPiercing = false;
	Desc.fSpeed = 15.f;
	Desc.fAttackRadius = 0.3f;
	Desc.fAttackDamage = 400.f;

	Player::INPUT_DESC& InputDesc = m_pPlayer->Get_PlayerInputDesc();


	m_pPlayerStat->Apply_Myeonghyeol(-2);

	XMStoreFloat3(&Desc.vDir, m_pPlayer->Get_Transform()->Get_State(DIRECTION::LOOK));

	GameObject* pBullet = PoolingManager::Get_Instance()->Acquire(POOL_ID::BAYONET_BULLET, &Desc);
	Transform* pBulletTransform = pBullet->Get_Transform();

	ParticleSystem* pEffect = CAST(ParticleSystem*)(PoolingManager::Get_Instance()->Acquire_Effect(POOL_ID::EFFECT_SHOTGUN_CIRCLE));
	ParticleSystem* pEffect2 = CAST(ParticleSystem*)(PoolingManager::Get_Instance()->Acquire_Effect(POOL_ID::EFFECT_SHOTGUN_CIRCLE_SMOKE));

	if (pEffect == nullptr) return;

	pEffect->Set_WorldMatrix(pBulletTransform->Get_WorldFloat4x4());
	pEffect->Play();
	pEffect->Set_FollowCallback([pBulletTransform]() -> _float4x4 {
		return pBulletTransform->Get_WorldFloat4x4();
		});
	pEffect->Set_FollowParent(true);

	CAST(Projectile*)(pBullet)->Set_FollowEffect(pEffect);

	if (pEffect2 == nullptr) return;

	pEffect2->Set_WorldMatrix(pBulletTransform->Get_WorldFloat4x4());
	pEffect2->Play();
	pEffect2->Set_FollowCallback([pBulletTransform]() -> _float4x4 {
		return pBulletTransform->Get_WorldFloat4x4();
		});
	pEffect2->Set_FollowParent(false);

	CAST(Projectile*)(pBullet)->Set_FollowEffect(pEffect2);

}

Player_AttackStrongCharge* Client::Player_AttackStrongCharge::Create(GameObject* pOwner, Player_MasterRig* pPlayer_MasterRig)
{
	Player_AttackStrongCharge* pInstance = new Player_AttackStrongCharge();

	if (FAILED(pInstance->Initialize(pOwner, pPlayer_MasterRig)))
	{
		MSG_BOX("Failed to Created : Player_AttackStrongCharge");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Player_AttackStrongCharge::Free()
{
	__super::Free();
}
