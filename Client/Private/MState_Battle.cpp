#include "Client_Define.h"
#include "MState_Battle.h"
#include "GameObject.h"
#include "Monster.h"


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::MState_Battle::MState_Battle()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::MState_Battle::Initialize(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	__super::Initialize(pOwner, iAnimIdx, fSpeed);

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 상태 진입 ////////////////////////////////////////////////////////
void Client::MState_Battle::Enter_State()
{
	// 몬스터가 배틀 상태에 진입(여기서 대부분 분기될 것)
	CHECK_JUST_NULL(m_pMonster);

	m_pMonster->Set_Battle(true);
}
/******************************************************* 상태 진입 *******************************************************/



//////////////////////////////////////////////////////// 상태 업데이트 ////////////////////////////////////////////////////////
void Client::MState_Battle::Update_State(_float fTimeDelta)
{
	// 배틀 상태의 몬스터는 어떻게 할 것인가
	CHECK_JUST_NULL(m_pMonster);

	/* 모두 상태 전이 테이블로 옮김 */
	//// 추격 범위 밖으로 도망쳤으면
	//if (Check_To_FallBack())
	//{
	//	// 원래 있던 자리로 돌아가
	//	m_pStateMachine->Change_State(Monster::MS_FALLBACK);
	//	return;
	//}
	//
	//// 시야각 밖에 있으면
	//if (!m_pMonster->Detect_With_FOV())
	//{
	//	// 보이는곳까지 돌아
	//	m_pStateMachine->Change_State(Monster::MS_TURN);
	//	return;
	//}

	//// 공격 쿨타임이 돌아왔으면
	//if(m_pMonster->Can_Attack())
	//{
	//	// 근접공격 범위 안에 있으면
	//	if (In_Attack_Range_Melee())
	//	{
	//		// 근접공격해
	//		m_pStateMachine->Change_State(Monster::MS_ATTACKMELEE0);

	//		return;
	//	}
	//	// 원거리공격 범위 안에 있으면
	//	else if(In_Attack_Range_Far())
	//	{
	//		// 원거리공격해
	//		m_pStateMachine->Change_State(Monster::MS_ATTACKFAR0);
	//		m_pMonster->Set_Attack(false);
	//		m_pMonster->Set_Walk_Dir(Monster::WALK_NONE);
	//		return;
	//	}
	//	// 나중에 스페셜 공격도 추가하지 않을까
	//	// 보스같은 경우엔 Battle_Boss로 따로 만들듯
	//}

	//// 이정도 범위면 걸어다니니?
	//if (In_Walk_Range())
	//{
	//	// 그럼 걸으렴
	//	m_pStateMachine->Change_State(Monster::MS_WALK);
	//	return;
	//}

	//// 어떻게 위에꺼 하나도 안 걸리냐 너는 그냥 달려서 플레이어 쫒아가라 어휴
	//m_pStateMachine->Change_State(Monster::MS_RUN);
}
/******************************************************* 상태 업데이트 *******************************************************/



//////////////////////////////////////////////////////// 상태 탈출 ////////////////////////////////////////////////////////
void Client::MState_Battle::Exit_State()
{
	CHECK_JUST_NULL(m_pMonster);

}
/******************************************************* 상태 탈출 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
MState_Battle* Client::MState_Battle::Create(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	MState_Battle* pInstance = new MState_Battle();

	MSG_FAIL(pInstance->Initialize(pOwner, iAnimIdx, fSpeed), L"MState_Battle 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::MState_Battle::Free()
{
	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/
