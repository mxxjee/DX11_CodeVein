#include "Client_Define.h"
#include "Player_Dash.h"
#include "Player_MasterRig.h"
#include "StateMachine.h"
#include "Player.h"
#include "Player_Weapon.h"

Client::Player_Dash::Player_Dash()
{
}

HRESULT Client::Player_Dash::Initialize(GameObject* pOwner, Player_MasterRig* pPlayer_MasterRig)
{
	__super::Initialize(pOwner);

	m_pPlayerTransformCom = m_pOwner->Get_Transform();
	m_pStateMachine = dynamic_cast<StateMachine*>(m_pOwner->Get_Component_FromName(Com_StateMachine));
	m_pUpperStateMachine = dynamic_cast<StateMachine*>(m_pOwner->Get_Component_FromName(L"UpperStateMachine_Component"));
	m_pPlayerMasterRig = pPlayer_MasterRig;
	m_pPlayer = dynamic_cast<Player*>(m_pOwner);
	Set_HasAnimEvents(false);
	if (m_pPlayerTransformCom == nullptr || m_pStateMachine == nullptr || m_pPlayerMasterRig == nullptr || m_pPlayer == nullptr)
		return E_FAIL;

	return S_OK;
}

void Client::Player_Dash::Enter_State()
{
	if (m_pPlayer == nullptr || m_pPlayerMasterRig == nullptr)
		return;

	Player::INPUT_DESC& Desc = m_pPlayer->Get_PlayerInputDesc();

	if (Desc.bZeroStamina)
	{
		if (!XMVector3Equal(Desc.vMoveDir, XMVectorZero()))
		{
			m_pStateMachine->Change_State(Player::RUN);
			return;
		}
		else
		{
			m_pStateMachine->Change_State(Player::IDLE);
			return;
		}
	}

	m_pPlayerMasterRig->Set_Animation(39,true);

}

void Client::Player_Dash::Update_State(_float fTimeDelta)
{
	Player::INPUT_DESC& Desc = m_pPlayer->Get_PlayerInputDesc();


	//이 키입력 + Shift 상태가 Dash고  좌클릭 or 우클릭
	if (Desc.bCDown && !m_pPlayer->Get_OnGuard() && !Desc.bZeroStamina)
	{
		m_pStateMachine->Change_State(Player::ATTACKPARRY);
		return;
	}
	else if (m_pPlayer->CanUseSkill())
	{
		return;
	}
	else if (Desc.bDownKeyDown)
	{
		m_pUpperStateMachine->Change_State(Player::USEITEM);
		return;
	}
	else if (Desc.bUpKeyDown)
	{
		m_pUpperStateMachine->Change_State(Player::WEAPONCHANGE);
		return;
	}
	else if ((Desc.eLastMouseKeyState == MOUSEKEYSTATE::LB || Desc.eLastMouseKeyState == MOUSEKEYSTATE::RB && !m_pPlayer->Get_OnGuard()) && !Desc.bZeroStamina)
	{
		m_pStateMachine->Change_State(Player::ATTACKDODGEF);
		return;
	}
	else if (Desc.bSpaceDown && !XMVector3Equal(Desc.vMoveDir, XMVectorZero()) && !m_pPlayer->Get_OnGuard() && !Desc.bZeroStamina)
	{
		m_pStateMachine->Change_State(Player::ROLL);
		return;
	}
	else if ((!Desc.bShiftPressed && !m_pPlayer->Get_OnGuard()) || Desc.bZeroStamina)
	{
		m_pStateMachine->Change_State(Player::RUN);
		return;
	}
	else if (Desc.bAltDown)
	{
		m_pUpperStateMachine->Change_State(Player::GUARDSTART);
		m_pStateMachine->Change_State(Player::GUARDWALK);
		return;
	}

	if (XMVector3Equal(Desc.vMoveDir, XMVectorZero())) //이거 이동량 없는경우
		m_pStateMachine->Change_State(Player::IDLE);
}

void Client::Player_Dash::Exit_State()
{
}

Player_Dash* Client::Player_Dash::Create(GameObject* pOwner, Player_MasterRig* pPlayer_MasterRig)
{
	Player_Dash* pInstance = new Player_Dash();

	if (FAILED(pInstance->Initialize(pOwner, pPlayer_MasterRig)))
	{
		MSG_BOX("Failed to Created : Player_Dash");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Player_Dash::Free()
{
	__super::Free();
}
