#include "Client_Define.h"
#include "MState.h"

#include "Monster.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::MState::MState() : State()
{
}
/******************************************************* 생성자, 소멸자 *******************************************************/



//////////////////////////////////////////////////////// 이니셜라이즈 ////////////////////////////////////////////////////////
HRESULT Client::MState::Initialize(GameObject* pOwner, _uint iAnimIdx, _float fSpeed)
{
    CHECK_FAILED(__super::Initialize(pOwner), E_FAIL);

    m_pMonsterTransformCom = m_pOwner->Get_Transform();
    m_pStateMachine = CAST(StateMachine*)(m_pOwner->Get_Component_FromName(Com_StateMachine));
    m_pMonster = CAST(Monster*)(m_pOwner);

    m_bHasAnimEvents = false;

    if (m_pMonsterTransformCom == nullptr || m_pStateMachine == nullptr || m_pMonster == nullptr)
        return E_FAIL;

    m_iAnimIdx = iAnimIdx;
    m_fAnimSpeed = fSpeed;

    return S_OK;
}
/******************************************************* 이니셜라이즈 *******************************************************/



//////////////////////////////////////////////////////// 체크 헬퍼 함수 ////////////////////////////////////////////////////////
// Idle상태로 돌아갈지 확인(원래 위치로 돌아간 후 IDLE화)
_bool Client::MState::Check_To_FallBack()
{
    // 플레이어가 추격 범위 밖으로 벗어나면
    if (!m_pMonster->Chase_Player())
    {
        // 돌아가라고 명령
        return true;
    }

    return false;
}

// 플레이어가 탐지 범위에 들어왔는지 확인
_bool Client::MState::Detect_Player()
{
    // 플레이어를 찾았다면 true
    if (m_pMonster->Detect_Player())
    {
        return true;
    }

    return false;
}

// 근접공격 범위 안에 들어왔니?
_bool Client::MState::In_Attack_Range_Melee()
{
    _float meleeRange = m_pMonster->Get_AttackRange_MeleeSq();
    _float distance = m_pMonster->Get_DistanceSq();

    return meleeRange >= distance;
}

// 원거리공격 범위 안에 들어왔니?
_bool Client::MState::In_Attack_Range_Far()
{
    // 원거리 공격 없는애면 바로 컷
    if (m_pMonster->Get_AttackRange_Far() <= 0.f)
        return false;

    // 안 볼 것 같아서 rangeRange로 장난쳐봄
    _float rangeRange = m_pMonster->Get_AttackRange_FarSq();
    _float distance = m_pMonster->Get_DistanceSq();

    return rangeRange >= distance;
}

_bool Client::MState::In_Walk_Range()
{
    _float walkRange = m_pMonster->Get_WalkRangeSq();
    _float distance = m_pMonster->Get_DistanceSq();

    return walkRange >= distance;
}
/******************************************************* 체크 헬퍼 함수 *******************************************************/



//////////////////////////////////////////////////////// 보내줘라 ////////////////////////////////////////////////////////
void MState::Free()
{
    __super::Free();

}
/******************************************************* 보내줘라 *******************************************************/
