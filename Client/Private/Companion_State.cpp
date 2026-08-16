#include "Client_Define.h"
#include "Companion_State.h"
#include "Yakumo.h"

Client::Companion_State::Companion_State()
{
}

HRESULT Client::Companion_State::Initialize(GameObject* pOwner)
{
    CHECK_FAILED(__super::Initialize(pOwner), E_FAIL);

    m_pYakumoTransformCom = m_pOwner->Get_Transform();
    m_pStateMachine = CAST(StateMachine*)(m_pOwner->Get_Component_FromName(Com_StateMachine));
    m_pYakumo = CAST(Yakumo*)(m_pOwner);
    m_pYakumoStatCom = m_pYakumo->Get_YakumoStatCom();

    if (m_pYakumoTransformCom == nullptr || m_pStateMachine == nullptr || m_pYakumo == nullptr || m_pYakumoStatCom == nullptr)
        return E_FAIL;

    return S_OK;
}

void Client::Companion_State::Check_SameStateCommandAI()
{
    auto& tAICommandDesc = m_pYakumo->Get_AICommandDesc();

    if (!tAICommandDesc.bHasCommand)
        return;
 
    //예약된 행동이 현재 상태와 같으면 리셋 + 재진입
    if (tAICommandDesc.eReserveAction == m_pStateMachine->Get_CurStateType())
    {
        //m_pYakumo->Reset_AICommand();
        Enter_State();
    }
}

void Client::Companion_State::Free()
{
    __super::Free();
}
