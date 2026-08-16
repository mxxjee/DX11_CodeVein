#include "Client_Define.h"
#include "Yakumo_DamageEnd.h"
#include "Yakumo.h"
#include "Player_Stat.h"
#include "Yakumo_DamageBlow.h"

Client::Yakumo_DamageEnd::Yakumo_DamageEnd()
{
}

HRESULT Client::Yakumo_DamageEnd::Initialize(GameObject* pOwner)
{
	__super::Initialize(pOwner);

	return S_OK;
}

void Client::Yakumo_DamageEnd::Enter_State()
{
	if (m_pYakumo == nullptr)
		return;

	State* pState = m_pStateMachine->Find_State(Yakumo::DAMAGEBLOW);
	if (pState == nullptr)
		return;

	Yakumo_DamageBlow* pDamageBlowState = dynamic_cast<Yakumo_DamageBlow*>(pState);

	DOWN_POSTURE eDownPosture = pDamageBlowState->Get_DownPosture();

	_int iAnimIndex = {};

	switch (eDownPosture)
	{
	case PRONE:
		iAnimIndex = 27;
		break;
	case SUPINE:
		iAnimIndex = 29;
		break;
	default: iAnimIndex = 27;
		break;
	}

	m_pYakumo->Set_Animation(iAnimIndex, false);

}

void Client::Yakumo_DamageEnd::Update_State(_float fTimeDelta)
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

	if (tRunTimeEvent.bCanCombo() && tAICommandDesc.eReserveAction == Yakumo::SPECIALATTACK)
	{
		m_pStateMachine->Change_State(Yakumo::SPECIALATTACK);
		return;
	}
	else if (tRunTimeEvent.bCanCombo() && tAICommandDesc.eReserveAction == Yakumo::ATTACKSTRONG)
	{
		m_pStateMachine->Change_State(Yakumo::ATTACKSTRONG);
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

void Client::Yakumo_DamageEnd::Exit_State()
{
	m_pYakumo->Set_ApplyTranslation(true);
}

Yakumo_DamageEnd* Client::Yakumo_DamageEnd::Create(GameObject* pOwner)
{
	Yakumo_DamageEnd* pInstance = new Yakumo_DamageEnd();

	if (FAILED(pInstance->Initialize(pOwner)))
	{
		MSG_BOX("Failed to Created : Yakumo_DamageEnd");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Yakumo_DamageEnd::Free()
{
	__super::Free();
}
