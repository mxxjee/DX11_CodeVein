#include "Client_Define.h"
#include "Yakumo_Roll.h"
#include "Yakumo.h"
#include "Player_Stat.h"

Client::Yakumo_Roll::Yakumo_Roll()
{
}

HRESULT Client::Yakumo_Roll::Initialize(GameObject* pOwner)
{
	__super::Initialize(pOwner);

	return S_OK;
}

void Client::Yakumo_Roll::Enter_State()
{
	if (m_pYakumo == nullptr)
		return;

	if (m_pYakumo->Get_AITargetInfo().bCombat)
	{
		Yakumo::FOUR_DIR eDir = m_pYakumo->Calculate_FourMoveDir(
			m_pYakumo->Get_AICommandDesc().vMoveDir);

		switch (eDir)
		{
		case Yakumo::FRONT: m_pYakumo->Set_Animation(9, true); break;
		case Yakumo::BACK:  m_pYakumo->Set_Animation(8, true); break;
		case Yakumo::LEFT:  m_pYakumo->Set_Animation(36, true); break;
		case Yakumo::RIGHT: m_pYakumo->Set_Animation(37, true); break;
		}
	}
	else
	{
		m_pYakumo->Set_Animation(9, true);
	}


	m_pYakumoStatCom->Decrease_Stamina(20.f);
}

void Client::Yakumo_Roll::Update_State(_float fTimeDelta)
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


	if (tRunTimeEvent.bCanEscape() && tAICommandDesc.eReserveAction == Yakumo::ROLL)
	{
		m_pYakumo->Set_ApplyTranslation(true);
		Enter_State();
		return;
	}
	else if (tRunTimeEvent.bCanEscape() && tAICommandDesc.eReserveAction == Yakumo::BACKSTEP)
	{
		m_pStateMachine->Change_State(Yakumo::BACKSTEP);
		return;
	}
	else if (tRunTimeEvent.bCanCombo() && tAICommandDesc.eReserveAction == Yakumo::ATTACKNORMAL)
	{
		m_pStateMachine->Change_State(Yakumo::ATTACKNORMAL);
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

void Client::Yakumo_Roll::Exit_State()
{
	m_pYakumo->Set_ApplyTranslation(true);
}

Yakumo_Roll* Client::Yakumo_Roll::Create(GameObject* pOwner)
{
	Yakumo_Roll* pInstance = new Yakumo_Roll();

	if (FAILED(pInstance->Initialize(pOwner)))
	{
		MSG_BOX("Failed to Created : Yakumo_Roll");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Yakumo_Roll::Free()
{
	__super::Free();
}
