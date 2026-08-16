#include "Client_Define.h"
#include "Yakumo_DamageBlow.h"
#include "Yakumo.h"
#include "Player_Stat.h"
#include "Yakumo_Death.h"

Client::Yakumo_DamageBlow::Yakumo_DamageBlow()
{
}

HRESULT Client::Yakumo_DamageBlow::Initialize(GameObject* pOwner)
{
	__super::Initialize(pOwner);

	return S_OK;
}

void Client::Yakumo_DamageBlow::Enter_State()
{
	if (m_pYakumo == nullptr)
		return;

	_vector vEnemyHitDirection = m_pYakumo->Get_EnemyHitDirection();
	_vector vPlayerLook = m_pYakumoTransformCom->Get_State(DIRECTION::LOOK);

	Yakumo::FOUR_DIR Dir;

	Dir = m_pYakumo->Calculate_FourDir(vEnemyHitDirection, vPlayerLook);

	_int iAnimIndex = {};

	if (!m_bFirstDamage)
	{
		m_eDamageBlowType = BLOW;
	}

	m_bFirstDamage = true;

	switch (m_eDamageBlowType)
	{
	case BLOW:
		switch (Dir)
		{
		case Client::Yakumo::FRONT:
			iAnimIndex = 24;
			m_eDownPosture = PRONE;
			break;
		case Client::Yakumo::BACK:
			iAnimIndex = 23;
			m_eDownPosture = SUPINE;
			break;
		case Client::Yakumo::RIGHT:
			iAnimIndex = 26;
			m_eDownPosture = SUPINE;
			break;
		case Client::Yakumo::LEFT:
			iAnimIndex = 25;
			m_eDownPosture = SUPINE;
			break;
		default:
			break;
		}
		break;
	case LOOP:
		switch (m_eDownPosture)
		{
		case PRONE:
			iAnimIndex = 28;
			break;
		case SUPINE:
			iAnimIndex = 30;
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}

	m_pYakumo->Set_Animation(iAnimIndex, false);

}

void Client::Yakumo_DamageBlow::Update_State(_float fTimeDelta)
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
			if (m_eDownPosture == DOWN_POSTURE::PRONE)
			{
				pFindDeathState->Set_DeathType(DOWNDEATH_P);

			}
			else if (m_eDownPosture == DOWN_POSTURE::SUPINE)
			{
				pFindDeathState->Set_DeathType(DOWNDEATH_S);
			}

			m_pStateMachine->Change_State(Yakumo::DEATH);
		}

		return; //여기서는 계속 리턴해주고
	}

	m_pYakumo->Set_ApplyTranslation(false);

	if (m_eDamageBlowType == DAMAGEBLOWTYPE::LOOP)
	{
		m_fLoopEndTime -= fTimeDelta;
	}

	if (m_eDamageBlowType == DAMAGEBLOWTYPE::LOOP && m_fLoopEndTime <= 0.f)
	{
		//여기서는 조건이 충족되면
		//DamageEnd로 보내기
		m_bFirstDamage = false;
		m_pStateMachine->Change_State(Yakumo::DAMAGEEND);

		return;
	}
	else if (m_eDamageBlowType == DAMAGEBLOWTYPE::BLOW && m_pYakumo->Get_Model()->Is_AnimFinished() == true)
	{
		if (m_pYakumo->Get_LoseHpRatio() <= 30.f)
		{
			m_fLoopEndTime = 0.5f;
		}
		else if (m_pYakumo->Get_LoseHpRatio() >= 30.f)
		{
			m_fLoopEndTime = 1.0f;
		}
		else
		{
			m_fLoopEndTime = 1.0f;
		}
		//애니메이션 재생 끝나면 루프로 들어가게 한후에 
		m_eDamageBlowType = LOOP;
		Enter_State();
		return;
	}

}

void Client::Yakumo_DamageBlow::Exit_State()
{
	m_pYakumo->Set_ApplyTranslation(true);
}

Yakumo_DamageBlow* Client::Yakumo_DamageBlow::Create(GameObject* pOwner)
{
	Yakumo_DamageBlow* pInstance = new Yakumo_DamageBlow();

	if (FAILED(pInstance->Initialize(pOwner)))
	{
		MSG_BOX("Failed to Created : Yakumo_DamageBlow");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Yakumo_DamageBlow::Free()
{
	__super::Free();
}
