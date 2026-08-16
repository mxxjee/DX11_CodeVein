#include "Client_Define.h"
#include "BossBase.h"
#include "UIObj_HpBar.h"
#include "UIObj_Minimap.h"

Client::BossBase::BossBase()
{
}

Client::BossBase::BossBase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : Monster(pDevice, pContext)
{
}

Client::BossBase::BossBase(const BossBase& original)
    : Monster(original)
{
}

HRESULT Client::BossBase::Initialize(void* _arg)
{
    if (_arg != nullptr)
    {
        MONSTER_DESC* pDesc = static_cast<MONSTER_DESC*>(_arg);
    }

    CHECK_FAILED(__super::Initialize(_arg), E_FAIL);

    return S_OK;
}

_int Client::BossBase::Update(const _float fTimeDelta)
{
    m_DeadUIApearAlarm.Update(fTimeDelta);


    _int iProgress = __super::Update(fTimeDelta);
    if (iProgress < 0) return -1;

    return iProgress;
}

HRESULT Client::BossBase::Ready_Components()
{
    __super::Ready_Components();

    //보스용미니맵아이콘
    //전용 hp바
    //기타...

    return S_OK;
}

HRESULT Client::BossBase::Ready_States()
{
    if (FAILED(__super::Ready_States()))
        return E_FAIL;

    //m_pStateMachineCom->Add_State(PHASE_CHANGE, Monster_Olv_JumpAttack::Create(this));
    //m_pStateMachineCom->Add_State(GROGGY, Monster_Olv_Smash::Create(this));

    return S_OK;
}

void   Client::BossBase::OnDead()
{
    m_DeadUIApearAlarm.On();

    //HP바에게 죽음이벤트전송
    UIObj_HpBar::HpBarEvent Event;

    Event.eType = UIObj_HpBar::EVENTTYPE::DEAD;
    Event.iObjectID = m_iObjectID;
    m_pGameInstance->Publish(Event); //처리해주기!!

    //미니맵켜
    UIObject* pMinimap = m_pGameInstance->Find_UI_ByName(L"Minimap_Center");
    if (pMinimap)
    {
        pMinimap->Set_Visible(true);
        pMinimap->Set_Active(true);
    }

}


void Client::BossBase::Free()
{
    __super::Free();
}
