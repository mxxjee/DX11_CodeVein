#include "Client_Define.h"
#include "Yakumo_Damage.h"
#include "Yakumo.h"
#include "Player_Stat.h"
#include "Yakumo_Death.h"

Client::Yakumo_Damage::Yakumo_Damage()
{
}

HRESULT Client::Yakumo_Damage::Initialize(GameObject* pOwner)
{
	__super::Initialize(pOwner);

	return S_OK;
}

void Client::Yakumo_Damage::Enter_State()
{
	if (m_pYakumo == nullptr)
		return;

	_vector vEnemyHitDirection = m_pYakumo->Get_EnemyHitDirection();
	_vector vPlayerLook = m_pYakumoTransformCom->Get_State(DIRECTION::LOOK);

	Yakumo::FOUR_DIR Dir;

	Dir = m_pYakumo->Calculate_FourDir(vEnemyHitDirection, vPlayerLook);

	switch (Dir)
	{
	case Client::Yakumo::FRONT:
		m_pYakumo->Set_Animation(20, false);
		break;
	case Client::Yakumo::BACK:
		m_pYakumo->Set_Animation(19, false);
		break;
	case Client::Yakumo::RIGHT:
		m_pYakumo->Set_Animation(22, false);
		break;
	case Client::Yakumo::LEFT:
		m_pYakumo->Set_Animation(21, false);
		break;
	default:
		break;
	}


}

void Client::Yakumo_Damage::Update_State(_float fTimeDelta)
{
	auto& tRunTimeEvent = m_pYakumo->Get_RunTimeEvent();
	auto& tAICommandDesc = m_pYakumo->Get_AICommandDesc();

	if (m_pYakumoStatCom->Get_Hp() <= 0.f) //피격상태일때 플레이어의 Hp를 체크
	{

		m_pYakumo->Set_ApplyTranslation(false); //움직이지 못하게

		if (m_pYakumo->Get_Model()->Is_AnimFinished() == true) //데미지 피격 재생 끝났다면
		{
			State* pFindState = m_pStateMachine->Find_State(Yakumo::DEATH);
			Yakumo_Death* pFindDeathState = dynamic_cast<Yakumo_Death*>(pFindState);
			if (pFindDeathState)
				pFindDeathState->Set_DeathType(DEATHIDLE);
			m_pStateMachine->Change_State(Yakumo::DEATH);
		}

		return; //여기서는 계속 리턴해주고
	}

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

void Client::Yakumo_Damage::Exit_State()
{
	m_pYakumo->Set_ApplyTranslation(true);
}

Yakumo_Damage* Client::Yakumo_Damage::Create(GameObject* pOwner)
{
	Yakumo_Damage* pInstance = new Yakumo_Damage();

	if (FAILED(pInstance->Initialize(pOwner)))
	{
		MSG_BOX("Failed to Created : Yakumo_Damage");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Yakumo_Damage::Free()
{
	__super::Free();
}
