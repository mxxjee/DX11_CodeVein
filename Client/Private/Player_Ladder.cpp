#include "Client_Define.h"
#include "Player_Ladder.h"
#include "Player_MasterRig.h"
#include "StateMachine.h"
#include "Player.h"
#include "Player_Weapon.h"
#include "Player_DamageBlow.h"
#include "Player_BloodWeapon.h"
#include "InteractionManager.h"
#include "IInteractable.h"

Client::Player_Ladder::Player_Ladder()
{
}

HRESULT Client::Player_Ladder::Initialize(GameObject* pOwner, Player_MasterRig* pPlayer_MasterRig)
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

void Client::Player_Ladder::Enter_State()
{
	if (m_pPlayer == nullptr || m_pPlayerMasterRig == nullptr)
		return;

	Player::INPUT_DESC& Desc = m_pPlayer->Get_PlayerInputDesc();

	if (m_pPlayer->Get_ClimbUp()) //올라가는 동작
	{
		m_eLadderHand = HAND_LFET;
		m_eLadderDirection = LADDER_UP;
		m_pPlayerMasterRig->Set_Animation(253, false);
	}
	else if (m_pPlayer->Get_ClimbUp() == false) //내려가는 동작
	{
		m_eLadderHand = HAND_RIGHT;
		m_eLadderDirection = LADDER_DOWN;
		m_pPlayerMasterRig->Set_Animation(246, false);
	}

	m_bLadderEnd = false;

	//여기서 이제 해야하는게 Start Up일때는 플레이어이 위에있는손이 왼손이고 / Start Down일때는 플레이어의 위에있는손이 오른손 

	//이제 계속 번갈아가면서 행동을 정의해야함
}

