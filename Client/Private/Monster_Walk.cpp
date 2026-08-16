#include "Client_Define.h"
#include "Monster_Walk.h"
#include "StateMachine.h"
#include "Monster.h"
#include "Monster_Weapon.h"

Client::Monster_Walk::Monster_Walk()
{
}

HRESULT Client::Monster_Walk::Initialize(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	__super::Initialize(pOwner, iAnimIdx,fSpeed);

	m_pMonsterTransformCom = m_pOwner->Get_Transform();
	m_pStateMachine = dynamic_cast<StateMachine*>(m_pOwner->Get_Component_FromName(Com_StateMachine));
	m_pMonster = dynamic_cast<Monster*>(m_pOwner);

	Set_HasAnimEvents(false);

	if (m_pMonsterTransformCom == nullptr || m_pStateMachine == nullptr || m_pMonster == nullptr)
		return E_FAIL;

	return S_OK;
}

void Client::Monster_Walk::Enter_State()
{
	if (m_pMonster == nullptr)
		return;
	m_pMonster->Set_Speed(m_pMonster->Get_WalkSpeed());
	m_pMonster->Get_Model()->Set_Animation_CS(m_iAnimIdx, true, 0.2f, m_fAnimSpeed);

	//COUT("MonsterState::WALK");
}

void Client::Monster_Walk::Update_State(_float fTimeDelta)
{
	if (m_pMonster == nullptr) return;
	_float fDist = m_pMonster->Get_DistanceToTarget();
	                                                           
	if (fDist <= m_pMonster->Get_AttackRange()) {
		m_pStateMachine->Change_State(m_pMonster->Select_NextPattern());
		return;
	}

	if (fDist > m_pMonster->Get_WalkRange()) {
		m_pStateMachine->Change_State(Monster::RUN);
		return;
	}

	m_pMonster->LookAt_Target();
	m_pMonster->Set_MoveDirectionToTarget();
}

void Client::Monster_Walk::Exit_State()
{
}

Monster_Walk* Client::Monster_Walk::Create(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	Monster_Walk* pInstance = new Monster_Walk();

	if (FAILED(pInstance->Initialize(pOwner, iAnimIdx,fSpeed)))
	{
		MSG_BOX("Failed to Created : Monster_Walk");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Monster_Walk::Free()
{
	__super::Free();
}