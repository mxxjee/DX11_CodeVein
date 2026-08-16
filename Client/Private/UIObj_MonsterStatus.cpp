#include "Client_Define.h"
#include "UIObj_MonsterStatus.h"
#include "UIObj_ProgressBar.h"

#include "UIObj_HpBar.h"
#include "UIObj_GuardBar.h"


#include "Monster.h"
#include "Camera.h"


/////////////////////////////////////////////
Client::UIObj_MonsterStatus::UIObj_MonsterStatus()
{
}

Client::UIObj_MonsterStatus::UIObj_MonsterStatus(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UIObject(pDevice, pContext)
{
}

Client::UIObj_MonsterStatus::UIObj_MonsterStatus(const UIObj_MonsterStatus& original)
    :UIObject(original)
{
}

Client::UIObj_MonsterStatus::~UIObj_MonsterStatus()
{
}
///////////////////////////////////////////////////////


//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_MonsterStatus::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    return S_OK;
}

HRESULT Client::UIObj_MonsterStatus::Initialize(void* arg)
{
    CHECK_FAILED(__super::Initialize(arg), E_FAIL);

    m_pGameInstance->Subscribe< MonsterStatusEvent>([this](const MonsterStatusEvent& Event)
        {
            CHECK_TRUE(m_iTargetObjectID == UINT_MAX);

            if (Event.iOwnerID == m_iTargetObjectID)
            {
                switch (Event.eType)
                {
                case MONSTERSTATUS_UI_EVENT::SET_VISIBLE:
                {
                    Set_Visible(true);
                    Set_Active(true);
                }
                       
                    break;

                case MONSTERSTATUS_UI_EVENT::SET_INVISIBLE:
                        Set_Active(false);
                    break;

                case MONSTERSTATUS_UI_EVENT::OWNER_DEAD:
                    Owner_Dead();
                    break;
                }
            }
        });

    return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/

_int Client::UIObj_MonsterStatus::Update_Priority(const _float fTimeDelta)
{

    __super::Update_Priority(fTimeDelta);
    return 0;
}

_int Client::UIObj_MonsterStatus::Update(const _float fTimeDelta)
{
    if (m_iTargetObjectID != UINT_MAX && m_pTargetTransform)
    {
        _float3 vWorldPos;
        XMStoreFloat3(&vWorldPos, m_pTargetTransform->Get_State(DIRECTION::POSITION));

        Set_Position_By_World(vWorldPos + m_fOffSet);
    }
    __super::Update(fTimeDelta);

    return 0;
}

_int Client::UIObj_MonsterStatus::Update_Late(const _float fTimeDelta)
{
   
    Set_Scale_By_Distance();


   __super::Update_Late(fTimeDelta);
    return 0;
}

HRESULT Client::UIObj_MonsterStatus::Render(const _float fTimeDelta)
{
    __super::Render(fTimeDelta);
    return 0;
}



UIObj_MonsterStatus* Client::UIObj_MonsterStatus::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    UIObj_MonsterStatus* pInstance = new UIObj_MonsterStatus(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_MonsterStatus 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}

GameObject* Client::UIObj_MonsterStatus::Clone(void* pArg)
{
    UIObj_MonsterStatus* pInstance = new UIObj_MonsterStatus(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_MonsterStatus 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}
void Client::UIObj_MonsterStatus::Free()
{
  /*  if(!m_bInPool)
        m_pGameInstance->Return_PoolObject(_UINT(UI_POOLING_TYPE::MONSTERSTATUSBAR), this);*/

    m_ProgressBars.clear();
   
    __super::Free();
}

void Client::UIObj_MonsterStatus::Set_Scale_By_Distance()
{
    CHECK_JUST_NULL(m_pTargetCam);
    CHECK_JUST_NULL(m_pTargetTransform);

    //기준 거리
    _float fBaseDistance = 10.0f;

    
    _float fDistance = XMVectorGetX(XMVector3Length(m_pTargetTransform->Get_State(DIRECTION::POSITION) - m_pTargetCam->Get_Position()));
    _float fScaleFactor = fBaseDistance / fDistance;

 
    fScaleFactor = max(0.3f, min(fScaleFactor,0.7f));

    // 4. 원래 UI 크기에 적용
    Set_Size(Get_InitSize().x * fScaleFactor,
        Get_InitSize().y * fScaleFactor);
}



///////////////////////////////////////////////////////////////////////////////////////////////////////
void        Client::UIObj_MonsterStatus::After_ApplyData()
{
    //자식캐싱
    m_ProgressBars.resize(_UINT(MONTSTERSTATUS_UI_TYPE::END));


    UIObject* pMainHPObj = Get_Child(L"HPBar");
    if (pMainHPObj)
        m_ProgressBars[_UINT(MONTSTERSTATUS_UI_TYPE::HPBAR)] = pMainHPObj;


    UIObject* pGuardBarObj = Get_Child(L"GuardBar");
    if (pGuardBarObj)
        m_ProgressBars[_UINT(MONTSTERSTATUS_UI_TYPE::GUARDBAR)] =pGuardBarObj;


}

/******************************************************* 객체 준비 함수 *******************************************************/

void Client::UIObj_MonsterStatus::Set_TargetObjectID(Monster* pOwner, _float3 OffSet)
{
    CHECK_JUST_NULL(pOwner);

    m_iTargetObjectID = pOwner->Get_ObjectID();
    m_pTargetTransform = pOwner->Get_Transform();


    if (m_ProgressBars[_UINT(MONTSTERSTATUS_UI_TYPE::HPBAR)])
    {
        UIObj_HpBar* pHpBar = dynamic_cast<UIObj_HpBar*>(m_ProgressBars[_UINT(MONTSTERSTATUS_UI_TYPE::HPBAR)]);
        if (pHpBar)
        {
            pHpBar->Set_Owner_ObjectID(m_iTargetObjectID);

            pHpBar->Set_CurrentFloat(pOwner->Get_CurrentHp());
            pHpBar->Set_MaxFloat(pOwner->Get_MaxHp());
        
            pHpBar->Init_Ratio(true, 1.f);
            pHpBar->Init_Ratio(false, 1.f);
        }

    }
      
    if (m_ProgressBars[_UINT(MONTSTERSTATUS_UI_TYPE::GUARDBAR)])
    {
        UIObj_GuardBar* pGuardBar = dynamic_cast<UIObj_GuardBar*>(m_ProgressBars[_UINT(MONTSTERSTATUS_UI_TYPE::GUARDBAR)]);
        if (pGuardBar)
        {
            pGuardBar->Set_OwnerObjectID(m_iTargetObjectID);

            //가드값도 연결..
        }

    }
    m_fOffSet = OffSet;

    if (!m_pTargetCam)
        m_pTargetCam = m_pGameInstance->Get_Camera(_UINT(CAMERA::PLAYER_FOCUS_01));

}

void Client::UIObj_MonsterStatus::Clear_TargetObjectID()
{
    m_iTargetObjectID = UINT_MAX;
    m_pTargetTransform = nullptr;


    if (m_ProgressBars[_UINT(MONTSTERSTATUS_UI_TYPE::HPBAR)])
    {
        UIObj_HpBar* pHpBar = dynamic_cast<UIObj_HpBar*>(m_ProgressBars[_UINT(MONTSTERSTATUS_UI_TYPE::HPBAR)]);
        if (pHpBar)
        {
           // pHpBar->Clear_Owner_ObjectID();
            pHpBar->On_Dead(nullptr);
        }

    }

    if (m_ProgressBars[_UINT(MONTSTERSTATUS_UI_TYPE::GUARDBAR)])
    {
        UIObj_GuardBar* pGuardBar = dynamic_cast<UIObj_GuardBar*>(m_ProgressBars[_UINT(MONTSTERSTATUS_UI_TYPE::GUARDBAR)]);
        if (pGuardBar)
            pGuardBar->On_Dead();


    }

    Set_Active(false);

}

void Client::UIObj_MonsterStatus::Owner_Dead()
{
    Clear_TargetObjectID();
    //다시 풀링객체로 들어가!!


    Set_Dead(true);

}

void Client::UIObj_MonsterStatus::OnDestroyInLayer()
{
    m_pGameInstance->Return_PoolObject(_UINT(UI_POOLING_TYPE::MONSTERSTATUSBAR), this);

    m_bIsDead = false;
}


void Client::UIObj_MonsterStatus::Set_Dead(_bool _isalive)
{
    // GameObject::Set_Dead(_isalive);

}
