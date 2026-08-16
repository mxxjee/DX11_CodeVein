#include "Client_Define.h"
#include "Yakumo_Dash.h"
#include "Yakumo.h"

Client::Yakumo_Dash::Yakumo_Dash()
{
}

HRESULT Client::Yakumo_Dash::Initialize(GameObject* pOwner)
{
	__super::Initialize(pOwner);

	return S_OK;           
}

void Client::Yakumo_Dash::Enter_State()
{
	if (m_pYakumo == nullptr)
		return;

	m_pYakumo->Set_Animation(7, true);

}

void Client::Yakumo_Dash::Update_State(_float fTimeDelta)
{
	auto& tRunTimeEvent = m_pYakumo->Get_RunTimeEvent();
	auto& tAICommandDesc = m_pYakumo->Get_AICommandDesc();
	auto& tAITargetInfo = m_pYakumo->Get_AITargetInfo();

	if (tAITargetInfo.bCombat && tAITargetInfo.pBattleTarget != nullptr)
	{
		if (tAITargetInfo.fDistToTarget <= m_pYakumo->Get_AdjustAttackRange()) //공격 가능한 범위에 있으면 이동상태를
		{
			_float fAttackProbability = m_pGameInstance->RandomValue(0.f, 100.f); //무슨 공격 할건지 확률
			if (fAttackProbability < 70.f) //대쉬 일떄는 70퍼 확률로 AttackDodgeF를 사용하게 만들어주고
			{
				m_pStateMachine->Change_State(Yakumo::ATTACKDODGEF);
				return;
			}
			else 
			{
				//m_pYakumo->Reset_AICommand();
				m_pYakumo->Set_EvaluateCoolTime(0.f);
			}
		}
	}

	m_fDashTime += fTimeDelta;
	if (m_fDashTime > 1.0f)
	{
		m_pYakumo->Reset_AICommand();
		m_pYakumo->Set_EvaluateCoolTime(0.f);
	}

	if (tRunTimeEvent.bInputArea()) //입력이벤트를(AI를 평가로 사용)
	{
		m_pYakumo->Evaluate_AI();
	}

	Check_SameStateCommandAI();

	if (!tAICommandDesc.bHasCommand) //아직 예약된 행동이 없으면 기존 유지 또는 Idle로 체인지 하면 되겠지
		return;

	if (tAICommandDesc.eReserveAction == Yakumo::ATTACKNORMAL || tAICommandDesc.eReserveAction == Yakumo::ATTACKSTRONG)
	{
		m_pStateMachine->Change_State(Yakumo::ATTACKDODGEF);
		return;
	}

	switch (tAICommandDesc.eReserveAction)
	{
	case Yakumo::IDLE:
		m_pStateMachine->Change_State(Yakumo::IDLE);
		break;
	case Yakumo::WALK:
		m_pStateMachine->Change_State(Yakumo::WALK);
		break;
	case Yakumo::RUN:
		m_pStateMachine->Change_State(Yakumo::RUN);
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

void Client::Yakumo_Dash::Exit_State()
{

}

Yakumo_Dash* Client::Yakumo_Dash::Create(GameObject* pOwner)
{
	Yakumo_Dash* pInstance = new Yakumo_Dash();

	if (FAILED(pInstance->Initialize(pOwner)))
	{
		MSG_BOX("Failed to Created : Yakumo_Dash");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Yakumo_Dash::Free()
{
	__super::Free();
}
