#include "Client_Define.h"
#include "MState_Repel.h"
#include "GameObject.h"
#include "Monster.h"


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::MState_Repel::MState_Repel()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::MState_Repel::Initialize(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	__super::Initialize(pOwner, iAnimIdx, fSpeed);

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 상태 진입 ////////////////////////////////////////////////////////
void Client::MState_Repel::Enter_State()
{
	CHECK_JUST_NULL(m_pMonster);

	m_pMonster->Set_Animation_CS(m_iAnimIdx, m_fAnimSpeed);
	m_pMonster->Set_CanBackAttack(false);
	m_pMonster->Set_TurnType(Monster::TURN_END);
	m_pMonster->Set_Walk_Dir(Monster::WALK_END);
	m_pMonster->Set_AttackSweep(false);
}
/******************************************************* 상태 진입 *******************************************************/



//////////////////////////////////////////////////////// 상태 업데이트 ////////////////////////////////////////////////////////
void Client::MState_Repel::Update_State(_float fTimeDelta)
{
	CHECK_JUST_NULL(m_pMonster);

	if (m_pMonster->Is_Animation_Finished())
	{
		m_pMonster->Set_StunState(Monster::STUN_END);
		m_pStateMachine->Change_State(Monster::MS_STUN);
	}

}
/******************************************************* 상태 업데이트 *******************************************************/



//////////////////////////////////////////////////////// 상태 탈출 ////////////////////////////////////////////////////////
void Client::MState_Repel::Exit_State()
{
	CHECK_JUST_NULL(m_pMonster);

}
/******************************************************* 상태 탈출 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
MState_Repel* Client::MState_Repel::Create(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	MState_Repel* pInstance = new MState_Repel();

	MSG_FAIL(pInstance->Initialize(pOwner, iAnimIdx, fSpeed), L"MState_Repel 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::MState_Repel::Free()
{
	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/
