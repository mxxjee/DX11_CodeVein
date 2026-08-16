#include "Client_Define.h"
#include "MState_Far0.h"
#include "GameObject.h"
#include "Monster.h"


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::MState_Far0::MState_Far0()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::MState_Far0::Initialize(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	__super::Initialize(pOwner, iAnimIdx, fSpeed);

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 상태 진입 ////////////////////////////////////////////////////////
void Client::MState_Far0::Enter_State()
{
	// 몬스터가 RANGE공격 상태에 들어왔다
	CHECK_JUST_NULL(m_pMonster);

	// 어떤 속도로 회전할지
	m_pMonster->Set_TurnType(Monster::TURN_ATTACK);
	m_pMonster->Set_Attack(false);	 // 공격 가능상태 false로 만들어서 쿨타임 진행시키기(Monster의 Update가 함)
	m_pMonster->Set_Walk_Dir(Monster::WALK_NONE);	// 다음에 걸을 방향 초기화
	m_pMonster->Set_IsAttacking(true);
	m_pMonster->Set_Combo(false);
	m_pMonster->Set_CurrentPattern(m_pStateMachine->Get_CurStateType());

	// 공격 애니메이션 세팅
	m_pMonster->Set_Animation_CS(m_iAnimIdx, m_fAnimSpeed);
}
/******************************************************* 상태 진입 *******************************************************/



//////////////////////////////////////////////////////// 상태 업데이트 ////////////////////////////////////////////////////////
void Client::MState_Far0::Update_State(_float fTimeDelta)
{
	CHECK_JUST_NULL(m_pMonster);
}
/******************************************************* 상태 업데이트 *******************************************************/



//////////////////////////////////////////////////////// 상태 탈출 ////////////////////////////////////////////////////////
void Client::MState_Far0::Exit_State()
{
	CHECK_JUST_NULL(m_pMonster);
	m_pMonster->Set_TurnType(Monster::TURN_END);
	m_pMonster->Set_IsAttacking(false);

}
/******************************************************* 상태 탈출 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
MState_Far0* Client::MState_Far0::Create(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	MState_Far0* pInstance = new MState_Far0();

	MSG_FAIL(pInstance->Initialize(pOwner, iAnimIdx, fSpeed), L"MState_Far0 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::MState_Far0::Free()
{
	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/
