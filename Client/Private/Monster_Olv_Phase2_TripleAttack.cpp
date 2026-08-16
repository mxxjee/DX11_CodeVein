#include "Client_Define.h"
#include "Monster_Olv_Phase2_TripleAttack.h"
#include "StateMachine.h"
#include "Monster.h"
#include "Monster_Weapon.h"

Client::Monster_Olv_Phase2_TripleAttack::Monster_Olv_Phase2_TripleAttack()
{
}

HRESULT Client::Monster_Olv_Phase2_TripleAttack::Initialize(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	__super::Initialize(pOwner, iAnimIdx, fSpeed);

	m_pMonsterTransformCom = m_pOwner->Get_Transform();
	m_pStateMachine = dynamic_cast<StateMachine*>(m_pOwner->Get_Component_FromName(Com_StateMachine));
	Set_DamagePower(DAMAGEPOWER::SPECIALPOWER);
	m_pMonster = dynamic_cast<Monster*>(m_pOwner);
	Set_HasAnimEvents(true);
	if (m_pMonsterTransformCom == nullptr || m_pStateMachine == nullptr || m_pMonster == nullptr)
		return E_FAIL;

	return S_OK;
}

void Client::Monster_Olv_Phase2_TripleAttack::Enter_State()
{
	if (m_pMonster == nullptr) return;

	m_pMonster->Get_Model()->Set_Animation_CS(m_iAnimIdx, false, 0.2f, m_fAnimSpeed);
}

void Client::Monster_Olv_Phase2_TripleAttack::Update_State(_float fTimeDelta)
{
	if (m_pMonster == nullptr) return;

	if (m_pMonster->Get_Model()->Is_AnimFinished())
	{
		m_pStateMachine->Change_State(Monster::IDLE);
	}
}

void Client::Monster_Olv_Phase2_TripleAttack::Exit_State()
{
	if (m_pMonster == nullptr) return;
}

Monster_Olv_Phase2_TripleAttack* Client::Monster_Olv_Phase2_TripleAttack::Create(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	Monster_Olv_Phase2_TripleAttack* pInstance = new Monster_Olv_Phase2_TripleAttack();

	if (FAILED(pInstance->Initialize(pOwner, iAnimIdx, fSpeed)))
	{
		MSG_BOX("Failed to Created : Monster_Olv_Phase2_TripleAttack");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Monster_Olv_Phase2_TripleAttack::Free()
{
	__super::Free();
}
