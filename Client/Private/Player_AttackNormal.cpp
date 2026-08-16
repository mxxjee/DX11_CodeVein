#include "Client_Define.h"
#include "Player_AttackNormal.h"
#include "Player_MasterRig.h"
#include "StateMachine.h"
#include "Player.h"
#include "Player_Weapon.h"

Client::Player_AttackNormal::Player_AttackNormal()
{
}

HRESULT Client::Player_AttackNormal::Initialize(GameObject* pOwner, Player_MasterRig* pPlayer_MasterRig)
{
	__super::Initialize(pOwner);

	m_pPlayerTransformCom = m_pOwner->Get_Transform();
	m_pStateMachine = dynamic_cast<StateMachine*>(m_pOwner->Get_Component_FromName(Com_StateMachine));
	m_pUpperStateMachine = dynamic_cast<StateMachine*>(m_pOwner->Get_Component_FromName(L"UpperStateMachine_Component"));
	m_pPlayerMasterRig = pPlayer_MasterRig;
	m_pPlayer = dynamic_cast<Player*>(m_pOwner);
	Set_HasAnimEvents(true);
	Set_DamagePower(DAMAGEPOWER::NORMALPOWER);
	
	
	if (m_pPlayerTransformCom == nullptr || m_pStateMachine == nullptr || m_pPlayerMasterRig == nullptr || m_pPlayer == nullptr)
		return E_FAIL;

	return S_OK;
}

void Client::Player_AttackNormal::Enter_State()
{
	if (m_pPlayer == nullptr || m_pPlayerMasterRig == nullptr)
		return;

	Player::INPUT_DESC& Desc = m_pPlayer->Get_PlayerInputDesc();

	m_pPlayer->Find_BackStabTarget();

	if (m_pPlayer->Get_SuccessBackStab())		
	{
		m_pStateMachine->Change_State(Player::BACKSTAB);
		return;
	}

	auto* ActiveWeapon = m_pPlayer->Get_ActivePlayerWeapon();
	if (ActiveWeapon == nullptr)
		return;
	else
	{
		if (ActiveWeapon->Get_WeaponType() == WEAPON_TYPE::SWORD)
		{
			if (m_iNormalAttackCobmo == 4)
				m_iNormalAttackCobmo = 0;
		}
		else
		{
			if (m_iNormalAttackCobmo == 3) //최대 콤보
				m_iNormalAttackCobmo = 0;
		}

		ANIMREQUEST_DESC AnimDesc = {};
		AnimDesc.eWeaponAction = WEAPON_ANIM_ACTION::ATTACKNORMAL;
		AnimDesc.iCombo = m_iNormalAttackCobmo;
		_int iAnimIndex = ActiveWeapon->RequestAnimIndex(AnimDesc);

		m_pPlayerMasterRig->Set_Animation(iAnimIndex, false);
	}

	if (!Desc.bLockOn)
	{
		m_pPlayer->Find_NearestMonsterAndRotation(40.f, 0.2f);
	}

}

void Client::Player_AttackNormal::Update_State(_float fTimeDelta)
{
	Player::INPUT_DESC& Desc = m_pPlayer->Get_PlayerInputDesc();
	Player::PLAYERRUNTIMEEVENT_DESC EventDesc = m_pPlayer->Get_PlayerAnimEventDesc();
	
	if (!EventDesc.bCanMove())
	{
		//m_pPlayer->Reset_PlayerMoveInputDesc();
		m_pPlayer->Set_ApplyTranslation(false);
	}


	if (EventDesc.bCanCombo() && Desc.bShiftPressed && (Desc.eLastMouseKeyState == MOUSEKEYSTATE::LB || Desc.eLastMouseKeyState == MOUSEKEYSTATE::RB) && !Desc.bZeroStamina)
	{
		Reset_NormalAttackCombo();
		m_pPlayer->Set_ApplyTranslation(true);
		m_pStateMachine->Change_State(Player::SPECIALATTACK);
		return;
	}
	else if (EventDesc.bCanCombo() && Desc.bCDown && !Desc.bZeroStamina)
	{
		Reset_NormalAttackCombo();
		m_pPlayer->Set_ApplyTranslation(true);
		m_pStateMachine->Change_State(Player::ATTACKPARRY);
		return;
	}
	else if (EventDesc.bCanCombo() && m_pPlayer->CanUseSkill())
	{
		Reset_NormalAttackCombo();
		m_pPlayer->Set_ApplyTranslation(true);
		return;
	}
	else if (EventDesc.bCanCombo() && Desc.eLastMouseKeyState ==MOUSEKEYSTATE::LB && !Desc.bZeroStamina)
	{
		m_pPlayer->Sample_AttackMoveDir_FromInput();

		if(m_iNormalAttackCobmo == 0)
			m_pPlayer->Start_AttackDir_RotationLerp(70.f,0.2f); //첫타는 좀더 강하게 회전
		else
			m_pPlayer->Start_AttackDir_RotationLerp(50.f, 0.2f); //콤보는 좀더 약하게 회전

		m_pPlayer->Set_ApplyTranslation(true);
		m_iNormalAttackCobmo++;
		Enter_State();
		return;
	}
	else if (EventDesc.bCanCombo() && Desc.eLastMouseKeyState == MOUSEKEYSTATE::RB && !Desc.bZeroStamina)
	{
		Reset_NormalAttackCombo();
		m_pPlayer->Set_ApplyTranslation(true);
		m_pStateMachine->Change_State(Player::ATTACKSTRONGSTART); //강공격
		return;
	}
	else if (EventDesc.bCanEscape() && Desc.bSpaceDown && XMVector3Equal(Desc.vMoveDir, XMVectorZero()) && !Desc.bZeroStamina)
	{
		Reset_NormalAttackCombo();
		m_pPlayer->Set_ApplyTranslation(true);
		m_pStateMachine->Change_State(Player::BACKSTEP); //백스텝  
		return;
	}
	else if (EventDesc.bCanEscape() && Desc.bSpaceDown && !XMVector3Equal(Desc.vMoveDir, XMVectorZero()) && !Desc.bZeroStamina)
	{
		Reset_NormalAttackCombo();
		m_pPlayer->Set_ApplyTranslation(true);
		m_pStateMachine->Change_State(Player::ROLL);
		return;
	}
	else if (EventDesc.bCanMove() && !XMVector3Equal(Desc.vMoveDir, XMVectorZero()))
	{
		Reset_NormalAttackCombo();
		m_pPlayer->Set_ApplyTranslation(true);
		m_pStateMachine->Change_State(Player::RUN);
		return;
	}
	else if (EventDesc.bCanMove() && Desc.bAltDown)
	{
		Reset_NormalAttackCombo();
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
		Reset_NormalAttackCombo();
		m_pPlayer->Set_ApplyTranslation(true);
		m_pStateMachine->Change_State(Player::IDLE);
		return;
	}

	//후반에는 키입력가능하게
}
  
void Client::Player_AttackNormal::Exit_State()
{
}

void Client::Player_AttackNormal::Reset_NormalAttackCombo()
{
	m_iNormalAttackCobmo = 0;
}

Player_AttackNormal* Client::Player_AttackNormal::Create(GameObject* pOwner, Player_MasterRig* pPlayer_MasterRig)
{
	Player_AttackNormal* pInstance = new Player_AttackNormal();

	if (FAILED(pInstance->Initialize(pOwner, pPlayer_MasterRig)))
	{
		MSG_BOX("Failed to Created : Player_AttackNormal");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Player_AttackNormal::Free()
{
	__super::Free();
}
