#include "Engine_Define.h"
#include "Stat.h"
#include "GameInstance.h"


Engine::Stat::Stat(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :Component(pDevice,pContext)
{
}

Engine::Stat::Stat(const Stat& original)
    :Component(original)
{
}

void Engine::Stat::Apply_Damage(_int iDamage)
{
    //최솟값 보장
    m_Stat.fCurrentHp = max(m_Stat.fCurrentHp - iDamage, 0.f);
}

_float Engine::Stat::ApplyDamageAndGetLoseHpRatio(_int iDamage)
{
    _float fPrevHp = m_Stat.fCurrentHp; 

    m_Stat.fCurrentHp = max(m_Stat.fCurrentHp - iDamage, 0.f);

    _float fLoseHp = fPrevHp - m_Stat.fCurrentHp;

    _float fLoseHpRatio = (fLoseHp / m_Stat.fMaxHp) * 100.f;

    return fLoseHpRatio;
}

void Engine::Stat::Add_Haze(_int iHaze)
{
    //최솟값 보장 
    m_Stat.iHaze = max(m_Stat.iHaze + iHaze, 0);

    COUT("StatComponent:Add_Haze" << "(" << iHaze << ")" << "Total:" << m_Stat.iHaze);


}

void Engine::Stat::Redeuce_Haze(_int iHaze)
{
   
    //최솟값 보장
    m_Stat.iHaze = max(m_Stat.iHaze - iHaze, 0);
    COUT("StatComponent:Redeuce_Haze" << "(" << iHaze << ")" << "Total:" << m_Stat.iHaze);

}   

void Engine::Stat::Add_FocusGauge(_float iFocusGauge, _uint iObjectID)
{
    if (m_bFocusState) //집중 상태라면 게이지가 증가하면 안되므로 리턴 (이 상태는 몬스터,또는 플레이어가 직접 세팅해주기)
        return;

    //최댓값 보장
    m_Stat.fCurrentFocusGauge = min(m_Stat.fCurrentFocusGauge + iFocusGauge,m_Stat.fMaxFocusGauge );

    if (m_Stat.fCurrentFocusGauge < m_Stat.fMaxFocusGauge)
    {
        GuardBarUIEvent Event;
        Event.m_EventType = GuardBarUIEventType::ONGUARDEVENT;
        Event.m_fValue= m_Stat.fCurrentFocusGauge;//"현재 집중 게이지 넘겨주고 "
        Event.m_iOwnerID = iObjectID;
        m_pGameInstance->Publish(Event);

        m_bIsMaxFoucsGauge = false;

    }
    else if (m_Stat.fCurrentFocusGauge >= m_Stat.fMaxFocusGauge)
    {
        m_bIsMaxFoucsGauge = true;

        GuardBarUIEvent Event;
        Event.m_EventType = GuardBarUIEventType::ONGUARDEVENT;
        Event.m_fValue = m_Stat.fCurrentFocusGauge;//"현재 집중 게이지 넘겨주고 "
        Event.m_iOwnerID = iObjectID;
        m_pGameInstance->Publish(Event);
    }
}

void Engine::Stat::Reduce_FocusGauge(_float iFocusGauge)
{
    //최솟값 보장
    m_Stat.fCurrentFocusGauge = max(m_Stat.fCurrentFocusGauge - iFocusGauge, 0.f);
}

void Engine::Stat::ContinuousDecrease_FocusGauge(_float fDecrasePerSecond, _float fTimeDelta)
{
    m_Stat.fCurrentFocusGauge -= fDecrasePerSecond * fTimeDelta;

    m_Stat.fCurrentFocusGauge = clamp(m_Stat.fCurrentFocusGauge, 0.f, m_Stat.fMaxFocusGauge);
}

void Engine::Stat::Heal_Hp(_float iHealAmount)
{
    //최댓값 보장
    m_Stat.fCurrentHp = min(m_Stat.fCurrentHp + iHealAmount, m_Stat.fMaxHp);
}

HRESULT Engine::Stat::Initialize_Prototype()
{
    return S_OK;
}

HRESULT Engine::Stat::Initialize(void* arg)
{
    if (arg == nullptr) //메시지 박스 띄워도 될듯
        return E_FAIL;

    m_Stat = *static_cast<STAT_DESC*>(arg);

    //인벤토리매니저로부터 오는 이벤트구독 (플레이어만 유효함)
    m_pGameInstance->Subscribe<HazeChangeEvent>([this](const HazeChangeEvent& Event)
        {
            CHECK_FALSE(m_Stat.m_bPlayer);
            switch (Event.m_eChangeType)
            {
            case HazeChangeType::SET:
                Set_Haze(Event.iHaze);
                break;

            case HazeChangeType::ADD:
                Add_Haze(Event.iHaze);
                break;

            case HazeChangeType::REDUCE:
                Redeuce_Haze(Event.iHaze);
                break;

            default:
                break;
            }
        });
    return S_OK;
}

void Engine::Stat::Free()
{
    __super::Free();
}
