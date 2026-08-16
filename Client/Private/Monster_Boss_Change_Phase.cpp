#include "Client_Define.h"
#include "Monster_Boss_Change_Phase.h"
#include "StateMachine.h"
#include "Monster.h"
#include "Monster_Weapon.h"
#include "St01_BossOliver.h" 

Client::Monster_Boss_Change_Phase::Monster_Boss_Change_Phase()
{
}

HRESULT Client::Monster_Boss_Change_Phase::Initialize(GameObject* pOwner)
{
	__super::Initialize(pOwner);

	m_pMonsterTransformCom = m_pOwner->Get_Transform();
	m_pStateMachine = dynamic_cast<StateMachine*>(m_pOwner->Get_Component_FromName(Com_StateMachine));
	Set_DamagePower(DAMAGEPOWER::HEAVYPOWER);
	m_pMonster = dynamic_cast<Monster*>(m_pOwner);
	Set_HasAnimEvents(true);
	if (m_pMonsterTransformCom == nullptr || m_pStateMachine == nullptr || m_pMonster == nullptr)
		return E_FAIL;

	return S_OK;
}

void Client::Monster_Boss_Change_Phase::Enter_State()
{
	if (m_pMonster == nullptr) return;

	m_iStep = 0;
	//0: 변신, 1: 모델교체, 2: 포효

	// 인간형태 변신 애니메이션 재생
	m_pMonster->Get_Model()->Set_Animation_CS(19, false);

	// 무적 혹은 강력한 슈퍼아머 부여
	m_pMonster->Set_SuperArmor(true);

}

void Client::Monster_Boss_Change_Phase::Update_State(_float fTimeDelta)
{
	if (m_pMonster == nullptr) return;

	if (m_iStep == 0 && m_pMonster->Get_Model()->Is_AnimFinished())
	{
		St01_BossOliver* pBoss = dynamic_cast<St01_BossOliver*>(m_pMonster);
		if (pBoss != nullptr)
		{
			pBoss->Change_To_Giant_Phase();
		}

		m_iStep = 1;

		m_pMonster->Get_Model()->Set_Animation_CS(5, false);
		m_iStep = 2;
	}

	if (m_iStep == 2 && m_pMonster->Get_Model()->Is_AnimFinished())
	{
		m_pStateMachine->Change_State(Monster::IDLE);
	}
}

void Client::Monster_Boss_Change_Phase::Exit_State()
{
	if (m_pMonster == nullptr) return;
	m_pMonster->Set_SuperArmor(false);
}

Monster_Boss_Change_Phase* Client::Monster_Boss_Change_Phase::Create(GameObject* pOwner)
{
	Monster_Boss_Change_Phase* pInstance = new Monster_Boss_Change_Phase();

	if (FAILED(pInstance->Initialize(pOwner)))
	{
		MSG_BOX("Failed to Created : Monster_Boss_Change_Phase");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Monster_Boss_Change_Phase::Free()
{
	__super::Free();
}
