#include "Client_Define.h"
#include "Yakumo_Walk.h"
#include "Yakumo.h"

Client::Yakumo_Walk::Yakumo_Walk()
{
}

HRESULT Client::Yakumo_Walk::Initialize(GameObject* pOwner)
{
	__super::Initialize(pOwner);

	return S_OK;
}

void Client::Yakumo_Walk::Enter_State()
{
	if (m_pYakumo == nullptr)
		return;

	if (m_pYakumo->Get_AITargetInfo().bCombat)
	{
		Yakumo::FOUR_DIR eDir = m_pYakumo->Calculate_FourMoveDir(
			m_pYakumo->Get_AICommandDesc().vMoveDir);

		switch (eDir)
		{
		case Yakumo::FRONT: m_pYakumo->Set_Animation(4, true); break;
		case Yakumo::BACK:  m_pYakumo->Set_Animation(3, true); break;
		case Yakumo::LEFT:  m_pYakumo->Set_Animation(32, true); break;
		case Yakumo::RIGHT: m_pYakumo->Set_Animation(33, true); break;
		}
	}
	else
	{
		m_pYakumo->Set_Animation(4, true);
	}

}

void Client::Yakumo_Walk::Update_State(_float fTimeDelta)
{
	auto& tRunTimeEvent = m_pYakumo->Get_RunTimeEvent();
	auto& tAICommandDesc = m_pYakumo->Get_AICommandDesc();
	auto& tAITargetInfo = m_pYakumo->Get_AITargetInfo();

	if (tAITargetInfo.bCombat && tAITargetInfo.pBattleTarget != nullptr)
	{
		if (tAITargetInfo.fDistToTarget <= m_pYakumo->Get_AdjustAttackRange()) //공격 가능한 범위에 있으면 이동상태를
		{
			m_pYakumo->Set_EvaluateCoolTime(0.f); //쿨타임 리셋
			//m_pYakumo->Reset_AICommand();
		}
	}

	if (tRunTimeEvent.bInputArea()) //입력이벤트를(AI를 평가로 사용)
	{
		m_pYakumo->Evaluate_AI();
	}

	Check_SameStateCommandAI();

	if (!tAICommandDesc.bHasCommand) //아직 예약된 행동이 없으면 기존 유지 또는 Idle로 체인지 하면 되겠지
		return;

	switch (tAICommandDesc.eReserveAction)
	{
	case Yakumo::IDLE:
		m_pStateMachine->Change_State(Yakumo::IDLE);
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

void Client::Yakumo_Walk::Exit_State()
{
}

Yakumo_Walk* Client::Yakumo_Walk::Create(GameObject* pOwner)
{
	Yakumo_Walk* pInstance = new Yakumo_Walk();

	if (FAILED(pInstance->Initialize(pOwner)))
	{
		MSG_BOX("Failed to Created : Yakumo_Walk");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Yakumo_Walk::Free()
{
	__super::Free();
}
