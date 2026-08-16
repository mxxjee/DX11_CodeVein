#include "Client_Define.h"
#include "Yakumo_BackStep.h"
#include "Yakumo.h"
#include "Player_Stat.h"

Client::Yakumo_BackStep::Yakumo_BackStep()
{
}

HRESULT Client::Yakumo_BackStep::Initialize(GameObject* pOwner)
{
	__super::Initialize(pOwner);

	return S_OK;
}

void Client::Yakumo_BackStep::Enter_State()
{
	if (m_pYakumo == nullptr)
		return;

	m_pYakumo->Set_Animation(8, false);

	m_pYakumoStatCom->Decrease_Stamina(20.f);
}

void Client::Yakumo_BackStep::Update_State(_float fTimeDelta)
{
	auto& tRunTimeEvent = m_pYakumo->Get_RunTimeEvent();
	auto& tAICommandDesc = m_pYakumo->Get_AICommandDesc();

	if (m_pYakumo->Get_Model()->Is_AnimFinished())
	{
		m_pStateMachine->Change_State(Yakumo::IDLE);
		return;
	}

	if (!tRunTimeEvent.bCanMove())
		m_pYakumo->Set_ApplyTranslation(false);

	if (tRunTimeEvent.bInputArea()) //입력이벤트를(AI를 평가로 사용)
	{
		m_pYakumo->Evaluate_AI();
	}

	if (!tAICommandDesc.bHasCommand)
		return;

	if (tRunTimeEvent.bCanCombo() && (tAICommandDesc.eReserveAction == Yakumo::ATTACKNORMAL || tAICommandDesc.eReserveAction == Yakumo::ATTACKNORMAL))
	{
		m_pStateMachine->Change_State(Yakumo::ATTACKDODGEB);
		return;
	}

	if (tRunTimeEvent.bCanEscape() && tAICommandDesc.eReserveAction == Yakumo::BACKSTEP)
	{
		m_pYakumo->Set_ApplyTranslation(true);
		Enter_State();
		return;
	}
	else if (tRunTimeEvent.bCanEscape() && tAICommandDesc.eReserveAction == Yakumo::ROLL)
	{
		m_pStateMachine->Change_State(Yakumo::ROLL);
		return;
	}
	else if (tRunTimeEvent.bCanCombo() && tAICommandDesc.eReserveAction == Yakumo::SPECIALATTACK)
	{
		m_pStateMachine->Change_State(Yakumo::SPECIALATTACK);
		return;
	}
	else if (tRunTimeEvent.bCanMove() && tAICommandDesc.eReserveAction == Yakumo::DASH)
	{
		m_pStateMachine->Change_State(Yakumo::DASH);
		return;
	}
	else if (tRunTimeEvent.bCanMove() && tAICommandDesc.eReserveAction == Yakumo::RUN)
	{
		m_pStateMachine->Change_State(Yakumo::RUN);
		return;
	}
	else if (tRunTimeEvent.bCanMove() && tAICommandDesc.eReserveAction == Yakumo::WALK)
	{
		m_pStateMachine->Change_State(Yakumo::WALK);
		return;
	}
	else if (tRunTimeEvent.bCanMove() && tAICommandDesc.eReserveAction == Yakumo::IDLE)
	{
		m_pStateMachine->Change_State(Yakumo::IDLE);
		return;
	}

}

void Client::Yakumo_BackStep::Exit_State()
{
	m_pYakumo->Set_ApplyTranslation(true);
}

Yakumo_BackStep* Client::Yakumo_BackStep::Create(GameObject* pOwner)
{
	Yakumo_BackStep* pInstance = new Yakumo_BackStep();

	if (FAILED(pInstance->Initialize(pOwner)))
	{
		MSG_BOX("Failed to Created : Yakumo_BackStep");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Yakumo_BackStep::Free()
{
	__super::Free();
}
