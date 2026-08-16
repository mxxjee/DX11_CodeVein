#include "Client_Define.h"
#include "Player_Walk.h"
#include "Player_MasterRig.h"
#include "StateMachine.h"
#include "Player.h"
#include "Player_Weapon.h"

Client::Player_Walk::Player_Walk()
{
}

HRESULT Client::Player_Walk::Initialize(GameObject* pOwner, Player_MasterRig* pPlayer_MasterRig)
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

void Client::Player_Walk::Enter_State()
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
		case Engine::INPUT_DIR::LEFT: iAnimationIndex = 3;
			break;
		case Engine::INPUT_DIR::RIGHT: iAnimationIndex = 2;
			break;
		case Engine::INPUT_DIR::UP: iAnimationIndex = 4;
			break;
		case Engine::INPUT_DIR::DOWN: iAnimationIndex = 5;
			break;
		case Engine::INPUT_DIR::LEFT_UP: iAnimationIndex = 8;
			break;
		case Engine::INPUT_DIR::LEFT_DOWN:iAnimationIndex = 9;
			break;
		case Engine::INPUT_DIR::RIGHT_UP: iAnimationIndex = 6;
			break;
		case Engine::INPUT_DIR::RIGHT_DOWN: iAnimationIndex = 10;
			break;
		case Engine::INPUT_DIR::INPUT_END: iAnimationIndex = 4;
			break;
		default:
			break;
		}
	}
	else
		iAnimationIndex = 4;

	m_pPlayerMasterRig->Set_Animation(iAnimationIndex, true);

}

void Client::Player_Walk::Update_State(_float fTimeDelta)
{
	Player::INPUT_DESC& Desc = m_pPlayer->Get_PlayerInputDesc();
	Player::PLAYERRUNTIMEEVENT_DESC EventDesc = m_pPlayer->Get_PlayerAnimEventDesc();
	_bool bUpperPlaying = m_pPlayerMasterRig->Get_UpperBlendEnable();

	if (!XMVector3Equal(Desc.vMoveDir, XMVectorZero())) //움직임이 있으면 다시 Walk로 
	{
		if (!bUpperPlaying)
		{
			m_pPlayerMasterRig->Request_ClearAnimationUpper(0.05f); //상체 0.05초 블렌드하면서 사라지게
			m_pUpperStateMachine->Change_State(Player::PLAYERUPPERSTATE_END); //상체 상태머신은 nullptr로 초기화
			m_pStateMachine->Change_State(Player::IDLE);
			return;
		}
		else
		{
			Enter_State();
			return;
		}
	}
	else //움직임 없을때
	{
		if (bUpperPlaying) //상체 애니메이션 중이라면
		{
			//m_pPlayerMasterRig->Sync_BaseToUpperAnim();
			return;
		}
		else
		{
			m_pPlayerMasterRig->Request_ClearAnimationUpper(0.05f); //상체 0.05초 블렌드하면서 사라지게
			m_pUpperStateMachine->Change_State(Player::PLAYERUPPERSTATE_END); //상체 상태머신은 nullptr로 초기화
			m_pStateMachine->Change_State(Player::IDLE);
			return;
		}
	}

}

void Client::Player_Walk::Exit_State()
{
}

Player_Walk* Client::Player_Walk::Create(GameObject* pOwner, Player_MasterRig* pPlayer_MasterRig)
{
	Player_Walk* pInstance = new Player_Walk();

	if (FAILED(pInstance->Initialize(pOwner, pPlayer_MasterRig)))
	{
		MSG_BOX("Failed to Created : Player_Walk");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Player_Walk::Free()
{
	__super::Free();
}
