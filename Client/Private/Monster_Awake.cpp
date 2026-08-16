#include "Client_Define.h"
#include "Monster_Awake.h"
#include "GameObject.h"
#include "Monster.h"


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::Monster_Awake::Monster_Awake()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::Monster_Awake::Initialize(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	__super::Initialize(pOwner, iAnimIdx, fSpeed);

	m_pMonsterTransformCom = m_pOwner->Get_Transform();
	m_pStateMachine = dynamic_cast<StateMachine*>(m_pOwner->Get_Component_FromName(Com_StateMachine));
	m_pMonster = dynamic_cast<Monster*>(m_pOwner);

	Set_HasAnimEvents(false);

	if (m_pMonsterTransformCom == nullptr || m_pStateMachine == nullptr || m_pMonster == nullptr)
		return E_FAIL;

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 상태 진입 ////////////////////////////////////////////////////////
void Client::Monster_Awake::Enter_State()
{
	CHECK_JUST_NULL(m_pMonster);

	m_pMonster->Get_Model()->Set_Animation_CS(m_iAnimIdx, false, 0.2f, m_fAnimSpeed);
}
/******************************************************* 상태 진입 *******************************************************/



//////////////////////////////////////////////////////// 상태 업데이트 ////////////////////////////////////////////////////////
void Client::Monster_Awake::Update_State(_float fTimeDelta)
{
	CHECK_JUST_NULL(m_pMonster);

	// 애니메이션 덜 끝났으면 리턴
	if (!m_pMonster->Get_Model()->Is_AnimFinished())
		return;

	_float targetDist = m_pMonster->Get_DistanceToTarget();
	_float attackRange = m_pMonster->Get_AttackRange();

	// 타겟과의 거리가 공격범위 이내일 경우
	if (attackRange > targetDist)
	{
		// 공격
		m_pStateMachine->Set_State(Monster::ATTACKNORMAL);
		return;
	}

	_float dist = m_pMonster->Get_ChaseRange();
	// 추격 범위 이내일 경우
	if (targetDist < dist)
	{
		// 쫒아갓!
		m_pStateMachine->Set_State(Monster::RUN);
		return;
	}


}
/******************************************************* 상태 업데이트 *******************************************************/



//////////////////////////////////////////////////////// 상태 탈출 ////////////////////////////////////////////////////////
void Client::Monster_Awake::Exit_State()
{
	CHECK_JUST_NULL(m_pMonster);

}
/******************************************************* 상태 탈출 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Monster_Awake* Client::Monster_Awake::Create(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	Monster_Awake* pInstance = new Monster_Awake();

	MSG_FAIL(pInstance->Initialize(pOwner, iAnimIdx, fSpeed), L"Monster_Awake 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::Monster_Awake::Free()
{
	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/
