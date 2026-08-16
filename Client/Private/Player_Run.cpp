#include "Client_Define.h"
#include "Player_Run.h"
#include "Player_MasterRig.h"
#include "StateMachine.h"
#include "Player.h"
#include "Player_Weapon.h"

Client::Player_Run::Player_Run()
{
}

HRESULT Client::Player_Run::Initialize(GameObject* pOwner, Player_MasterRig* pPlayer_MasterRig)
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

void Client::Player_Run::Enter_State()
{
	if (m_pPlayer == nullptr || m_pPlayerMasterRig == nullptr)
		return;

	Player::INPUT_DESC& Desc = m_pPlayer->Get_PlayerInputDesc();

	_uint iAnimationIndex = {};

	if (Desc.bLockOn)
	{
		GameObject* pLockOnTarget = m_pPlayer->Get_LockOnTarget();
		if (pLockOnTarget == nullptr)
			return;

		INPUT_DIR InputDir = m_pPlayer->Calculate_LockOnDir(Desc.vMoveDir, pLockOnTarget);

		switch (InputDir)
		{
		case Engine::INPUT_DIR::LEFT: iAnimationIndex = 13;
			break;
		case Engine::INPUT_DIR::RIGHT: iAnimationIndex = 12;
			break;
		case Engine::INPUT_DIR::UP: iAnimationIndex = 14;
			break;
		case Engine::INPUT_DIR::DOWN: iAnimationIndex = 15;
			break;
		case Engine::INPUT_DIR::LEFT_UP: iAnimationIndex = 25;
			break;
		case Engine::INPUT_DIR::LEFT_DOWN:iAnimationIndex = 36;
			break;
		case Engine::INPUT_DIR::RIGHT_UP: iAnimationIndex = 21;
			break;
		case Engine::INPUT_DIR::RIGHT_DOWN: iAnimationIndex = 31;
			break;
		case Engine::INPUT_DIR::INPUT_END: iAnimationIndex = 14;
			break;
		default:
			break;
		}
	}
	else
		iAnimationIndex = 14;

	m_pPlayerMasterRig->Set_Animation(iAnimationIndex,true);
}

void Client::Player_Run::Update_State(_float fTimeDelta)
{
	Player::INPUT_DESC& Desc = m_pPlayer->Get_PlayerInputDesc();

	_uint iPrevState = m_pStateMachine->Get_PrevStateType();
	_uint iCurrentState = m_pStateMachine->Get_CurStateType();

	m_fDashDelay -= fTimeDelta;

	if (iPrevState == Player::DASH && iCurrentState == Player::RUN && Desc.bZeroStamina)
	{
		m_fDashDelay = 1.0f;
		return;
	}


	if (Desc.bCDown && !m_pPlayer->Get_OnGuard() && !Desc.bZeroStamina)
	{
		m_pStateMachine->Change_State(Player::ATTACKPARRY);
		return;
	}
	else if (m_pPlayer->CanUseSkill() && !m_pPlayer->Get_OnGuard())
	{
		return;
	}
	else if (Desc.bSpaceDown && !XMVector3Equal(Desc.vMoveDir, XMVectorZero()) && !m_pPlayer->Get_OnGuard() && !Desc.bZeroStamina)
	{
		m_pStateMachine->Change_State(Player::ROLL);
		return;
	}
	else if (Desc.eLastMouseKeyState == MOUSEKEYSTATE::LB && !m_pPlayer->Get_OnGuard() && !Desc.bZeroStamina)
	{
		m_pStateMachine->Change_State(Player::ATTACKNORMAL);
		return;
	}
	else if (Desc.eLastMouseKeyState == MOUSEKEYSTATE::RB && !m_pPlayer->Get_OnGuard() && !Desc.bZeroStamina)
	{
		m_pStateMachine->Change_State(Player::ATTACKSTRONGSTART);
		return;
	}
	else if (Desc.bShiftPressed && !m_pPlayer->Get_OnGuard())
	{
		if (Desc.bLockOn && !m_pPlayer->Get_LockOnShift())
		{
			Enter_State();
			return;
		}

		if (m_fDashDelay >= 0.f)
		{
			Enter_State();
			return;
		}

		m_pStateMachine->Change_State(Player::DASH);
		return;
	}
	else if (Desc.bAltDown)
	{
		m_pUpperStateMachine->Change_State(Player::GUARDSTART);
		m_pStateMachine->Change_State(Player::GUARDWALK);
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
	else if (!XMVector3Equal(Desc.vMoveDir, XMVectorZero()) && !m_pPlayer->Get_OnGuard())
	{
		Enter_State();
		return;
	}



	if (XMVector3Equal(Desc.vMoveDir, XMVectorZero())) //이거 이동량 없는경우
		m_pStateMachine->Change_State(Player::IDLE);
}

void Client::Player_Run::Exit_State()
{
}

Player_Run* Client::Player_Run::Create(GameObject* pOwner, Player_MasterRig* pPlayer_MasterRig)
{
	Player_Run* pInstance = new Player_Run();

	if (FAILED(pInstance->Initialize(pOwner, pPlayer_MasterRig)))
	{
		MSG_BOX("Failed to Created : Player_Run");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Player_Run::Free()
{
	__super::Free();
}
