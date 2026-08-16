#include "Client_Define.h"
#include "MState_Run.h"
#include "GameObject.h"
#include "Monster.h"


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::MState_Run::MState_Run()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::MState_Run::Initialize(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	__super::Initialize(pOwner, iAnimIdx, fSpeed);

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 상태 진입 ////////////////////////////////////////////////////////
void Client::MState_Run::Enter_State()
{
	CHECK_JUST_NULL(m_pMonster);

	m_pMonster->Set_Animation_CS(m_iAnimIdx, m_fAnimSpeed, true);
	m_pMonster->Set_TurnType(Monster::TURN_RUN);	// RUN상태의 회전속도 적용
	m_pMonster->Set_RootPosAble(false);				// 달리기 모션이 앞으로 가는게 있어서 RootPos적용 X
	m_pMonster->Set_Acceleration(Monster::ACC_RUN);	// RUN상태의 가속도 적용


}
/******************************************************* 상태 진입 *******************************************************/



//////////////////////////////////////////////////////// 상태 업데이트 ////////////////////////////////////////////////////////
void Client::MState_Run::Update_State(_float fTimeDelta)
{
	CHECK_JUST_NULL(m_pMonster);

	//if (In_Attack_Range_Melee())
	//{
	//	m_pStateMachine->Change_State(Monster::MS_ATTACKMELEE0);
	//	return;
	//}
	//else if (In_Attack_Range_Far())
	//{
	//	m_pStateMachine->Change_State(Monster::MS_ATTACKFAR0);
	//	return;
	//}


	//if (m_pMonster->Is_Animation_Finished())
	//{
	//	m_pStateMachine->Change_State(Monster::MS_BATTLE);
	//	m_pStateMachine->Update_State(fTimeDelta);
	//}

}
/******************************************************* 상태 업데이트 *******************************************************/



//////////////////////////////////////////////////////// 상태 탈출 ////////////////////////////////////////////////////////
void Client::MState_Run::Exit_State()
{
	CHECK_JUST_NULL(m_pMonster);
	m_pMonster->Set_TurnType(Monster::TURN_END);
	m_pMonster->Set_RootPosAble(true);
	m_pMonster->Set_Acceleration(Monster::ACC_DEC);	// 감속


	m_pMonster->Set_FirstAware(true);//run은한번 감지한것이므로 aware true처리

}
/******************************************************* 상태 탈출 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
MState_Run* Client::MState_Run::Create(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	MState_Run* pInstance = new MState_Run();

	MSG_FAIL(pInstance->Initialize(pOwner, iAnimIdx, fSpeed), L"MState_Run 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::MState_Run::Free()
{
	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/
