#include "Client_Define.h"
#include "Yakumo_Death.h"
#include "Yakumo.h"
#include "Player_Stat.h"
#include "Yakumo_DamageBlow.h"

Client::Yakumo_Death::Yakumo_Death()
{
}

HRESULT Client::Yakumo_Death::Initialize(GameObject* pOwner)
{
	__super::Initialize(pOwner);

	return S_OK;
}

void Client::Yakumo_Death::Enter_State()
{
	if (m_pYakumo == nullptr)
		return;

	switch (m_eDeathType)
	{
	case DEATHIDLE:
		m_pYakumo->Set_Animation(39, false);
		break;
	case DOWNDEATH_S:
		m_pYakumo->Set_Animation(40, false);
		break;
	case DOWNDEATH_P:
		m_pYakumo->Set_Animation(41, false);
		break;
	default:
		break;
	}

	m_bFirst = false;

}

void Client::Yakumo_Death::Update_State(_float fTimeDelta)
{
	auto& tRunTimeEvent = m_pYakumo->Get_RunTimeEvent();
	auto& tAICommandDesc = m_pYakumo->Get_AICommandDesc();

	if (m_pYakumo->Get_Model()->Is_AnimFinished())
	{
		//디졸브 처리 -> 디졸브 끝나면 비활성화 해도 될듯
		// + Enter_Death() 에서 뭐 힐 카운트 초기화 등등 배틀 nullptr 등등
		m_pYakumo->Enter_DeadState();
	
		m_bFirst = true;

	}

}

void Client::Yakumo_Death::Exit_State()
{
	m_pYakumo->Set_ApplyTranslation(true);
}

Yakumo_Death* Client::Yakumo_Death::Create(GameObject* pOwner)
{
	Yakumo_Death* pInstance = new Yakumo_Death();

	if (FAILED(pInstance->Initialize(pOwner)))
	{
		MSG_BOX("Failed to Created : Yakumo_Death");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Yakumo_Death::Free()
{
	__super::Free();
}
