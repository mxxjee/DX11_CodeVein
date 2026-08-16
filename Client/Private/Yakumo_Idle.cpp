#include "Client_Define.h"
#include "Yakumo_Idle.h"
#include "Yakumo.h"

Client::Yakumo_Idle::Yakumo_Idle()
{
}

HRESULT Client::Yakumo_Idle::Initialize(GameObject* pOwner)
{
	__super::Initialize(pOwner);

	return S_OK;
}

void Client::Yakumo_Idle::Enter_State()
{
	if (m_pYakumo == nullptr)
		return;

	m_pYakumo->Set_Animation(1, true);

}

void Client::Yakumo_Idle::Update_State(_float fTimeDelta)
{
	auto& tRunTimeEvent = m_pYakumo->Get_RunTimeEvent();
	auto& tAICommandDesc = m_pYakumo->Get_AICommandDesc();

	if (tRunTimeEvent.bInputArea()) //입력이벤트를(AI를 평가로 사용)
	{
		m_pYakumo->Evaluate_AI();
	}

	//나중에 시간 기준으로 상태를 바꾸지않고 애니메이션만 IdleTurn 이나 Stay 같은거 재생하도록

	//Check_SameStateCommandAI(); Idle 같은경우에는 재진입하면 이벤트가 꼬이네

	if (tAICommandDesc.eReserveAction == m_pStateMachine->Get_CurStateType()) 
	{
		m_pYakumo->Reset_AICommand();

	}

	if (!tAICommandDesc.bHasCommand) //아직 예약된 행동이 없으면 기존 유지 또는 Idle로 체인지 하면 되겠지
		return;

	switch (tAICommandDesc.eReserveAction)
	{
	case Yakumo::WALK:
		m_pStateMachine->Change_State(Yakumo::WALK);
		break;
	case Yakumo::RUN:
		m_pStateMachine->Change_State(Yakumo::RUN);
		break;
	case Yakumo::DASH:
		m_pStateMachine->Change_State(Yakumo::DASH);
		break;
	case Yakumo::ATTACKNORMAL:
		m_pStateMachine->Change_State(Yakumo::ATTACKNORMAL);
		break;
	case Yakumo::ATTACKSTRONG:
		m_pStateMachine->Change_State(Yakumo::ATTACKSTRONG);
		break;
	case Yakumo::SPECIALATTACK:
		m_pStateMachine->Change_State(Yakumo::SPECIALATTACK);
		break;
	case Yakumo::ROLL:
		m_pStateMachine->Change_State(Yakumo::ROLL);
		break;
	case Yakumo::BACKSTEP:
		m_pStateMachine->Change_State(Yakumo::BACKSTEP);
		break;
	default:
		break;
	}


}

void Client::Yakumo_Idle::Exit_State()
{
}

Yakumo_Idle* Client::Yakumo_Idle::Create(GameObject* pOwner)
{
	Yakumo_Idle* pInstance = new Yakumo_Idle();

	if (FAILED(pInstance->Initialize(pOwner)))
	{
		MSG_BOX("Failed to Created : Yakumo_Idle");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Yakumo_Idle::Free()
{
	__super::Free();
}
