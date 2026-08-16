#include "Client_Define.h"
#include "Yakumo_AttackNormal.h"
#include "Yakumo.h"
#include "Player_Stat.h"

Client::Yakumo_AttackNormal::Yakumo_AttackNormal()
{
}

HRESULT Client::Yakumo_AttackNormal::Initialize(GameObject* pOwner)
{
	__super::Initialize(pOwner);

	Set_DamagePower(DAMAGEPOWER::NORMALPOWER);

	return S_OK;
}

void Client::Yakumo_AttackNormal::Enter_State()
{
	if (m_pYakumo == nullptr)
		return;
	
	switch (m_iNormalAttackCobmo)
	{
	case 0:
		m_pYakumo->Set_Animation(10, false);
		break;
	case 1:
		m_pYakumo->Set_Animation(11, false);
		break;
	case 2:
		m_pYakumo->Set_Animation(12, false);
		break;
	default:
		break;
	}

	m_pYakumoStatCom->Decrease_Stamina(30.f);
}

void Client::Yakumo_AttackNormal::Update_State(_float fTimeDelta)
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

	if (tRunTimeEvent.bCanCombo() && tAICommandDesc.eReserveAction == Yakumo::ATTACKNORMAL)
	{
		m_iNormalAttackCobmo++;
		m_pYakumo->Set_ApplyTranslation(true);
		Enter_State();
		return;
	}
	else if (tRunTimeEvent.bCanCombo() && tAICommandDesc.eReserveAction == Yakumo::ATTACKSTRONG)
	{
		m_pStateMachine->Change_State(Yakumo::ATTACKSTRONG);
		return;
	}
	else if (tRunTimeEvent.bCanCombo() && tAICommandDesc.eReserveAction == Yakumo::SPECIALATTACK)
	{
		m_pStateMachine->Change_State(Yakumo::SPECIALATTACK);
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

void Client::Yakumo_AttackNormal::Exit_State()
{
	m_pYakumo->Set_ApplyTranslation(true);
	Reset_NormalAttackCombo();
}

void Client::Yakumo_AttackNormal::Reset_NormalAttackCombo()
{
	m_iNormalAttackCobmo = 0;
}

Yakumo_AttackNormal* Client::Yakumo_AttackNormal::Create(GameObject* pOwner)
{
	Yakumo_AttackNormal* pInstance = new Yakumo_AttackNormal();

	if (FAILED(pInstance->Initialize(pOwner)))
	{
		MSG_BOX("Failed to Created : Yakumo_AttackNormal");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Yakumo_AttackNormal::Free()
{
	__super::Free();
}
