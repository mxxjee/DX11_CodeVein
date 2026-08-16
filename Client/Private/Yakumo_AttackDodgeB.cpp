#include "Client_Define.h"
#include "Yakumo_AttackDodgeB.h"
#include "Yakumo.h"
#include "Player_Stat.h"

Client::Yakumo_AttackDodgeB::Yakumo_AttackDodgeB()
{
}

HRESULT Client::Yakumo_AttackDodgeB::Initialize(GameObject* pOwner)
{
	__super::Initialize(pOwner);

	Set_DamagePower(DAMAGEPOWER::NORMALPOWER);
	return S_OK;
}

void Client::Yakumo_AttackDodgeB::Enter_State()
{
	if (m_pYakumo == nullptr)
		return;

	m_pYakumo->Set_Animation(15, false);

	m_pYakumoStatCom->Decrease_Stamina(20.f);
}

void Client::Yakumo_AttackDodgeB::Update_State(_float fTimeDelta)
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

	if (tRunTimeEvent.bCanCombo() && tAICommandDesc.eReserveAction == Yakumo::ATTACKSTRONG)
	{
		m_pStateMachine->Change_State(Yakumo::ATTACKSTRONG);
		return;
	}
	else if (tRunTimeEvent.bCanCombo() && tAICommandDesc.eReserveAction == Yakumo::SPECIALATTACK)
	{
		m_pStateMachine->Change_State(Yakumo::SPECIALATTACK);
		return;
	}
	else if (tRunTimeEvent.bCanCombo() && tAICommandDesc.eReserveAction == Yakumo::ATTACKNORMAL)
	{
		m_pStateMachine->Change_State(Yakumo::ATTACKNORMAL);
		return;
	}
	else if (tRunTimeEvent.bCanEscape() && tAICommandDesc.eReserveAction == Yakumo::ROLL)
	{
		m_pStateMachine->Change_State(Yakumo::ROLL);
		return;
	}
	else if (tRunTimeEvent.bCanEscape() && tAICommandDesc.eReserveAction == Yakumo::BACKSTEP)
	{
		m_pStateMachine->Change_State(Yakumo::BACKSTEP);
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

void Client::Yakumo_AttackDodgeB::Exit_State()
{
	m_pYakumo->Set_ApplyTranslation(true);
}

Yakumo_AttackDodgeB* Client::Yakumo_AttackDodgeB::Create(GameObject* pOwner)
{
	Yakumo_AttackDodgeB* pInstance = new Yakumo_AttackDodgeB();

	if (FAILED(pInstance->Initialize(pOwner)))
	{
		MSG_BOX("Failed to Created : Yakumo_AttackDodgeB");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Yakumo_AttackDodgeB::Free()
{
	__super::Free();
}
