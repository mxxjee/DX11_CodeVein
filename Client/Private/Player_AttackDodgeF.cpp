#include "Client_Define.h"
#include "Player_AttackDodgeF.h"
#include "Player_MasterRig.h"
#include "StateMachine.h"
#include "Player.h"
#include "Player_Weapon.h"

Client::Player_AttackDodgeF::Player_AttackDodgeF()
{
}

HRESULT Client::Player_AttackDodgeF::Initialize(GameObject* pOwner, Player_MasterRig* pPlayer_MasterRig)
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

void Client::Player_AttackDodgeF::Enter_State()
{
	if (m_pPlayer == nullptr || m_pPlayerMasterRig == nullptr)
		return;

	Player::INPUT_DESC& Desc = m_pPlayer->Get_PlayerInputDesc();

	auto* ActiveWeapon = m_pPlayer->Get_ActivePlayerWeapon();
	if (ActiveWeapon == nullptr)
		return;
	else
	{
		ANIMREQUEST_DESC AnimDesc = {};
		AnimDesc.eWeaponAction = WEAPON_ANIM_ACTION::ATTACKDODGE_F;
		_int iAnimIndex = ActiveWeapon->RequestAnimIndex(AnimDesc);

		m_pPlayerMasterRig->Set_Animation(iAnimIndex, false);
	}

	if (!Desc.bLockOn)
	{
		m_pPlayer->Find_NearestMonsterAndRotation(40.f, 0.2f);
	}

}

void Client::Player_AttackDodgeF::Update_State(_float fTimeDelta)
{
	Player::INPUT_DESC& Desc = m_pPlayer->Get_PlayerInputDesc();
	Player::PLAYERRUNTIMEEVENT_DESC EventDesc = m_pPlayer->Get_PlayerAnimEventDesc();

	if (!EventDesc.bInputArea())
	{
		m_pPlayer->Sample_AttackMoveDir_FromInput();

		m_pPlayer->Start_AttackDir_RotationLerp(30.f, 0.2f);
	}

	if (!EventDesc.bCanMove())
	{
		//m_pPlayer->Reset_PlayerMoveInputDesc();
		m_pPlayer->Set_ApplyTranslation(false);
	}

	if (EventDesc.bCanCombo() && Desc.bShiftPressed && (Desc.eLastMouseKeyState == MOUSEKEYSTATE::LB || Desc.eLastMouseKeyState == MOUSEKEYSTATE::RB) && !Desc.bZeroStamina)
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
	else if (EventDesc.bCanCombo() && Desc.eLastMouseKeyState == MOUSEKEYSTATE::LB && !Desc.bZeroStamina)
	{
		m_pPlayer->Set_ApplyTranslation(true);
		m_pStateMachine->Change_State(Player::ATTACKNORMAL);
		return;
	}
	else if (EventDesc.bCanCombo() && Desc.eLastMouseKeyState == MOUSEKEYSTATE::RB && !Desc.bZeroStamina)
	{
		m_pPlayer->Set_ApplyTranslation(true);
		m_pStateMachine->Change_State(Player::ATTACKSTRONGSTART);
		return;
	}
	else if (EventDesc.bCanEscape() && Desc.bSpaceDown && XMVector3Equal(Desc.vMoveDir, XMVectorZero()) && !Desc.bZeroStamina)
	{
		m_pPlayer->Set_ApplyTranslation(true);
		m_pStateMachine->Change_State(Player::BACKSTEP); //백스텝  
		return;
	}
	else if (EventDesc.bCanEscape() && Desc.bSpaceDown && !XMVector3Equal(Desc.vMoveDir, XMVectorZero()) && !Desc.bZeroStamina)
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

void Client::Player_AttackDodgeF::Exit_State()
{
}

Player_AttackDodgeF* Client::Player_AttackDodgeF::Create(GameObject* pOwner, Player_MasterRig* pPlayer_MasterRig)
{
	Player_AttackDodgeF* pInstance = new Player_AttackDodgeF();

	if (FAILED(pInstance->Initialize(pOwner, pPlayer_MasterRig)))
	{
		MSG_BOX("Failed to Created : Player_AttackDodgeF");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Player_AttackDodgeF::Free()
{
	__super::Free();
}
