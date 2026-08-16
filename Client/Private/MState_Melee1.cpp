#include "Client_Define.h"
#include "MState_Melee1.h"
#include "GameObject.h"
#include "Monster.h"


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::MState_Melee1::MState_Melee1()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::MState_Melee1::Initialize(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	__super::Initialize(pOwner, iAnimIdx, fSpeed);

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 상태 진입 ////////////////////////////////////////////////////////
void Client::MState_Melee1::Enter_State()
{
	// 몬스터가 MELEE공격 상태에 들어왔다
	CHECK_JUST_NULL(m_pMonster);

	// 어떤 속도로 회전할지
	m_pMonster->Set_TurnType(Monster::TURN_ATTACK);
	m_pMonster->Set_Attack(false);	 // 공격 가능상태 false로 만들어서 쿨타임 진행시키기(Monster의 Update가 함)
	m_pMonster->Set_Walk_Dir(Monster::WALK_NONE);	// 
	m_pMonster->Set_IsAttacking(true);

	// 애니메이션 세팅그
	m_pMonster->Set_Animation_CS(m_iAnimIdx, m_fAnimSpeed);
}
/******************************************************* 상태 진입 *******************************************************/



//////////////////////////////////////////////////////// 상태 업데이트 ////////////////////////////////////////////////////////
void Client::MState_Melee1::Update_State(_float fTimeDelta)
{
	CHECK_JUST_NULL(m_pMonster);
}
/******************************************************* 상태 업데이트 *******************************************************/



//////////////////////////////////////////////////////// 상태 탈출 ////////////////////////////////////////////////////////
void Client::MState_Melee1::Exit_State()
{
	CHECK_JUST_NULL(m_pMonster);
	m_pMonster->Set_TurnType(Monster::TURN_END);
	m_pMonster->Set_IsAttacking(false);
}
/******************************************************* 상태 탈출 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
MState_Melee1* Client::MState_Melee1::Create(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	MState_Melee1* pInstance = new MState_Melee1();

	MSG_FAIL(pInstance->Initialize(pOwner, iAnimIdx, fSpeed), L"MState_Melee1 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::MState_Melee1::Free()
{
	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/
