#include "Client_Define.h"
#include "UIObj_GuardBar.h"
#include "UI_Animation.h"
#include "UIObj_ProgressBar.h"
#include "UIObj_Stamina.h"

Client::UIObj_GuardBar::UIObj_GuardBar()
{
}

Client::UIObj_GuardBar::UIObj_GuardBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UIObject(pDevice,pContext)
{
}

Client::UIObj_GuardBar::UIObj_GuardBar(const UIObj_GuardBar& original)
    :UIObject(original)
{
}

Client::UIObj_GuardBar::~UIObj_GuardBar()
{
}

//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_GuardBar::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;


    return S_OK;
}

HRESULT Client::UIObj_GuardBar::Initialize(void* arg)
{
    CHECK_FAILED(__super::Initialize(arg), E_FAIL);
    
    
    m_pGameInstance->Subscribe<GuardBarUIEvent>([this](const GuardBarUIEvent& Event)
        {
            if (Event.m_iOwnerID == m_iOwnerObjectID)
            {
                OnGuardEvent(Event);
            }
        });
    m_Arrows.resize(_UINT(ArrowType::END));   

    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////


_int Client::UIObj_GuardBar::Update_Priority(const _float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    return 0;
}

_int Client::UIObj_GuardBar::Update(const _float fTimeDelta)
{
    if (m_bPlayFullEffect)
    {
       Move_Arrow(0.f, MoveType::DECREASE);

        //완료체크하기
        bool bResult = Check_AnimationEnd();
        if (bResult)
            End_Animation();


    }

    __super::Update(fTimeDelta);
    return 0;
}

_int Client::UIObj_GuardBar::Update_Late(const _float fTimeDelta)
{
    if (m_fCurrentMoveRatio==1)
    {
        if (!m_bPlayFullEffect)
        {
            m_bPlayFullEffect = true;
            Active_GuageFullEffect();
          
        }
    }


    __super::Update_Late(fTimeDelta);
    return 0;
}

HRESULT Client::UIObj_GuardBar::Render(const _float fTimeDelta)
{
    __super::Render(fTimeDelta);
    return 0;
}

UIObj_GuardBar* Client::UIObj_GuardBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    UIObj_GuardBar* pInstance = new UIObj_GuardBar(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_GuardBar 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}


GameObject* Client::UIObj_GuardBar::Clone(void* pArg)
{
    UIObj_GuardBar* pInstance = new UIObj_GuardBar(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_GuardBar 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}

void Client::UIObj_GuardBar::Free()
{
    for (auto& pArrow : m_Arrows)
    {
        if(pArrow.m_pArrow)
            Safe_Release(pArrow.m_pArrow);

    }
    m_Arrows.clear();

    ///////////////
    for (auto& pObj : m_ProgressBars)
          Safe_Release(pObj);

    m_ProgressBars.clear();

    Safe_Release(m_HitRate);
    Safe_Release(m_GuageFullEffect);

    __super::Free();
}

void Client::UIObj_GuardBar::Move_Arrow(_float fValue, MoveType eType)
{
    //움직일 비율 설정
    m_fCurrentMoveRatio = *m_fCurrnetPoint / *m_fMaxPoint;

    if (eType == MoveType::INCREASE)
    {
     
        float fSign = 1.f;      //왼 ,오 부호결정

        /*Max값까지만 이동*/
        //Arrow들 옮겨주기
        for (_uint i = 0; i < _UINT(ArrowType::END); ++i)
        {
            ArrowInfo Info = m_Arrows[i];
            _float TargetPos = 0.f;
            if (i == 0)
            {
                TargetPos = Info.m_InitLocalPosX + (m_fMaxGuardDistance * m_fCurrentMoveRatio);
                TargetPos = clamp(TargetPos, Info.m_InitLocalPosX, Info.m_MaxLocalPosX);
            }

            else
            {
                TargetPos = Info.m_InitLocalPosX + (m_fMaxGuardDistance *-1* m_fCurrentMoveRatio);
                TargetPos = clamp(TargetPos, Info.m_MaxLocalPosX, Info.m_InitLocalPosX);
            }


            UIObject* pObj = m_Arrows[i].m_pArrow;
            if (pObj)
            {
                pObj->Set_Position(TargetPos, pObj->Get_LocalTransform().m_fY);
            }
        }

    }
  
    else
    {

        float fSign = 1.f;      //왼 ,오 부호결정
        /*Init값까지만 이동*/
        //Arrow들 옮겨주기
        for (_uint i = 0; i < _UINT(ArrowType::END); ++i)
        {
            ArrowInfo Info = m_Arrows[i];
            _float TargetPos = 0.f;
            if (i == 0)
            {
                TargetPos = Info.m_InitLocalPosX + (m_fMaxGuardDistance* m_ProgressBars[0]->Get_RenderRatio(true)) + 0.3f;
                TargetPos = clamp(TargetPos, Info.m_InitLocalPosX, Info.m_MaxLocalPosX);
            }

            else
            {
                TargetPos = Info.m_InitLocalPosX + (m_fMaxGuardDistance * m_ProgressBars[0]->Get_RenderRatio(true)*-1.f) -0.3f;
                TargetPos = clamp(TargetPos, Info.m_MaxLocalPosX, Info.m_InitLocalPosX);
            }


            UIObject* pObj = m_Arrows[i].m_pArrow;
            if (pObj)
            {
                pObj->Set_Position(TargetPos, pObj->Get_LocalTransform().m_fY);
            }
        }
    }
  
}

void Client::UIObj_GuardBar::Blink_HitRate()
{
    //켰다가 꺼주기(꺼주면서 애니메이션실행됨)
    m_HitRate->Set_Visible(true);
    m_HitRate->Set_Active(true);
    m_HitRate->Set_Active(false);

}

void Client::UIObj_GuardBar::Active_GuageFullEffect()
{
    //삥!!빛나는 파란색
    m_GuageFullEffect->Set_Visible(true);
    m_GuageFullEffect->Set_Active(true);
    m_GuageFullEffect->Set_Active(false);

    //Fullguage 활성화
    //색변화 애니메이션 설정하기
    for (auto& pObj : m_ProgressBars)
    {
        pObj->Set_Visible(true);
        pObj->Set_Active(true);
        pObj->Set_UseRandomColor(true,true);
        pObj->Set_MinHue(true, 156);
        pObj->Set_MaxHue(true, 255);
    }
}

bool Client::UIObj_GuardBar::Check_AnimationEnd()
{

    bool bEnd = true;
    if (m_fCurrentMoveRatio != 0.f)
        bEnd &= false;

    //arrow가 모두 도착했는지 판단.
    for (auto& pArrow : m_Arrows)
    {

        UITransform pTransform = pArrow.m_pArrow->Get_LocalTransform();
        if (fabs(pTransform.m_fX - pArrow.m_InitLocalPosX) > 1.f)
            bEnd &= false;

        else
        {
            pTransform.m_fX = pArrow.m_InitLocalPosX;
            pArrow.m_pArrow->Set_Position(pTransform.m_fX, pTransform.m_fY);
        }

          
    }

    return bEnd;
}

void Client::UIObj_GuardBar::End_Animation()
{
    m_bPlayFullEffect = false;
    for (auto& pProgressBar : m_ProgressBars)
    {
        pProgressBar->Set_Visible(false);
        pProgressBar->Set_Active(false);

    }

}

void Client::UIObj_GuardBar::OnGuardEvent(GuardBarUIEvent e)
{
  
	CHECK_TRUE(m_bPlayFullEffect);
	m_fCurrentMoveRatio = e.m_fValue / m_fMaxGuardDistance;
	Move_Arrow(0.f, MoveType::INCREASE);
	Blink_HitRate();

}

void Client::UIObj_GuardBar::Set_TargetValue(float* pTarget)
{
    m_fCurrnetPoint = pTarget;


    //자식에게도 전파
    for (auto& pObj : m_ProgressBars)
        pObj->Set_CurrentFloat(m_fCurrnetPoint);

}

void Client::UIObj_GuardBar::Set_MaxValue(float* pMax)
{  
    m_fMaxPoint = pMax;

    //자식에게도 전파
    for (auto& pObj : m_ProgressBars)
        pObj->Set_MaxFloat(m_fMaxPoint);

}


///////////////////////////////////////////////////////////////////////////////////////////////////////
void        Client::UIObj_GuardBar::After_ApplyData()
{
    for (auto& pChild : m_Arrows)
    {
        CHECK_TRUE(pChild.m_pArrow != nullptr);
    }

    CHECK_TRUE(m_HitRate != nullptr);
    CHECK_TRUE(m_GuageFullEffect != nullptr);

    /*자식 캐싱*/
    UIObject* pLeftArrow = Get_Child(L"Arrow_Left");
    if (pLeftArrow)
    {
        ArrowInfo arrowInfo;
        arrowInfo.m_pArrow = pLeftArrow;
        UITransform pTransform = pLeftArrow->Get_LocalTransform();
        arrowInfo.m_InitLocalPosX = pTransform.m_fX;
        arrowInfo.m_CurrentLocalPosX = arrowInfo.m_InitLocalPosX;

                                                            //최대거리
        arrowInfo.m_MaxLocalPosX =arrowInfo.m_InitLocalPosX + m_fMaxGuardDistance;


        m_Arrows[_UINT(ArrowType::LEFTARROW)] = arrowInfo;
        Safe_AddRef(pLeftArrow);
    }

    UIObject* pRightArrow = Get_Child(L"Arrow_Right");
    if (pRightArrow)
    {
        ArrowInfo arrowInfo;
        arrowInfo.m_pArrow = pRightArrow;
        UITransform pTransform = pRightArrow->Get_LocalTransform();
        arrowInfo.m_InitLocalPosX = pTransform.m_fX;
        arrowInfo.m_CurrentLocalPosX = arrowInfo.m_InitLocalPosX;

        //최대거리
        arrowInfo.m_MaxLocalPosX = arrowInfo.m_InitLocalPosX - m_fMaxGuardDistance;

        m_Arrows[_UINT(ArrowType::RIGHTARROW)] = arrowInfo;
        Safe_AddRef(pRightArrow);
    }

    UIObject* pHitRate = Get_Child(L"HitRate");
    if (pHitRate)
    {
        m_HitRate = pHitRate;
        Safe_AddRef(m_HitRate);
    }


    UIObject* pGuageFullEffect = Get_Child(L"GuageFullEffect");
    if (pGuageFullEffect)
    {
        m_GuageFullEffect = pGuageFullEffect;
        Safe_AddRef(m_GuageFullEffect);

    }

    //꽉찼을때나오는 Progressbar 캐싱.
    UIObject* pProgress_Left = Get_Child(L"Progress_Left");
    if (pProgress_Left)
    {
        UIObj_ProgressBar* pObj = dynamic_cast<UIObj_ProgressBar*>(pProgress_Left);
        if (pObj)
        {
            m_ProgressBars.push_back(pObj);
            Safe_AddRef(pProgress_Left);
        }

    }

    UIObject* pProgress_Right = Get_Child(L"Progress_Right");
    if (pProgress_Right)
    {
        UIObj_ProgressBar* pObj = dynamic_cast<UIObj_ProgressBar*>(pProgress_Right);
        if (pObj)
        {
            m_ProgressBars.push_back(pObj);
            Safe_AddRef(pProgress_Right);

        }


    }
}
void Client::UIObj_GuardBar::On_Dead()
{
    m_iOwnerObjectID = UINT_MAX;
    m_fCurrnetPoint = nullptr;
    m_fMaxPoint = nullptr;

    Set_Active(false);

}
/******************************************************* 객체 준비 함수 *******************************************************/

void Client::UIObj_GuardBar::Execute_By_Event(const string& strActionName, void* pArg)
{
    
}