void Client::Player_Ladder::Update_State(_float fTimeDelta)
{
	Player::INPUT_DESC& Desc = m_pPlayer->Get_PlayerInputDesc();
	Player::PLAYERRUNTIMEEVENT_DESC EventDesc = m_pPlayer->Get_PlayerAnimEventDesc();

	m_pPlayer->Set_ApplyTranslation(false); //루트애니메이션이므로 확실하게 키 이동값 막아주고 

	_vector vPlayerPos = m_pPlayerTransformCom->Get_State(DIRECTION::POSITION);
	_float fPlayerY = XMVectorGetY(vPlayerPos);

	_float fLadderTopY = m_pPlayer->Get_LadderTopPos().y;
	_float fLadderBottomY = m_pPlayer->Get_LadderBottomPos().y - 1.94f; //사다리에서 제일 바닥을 찾기위한 오프셋 맞쳐줬던값 그대로 더 빼줘야 딱맞음
	_float fThreshold = 2.0f;

	if (m_eLadderDirection == LADDER_UP && fPlayerY >= fLadderTopY - fThreshold && !m_bLadderEnd)
	{
		m_bLadderEnd = true;
		switch (m_eLadderHand)
		{
		case Client::Player_Ladder::HAND_LFET:
			m_pPlayerMasterRig->Set_Animation(250, false);
			break;
		case Client::Player_Ladder::HAND_RIGHT:
			m_pPlayerMasterRig->Set_Animation(252, false);
			break;
		default:
			break;
		}
		return;
	}
	else if (m_eLadderDirection == LADDER_DOWN && fPlayerY <= fLadderBottomY  && !m_bLadderEnd)
	{
		m_bLadderEnd = true;
		switch (m_eLadderHand)
		{
		case Client::Player_Ladder::HAND_LFET:
			m_pPlayerMasterRig->Set_Animation(243, false);
			break;
		case Client::Player_Ladder::HAND_RIGHT:
			m_pPlayerMasterRig->Set_Animation(245, false);
			break;
		default:
			break;
		}
		return;
	}

	if (m_bLadderEnd && m_pPlayerMasterRig->Is_AnimFinished())
	{
		IInteractable* pObj = InteractionManager::GetInstance()->Get_Current_Interaction_Target();
		if (pObj)
			pObj->Exit_Interaction(m_pPlayer);

		m_pPlayer->Get_ActivePlayerWeapon()->Set_PartActive(true);
		m_pStateMachine->Change_State(Player::IDLE);
		m_pPlayer->Set_ApplyTranslation(true); 

		YAKUMO_TELEPORT_EVENT TeleportEvent = {};
		TeleportEvent.fDelay = 3.0f;

		m_pGameInstance->Publish(TeleportEvent);

		return;
	}

	if (m_bLadderEnd)
		return;

	if (m_pPlayerMasterRig->Is_AnimFinished()==false && !m_bWaitAction)
		return;

	if (m_pGameInstance->KeyPress(DIK_W))
	{
		m_eLadderDirection = LADDER_UP;
		m_bWaitAction = false;

		switch (m_eLadderHand)
		{
		case Client::Player_Ladder::HAND_LFET:
			m_pPlayerMasterRig->Set_Animation(251, false);
			m_eLadderHand = HAND_RIGHT;
			break;
		case Client::Player_Ladder::HAND_RIGHT:
			m_pPlayerMasterRig->Set_Animation(249, false);
			m_eLadderHand = HAND_LFET;
			break;
		default:
			break;
		}

		return;
	}
	else if (m_pGameInstance->KeyPress(DIK_S))
	{
		m_eLadderDirection = LADDER_DOWN;
		m_bWaitAction = false;

		switch (m_eLadderHand)
		{
		case Client::Player_Ladder::HAND_LFET:
			m_pPlayerMasterRig->Set_Animation(242, false);
			m_eLadderHand = HAND_RIGHT;
			break;
		case Client::Player_Ladder::HAND_RIGHT:
			m_pPlayerMasterRig->Set_Animation(244, false);
			m_eLadderHand = HAND_LFET;
			break;
		default:
			break;
		}

		return;
	}
	else //키입력 없을때
	{
		if (!m_bWaitAction)
		{
			//COUT("BottomY:" << fLadderBottomY);
			//COUT("TopY:" << fLadderTopY);
			//COUT("PlayerY:" << fPlayerY);

			m_bWaitAction = true;

			switch (m_eLadderDirection)
			{
			case Client::Player_Ladder::LADDER_UP: //올라가는 동작
				switch (m_eLadderHand)
				{
				case Client::Player_Ladder::HAND_LFET:
					m_pPlayerMasterRig->Set_Animation(254, true);
					m_eLadderHand = HAND_LFET;
					break;
				case Client::Player_Ladder::HAND_RIGHT:
					m_pPlayerMasterRig->Set_Animation(255, true);
					m_eLadderHand = HAND_RIGHT;
					break;
				default:
					break;
				}
				break;
			case Client::Player_Ladder::LADDER_DOWN: //내려가는 동작
				switch (m_eLadderHand)
				{
				case Client::Player_Ladder::HAND_LFET:
					m_pPlayerMasterRig->Set_Animation(247, true);
					m_eLadderHand = HAND_LFET;
					break;
				case Client::Player_Ladder::HAND_RIGHT:
					m_pPlayerMasterRig->Set_Animation(248, true);
					m_eLadderHand = HAND_RIGHT;
					break;
				default:
					break;
				}
				break;
			default:
				break;
			}

		}

	
	}

}

void Client::Player_Ladder::Exit_State()
{
}

Player_Ladder* Client::Player_Ladder::Create(GameObject* pOwner, Player_MasterRig* pPlayer_MasterRig)
{
	Player_Ladder* pInstance = new Player_Ladder();

	if (FAILED(pInstance->Initialize(pOwner, pPlayer_MasterRig)))
	{
		MSG_BOX("Failed to Created : Player_Ladder");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Player_Ladder::Free()
{
	__super::Free();
}
