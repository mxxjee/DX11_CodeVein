#include "Client_Define.h"
#include "Monster_Battle.h"
#include "GameObject.h"
#include "Monster.h"


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::Monster_Battle::Monster_Battle()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::Monster_Battle::Initialize(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	__super::Initialize(pOwner, iAnimIdx, fSpeed);

	m_pMonsterTransformCom = m_pOwner->Get_Transform();
	m_pStateMachine = CAST(StateMachine*)(m_pOwner->Get_Component_FromName(Com_StateMachine));
	m_pMonster = CAST(Monster*)(m_pOwner);

	Set_HasAnimEvents(false);

	if (m_pMonsterTransformCom == nullptr || m_pStateMachine == nullptr || m_pMonster == nullptr)
		return E_FAIL;

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 상태 진입 ////////////////////////////////////////////////////////
void Client::Monster_Battle::Enter_State()
{
	CHECK_JUST_NULL(m_pMonster);

	// 몬스터가 전투 상태에 돌입

}
/******************************************************* 상태 진입 *******************************************************/



//////////////////////////////////////////////////////// 상태 업데이트 ////////////////////////////////////////////////////////
void Client::Monster_Battle::Update_State(_float fTimeDelta)
{
	// 전투 상태인 몬스터가 할 일
	CHECK_JUST_NULL(m_pMonster);
	
	// Idle상태로 돌아가는거면 return
	//CHECK_FUNCTION_FAIL_VOID(MonsterState::Check_To_Idle());


	// COUT("인지 범위");
	_bool isInFOV = m_pMonster->Detect_With_FOV(); // 몬스터의 시야 안에 플레이어가 있는지 판단
	if (isInFOV)
	{
		m_pStateMachine->Change_State(m_pMonster->Select_NextPattern());

	}
	else if (!isInFOV && m_pMonster->Get_IdleType() != Monster::ENEMY_IDLE_TYPE::PATROL)
	{
		m_pStateMachine->Change_State(Monster::AWAKE);
	}
	else if (m_pMonster->Get_IdleType() == Monster::ENEMY_IDLE_TYPE::PATROL)
	{
		m_pStateMachine->Change_State(Monster::PATROL);
		return;
	}

}
/******************************************************* 상태 업데이트 *******************************************************/



//////////////////////////////////////////////////////// 상태 탈출 ////////////////////////////////////////////////////////
void Client::Monster_Battle::Exit_State()
{
	CHECK_JUST_NULL(m_pMonster);

}
/******************************************************* 상태 탈출 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Monster_Battle* Client::Monster_Battle::Create(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	Monster_Battle* pInstance = new Monster_Battle();

	MSG_FAIL(pInstance->Initialize(pOwner, iAnimIdx, fSpeed), L"Monster_Battle 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::Monster_Battle::Free()
{
	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/
