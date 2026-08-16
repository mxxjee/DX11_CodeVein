#include "Client_Define.h"
#include "Monster_AttackNormal.h"
#include "StateMachine.h"
#include "Monster.h"
#include "Monster_Weapon.h"
#include "UIObj_MonsterStatus.h"

Client::Monster_AttackNormal::Monster_AttackNormal()
{
}

HRESULT Client::Monster_AttackNormal::Initialize(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	__super::Initialize(pOwner, iAnimIdx, fSpeed);

	m_pMonsterTransformCom = m_pOwner->Get_Transform();
	m_pStateMachine = dynamic_cast<StateMachine*>(m_pOwner->Get_Component_FromName(Com_StateMachine));
	Set_DamagePower(DAMAGEPOWER::NORMALPOWER);
	m_pMonster = dynamic_cast<Monster*>(m_pOwner);
	Set_HasAnimEvents(true);
	if (m_pMonsterTransformCom == nullptr || m_pStateMachine == nullptr || m_pMonster == nullptr)
		return E_FAIL;

	return S_OK;
}

void Client::Monster_AttackNormal::Enter_State()
{
	if (m_pMonster == nullptr)
		return;

	m_pMonster->Get_Model()->Set_Animation_CS(m_iAnimIdx, false, 0.2f, m_fAnimSpeed);

	UIObj_MonsterStatus::MonsterStatusEvent Event{};
	Event.eType = UIObj_MonsterStatus::MONSTERSTATUS_UI_EVENT::SET_VISIBLE;

	Event.iOwnerID = m_pMonster->Get_ObjectID();
	m_pGameInstance->Publish(Event);
	m_pMonster->Set_IsAttacking(true);



	//COUT("MonsterState::ATTACKNORMAL");
}

void Client::Monster_AttackNormal::Update_State(_float fTimeDelta)
{
	//if (m_pMonster == nullptr) return;

	//// 공격 모션이 끝나지 않았다면 리턴
	//if (!m_pMonster->Get_Model()->Is_AnimFinished())
	//	return;

	//
	//_float dist = m_pMonster->Get_DistanceToTarget();
	//
	//// 공격 범위 안에 있으면
	//if (dist <= m_pMonster->Get_AttackRange())
	//{
	//	m_pMonster->LookAt_Smooth_Target(fTimeDelta);

	//	if(m_pMonster->Detect_With_FOV())
	//	{
	//		m_pMonster->Get_Model()->Set_Animation_CS(m_iAnimIdx, false, 0.2f, m_fAnimSpeed);
	//	}
	//	return;
	//}

	//// 추격 범위 안에 있으면
	//if(dist <= m_pMonster->Get_ChaseRange())
	//{
	//	// 계속 쫒아가기
	//	m_pStateMachine->Change_State(Monster::RUN);
	//	return;
	//}

	//// 기본값 IDLE화
	//m_pStateMachine->Change_State(Monster::IDLE);

	if (m_pMonster == nullptr) return;

	if (m_pMonster->Get_Model()->Is_AnimFinished())
	{
		m_pStateMachine->Change_State(Monster::IDLE);
	}
}

void Client::Monster_AttackNormal::Exit_State()
{
	m_pMonster->Set_IsAttacking(false);

}

Monster_AttackNormal* Client::Monster_AttackNormal::Create(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	Monster_AttackNormal* pInstance = new Monster_AttackNormal();

	if (FAILED(pInstance->Initialize(pOwner, iAnimIdx, fSpeed)))
	{
		MSG_BOX("Failed to Created : Monster_AttackNormal");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void Client::Monster_AttackNormal::Free()
{
	__super::Free();
}


//#include "Client_Define.h"
//#include "Monster_AttackNormal.h"
//#include "StateMachine.h"
//#include "Monster.h"
//#include "Monster_Weapon.h"
//
//Client::Monster_AttackNormal::Monster_AttackNormal()
//{
//}
//
//HRESULT Client::Monster_AttackNormal::Initialize(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
//{
//	__super::Initialize(pOwner, iAnimIdx, fSpeed);
//
//	m_pMonsterTransformCom = m_pOwner->Get_Transform();
//	m_pStateMachine = dynamic_cast<StateMachine*>(m_pOwner->Get_Component_FromName(Com_StateMachine));
//	
//	m_pMonster = dynamic_cast<Monster*>(m_pOwner);
//	Set_HasAnimEvents(true);
//	if (m_pMonsterTransformCom == nullptr || m_pStateMachine == nullptr || m_pMonster == nullptr)
//		return E_FAIL;
//
//	return S_OK;
//}
//
//void Client::Monster_AttackNormal::Enter_State()
//{
//	if (m_pMonster == nullptr)
//		return;
//
//	m_pMonster->Get_Model()->Set_Animation_CS(m_iAnimIdx, false, 0.2f, m_fAnimSpeed);
//
//
//	COUT("MonsterState::ATTACKNORMAL");
//}
//
//void Client::Monster_AttackNormal::Update_State(_float fTimeDelta)
//{
//	if (m_pMonster == nullptr) return;
//
//	// 공격 모션이 끝나지 않았다면 리턴
//	if (!m_pMonster->Get_Model()->Is_AnimFinished())
//		return;
//
//	
//	_float dist = m_pMonster->Get_DistanceToTarget();
//	
//	// 공격 범위 안에 있으면
//	if (dist <= m_pMonster->Get_AttackRange())
//	{
//		m_pMonster->LookAt_Smooth_Target(fTimeDelta);
//
//		if(m_pMonster->Detect_With_FOV())
//		{
//			m_pMonster->Get_Model()->Set_Animation_CS(m_iAnimIdx, false, 0.2f, m_fAnimSpeed);
//		}
//		return;
//	}
//
//	// 추격 범위 안에 있으면
//	if(dist <= m_pMonster->Get_ChaseRange())
//	{
//		// 계속 쫒아가기
//		m_pStateMachine->Change_State(Monster::RUN);
//		return;
//	}
//
//	// 기본값 IDLE화
//	m_pStateMachine->Change_State(Monster::IDLE);
//}
//
//void Client::Monster_AttackNormal::Exit_State()
//{
//
//}
//
//Monster_AttackNormal* Client::Monster_AttackNormal::Create(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
//{
//	Monster_AttackNormal* pInstance = new Monster_AttackNormal();
//
//	if (FAILED(pInstance->Initialize(pOwner, iAnimIdx,fSpeed)))
//	{
//		MSG_BOX("Failed to Created : Monster_AttackNormal");
//		Safe_Release(pInstance);
//	}
//
//	return pInstance;
//}
//
//void Client::Monster_AttackNormal::Free()
//{
//	__super::Free();
//}


