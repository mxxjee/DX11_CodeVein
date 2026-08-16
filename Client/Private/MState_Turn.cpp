#include "Client_Define.h"
#include "MState_Turn.h"
#include "GameObject.h"
#include "Monster.h"


//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::MState_Turn::MState_Turn()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::MState_Turn::Initialize(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	__super::Initialize(pOwner, iAnimIdx, fSpeed);

    m_vecAnimationData.resize(TURN_ANIMATION_END);

	return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 상태 진입 ////////////////////////////////////////////////////////
void Client::MState_Turn::Enter_State()
{
    CHECK_JUST_NULL(m_pMonster);

    m_pMonster->Calculate_To_PlayerDistance_And_Direction();
    m_pMonster->Set_TurnType(Monster::TURN_WALK);   // 걷는 속도로 돌아
    m_pMonster->Set_RootPosAble(true);              // 루트 회전 On
    _float angle = m_pMonster->Calculate_Angle_To_Player();
    _float absAngle = fabsf(angle);

    // 앞 45도면 회전 없이 바로 Battle
    if (absAngle <= 45.f)
    {
        m_pStateMachine->Change_State(Monster::MS_BATTLE);
    }
    // 뒤 45도면 180도 회전 
    else if (absAngle >= 135.f)
    {
        if (angle > 0.f)
        {
            Animation_Data data = m_vecAnimationData[TURN_RIGHT180];
            if (data.bIsSet == false)
            {
                m_pStateMachine->Change_State(Monster::MS_BATTLE);
                return;
            }

            m_pMonster->Set_Animation_CS(data.iAnimationIndex, data.fAnimationSpeed, data.bIsLoop, data.fLerpTime);
        }
        else
        {
            Animation_Data data = m_vecAnimationData[TURN_LEFT180];
            if (data.bIsSet == false)
            {
                m_pStateMachine->Change_State(Monster::MS_BATTLE);
                return;
            }

            m_pMonster->Set_Animation_CS(data.iAnimationIndex, data.fAnimationSpeed, data.bIsLoop, data.fLerpTime);
        }
    }
    // 그 외에 오른쪽
    else if (angle > 0.f)
    {
        Animation_Data data = m_vecAnimationData[TURN_RIGHT];
        if (data.bIsSet == false)
        {
            m_pStateMachine->Change_State(Monster::MS_BATTLE);
            return;
        }

        m_pMonster->Set_Animation_CS(data.iAnimationIndex, data.fAnimationSpeed, data.bIsLoop, data.fLerpTime);
    }
    // 왼쪽
    else
    {
        Animation_Data data = m_vecAnimationData[TURN_LEFT];
        if (data.bIsSet == false)
        {
            m_pStateMachine->Change_State(Monster::MS_BATTLE);
            return;
        }

        m_pMonster->Set_Animation_CS(data.iAnimationIndex, data.fAnimationSpeed, data.bIsLoop, data.fLerpTime);
    }
}
/******************************************************* 상태 진입 *******************************************************/



//////////////////////////////////////////////////////// 상태 업데이트 ////////////////////////////////////////////////////////
void Client::MState_Turn::Update_State(_float fTimeDelta)
{
	CHECK_JUST_NULL(m_pMonster);

    //// 애니메이션이 끝나지 않았다면 패스
    //if (!m_pMonster->Is_Loop_Animation() && !m_pMonster->Is_Animation_Finished())
    //    return;

    //// 플레이어를 쫒는 범위라면
    //if (m_pMonster->Chase_Player())
    //{
    //    // 한 번 더 회전 감지
    //    MState_Turn::Enter_State();
    //    return;
    //}

    //// 플레이어가 범위 밖으로 도망쳤으니까
    //// 원래 자리로 돌아가
    //m_pStateMachine->Change_State(Monster::MS_FALLBACK);
    return;

}
/******************************************************* 상태 업데이트 *******************************************************/



//////////////////////////////////////////////////////// 상태 탈출 ////////////////////////////////////////////////////////
void Client::MState_Turn::Exit_State()
{
	CHECK_JUST_NULL(m_pMonster);

    m_pMonster->Set_TurnType(Monster::TURN_END);

}
/******************************************************* 상태 탈출 *******************************************************/



//////////////////////////////////////////////////////// 추가 애니메이션 등록 ////////////////////////////////////////////////////////
HRESULT Client::MState_Turn::Add_Additional_Anim(_uint _Index, _float _speed, _int _slotNum, _bool _isLoop, _float _lerpTime)
{
    Animation_Data data;
    data.bIsSet = true;
    data.bIsLoop = _isLoop;
    data.fAnimationSpeed = _speed;
    data.fLerpTime = _lerpTime;
    data.iAnimationIndex = _Index;

    m_vecAnimationData[_slotNum] = data;

    return S_OK;
}
/******************************************************* 추가 애니메이션 등록 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
MState_Turn* Client::MState_Turn::Create(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
	MState_Turn* pInstance = new MState_Turn();

	MSG_FAIL(pInstance->Initialize(pOwner, iAnimIdx, fSpeed), L"MState_Turn 원본 생성 실패", L"경고!!!", nullptr);

	return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::MState_Turn::Free()
{
	__super::Free();
    m_umapAnimationData.clear();
}
/******************************************************* 객체 반환 함수 *******************************************************/
