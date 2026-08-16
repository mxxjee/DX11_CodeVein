#include "Client_Define.h"
#include "MState_Idle.h"
#include "GameObject.h"
#include "Monster.h"


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::MState_Idle::MState_Idle()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::MState_Idle::Initialize(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	__super::Initialize(pOwner, iAnimIdx, fSpeed);

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 상태 진입 ////////////////////////////////////////////////////////
void Client::MState_Idle::Enter_State()
{
	// 몬스터가 IDLE 상태에 진입
	CHECK_JUST_NULL(m_pMonster);

	// IDLE 애니메이션 재생
	m_pMonster->Set_Animation_CS(m_iAnimIdx, m_fAnimSpeed, true);
	// 전투상태 아님
	m_pMonster->Set_Battle(false);

	// PATROL하는 몬스터일 경우
	if (m_pMonster->Get_IdleType() == Monster::ENEMY_IDLE_TYPE::PATROL)
	{
		// 알람 초기화
		m_Alarm_To_Update.Limit = m_pGameInstance->RandomValue(2.f, 5.f);	// PATROL일 경우 얼마나 휴식했다 이동할지(나중에 변수로 바꿀듯)
		m_Alarm_To_Update.Elapsed = 0.f;
		m_Alarm_To_Update.On();
		m_bUpdate = false;

		// 시간 되면 다음 애니메이션 분기 처리
		m_Alarm_To_Update.m_AlarmFunc = [this]()
			{
				m_bUpdate = true;
			};
	}
}
/******************************************************* 상태 진입 *******************************************************/



//////////////////////////////////////////////////////// 상태 업데이트 ////////////////////////////////////////////////////////
void Client::MState_Idle::Update_State(_float fTimeDelta)
{
	// IDLE상태에서 할 일
	CHECK_JUST_NULL(m_pMonster);

	// 플레이어를 감지했는지 확인
	if (Detect_Player())
	{
		m_pMonster->Set_Battle(true);
		//m_pMonster->OnAwarePlayer(); 보스 update로 이동


		// IDLE타입에 따라 다음 행동 분기
		switch (m_pMonster->Get_IdleType())
		{
			// 수면 상태(앉아있거나 다른 뭔가 하던 상태)에서는 AWAKE로 전투 진입
		case Monster::ENEMY_IDLE_TYPE::SLEEP:
			m_pStateMachine->Change_State(Monster::MS_AWAKE);
			break;

			// PATROL중에 플레이어가 탐지범위 안에 들어왔다면 즉시 전투상태 진입
		case Monster::ENEMY_IDLE_TYPE::PATROL:
			m_pStateMachine->Change_State(Monster::MS_BATTLE);
			break;

			// STATIC중(가만히 서있는중)에 탐지범위 안에 들어왔다면 
		case Monster::ENEMY_IDLE_TYPE::STATIC:
			m_pStateMachine->Change_State(Monster::MS_TURN);
			break;
		}

		return;
	}

	// 감지하지 못 했는데 PATROL하는애면 적당히 지난 뒤에 정찰해라
	if (m_pMonster->Get_IdleType() == Monster::ENEMY_IDLE_TYPE::PATROL)
	{
		m_Alarm_To_Update.Update(fTimeDelta);

		if (m_bUpdate)
			m_pStateMachine->Change_State(Monster::MS_PATROL);

		return;
	}
}
/******************************************************* 상태 업데이트 *******************************************************/



//////////////////////////////////////////////////////// 상태 탈출 ////////////////////////////////////////////////////////
void Client::MState_Idle::Exit_State()
{
	CHECK_JUST_NULL(m_pMonster);

	m_Alarm_To_Update.Off();
}
/******************************************************* 상태 탈출 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
MState_Idle* Client::MState_Idle::Create(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	MState_Idle* pInstance = new MState_Idle();

	MSG_FAIL(pInstance->Initialize(pOwner, iAnimIdx, fSpeed), L"MState_Idle 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::MState_Idle::Free()
{
	__super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/
