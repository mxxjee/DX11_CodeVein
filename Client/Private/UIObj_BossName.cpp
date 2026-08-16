#include "Client_Define.h"
#include "UIObj_BossName.h"
#include "UI_Progress.h"
#include "UIObj_Text.h"
#include "UIObj_HpBar.h"

#include "Monster_Boss_Intro.h"

Client::UIObj_BossName::UIObj_BossName()
{
}

Client::UIObj_BossName::UIObj_BossName(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UIObject(pDevice,pContext)
{
}

Client::UIObj_BossName::UIObj_BossName(const UIObj_BossName& original)
    :UIObject(original)
{
}

Client::UIObj_BossName::~UIObj_BossName()
{
}



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_BossName::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    return S_OK;
}

HRESULT Client::UIObj_BossName::Initialize(void* arg)
{
    CHECK_FAILED(__super::Initialize(arg), E_FAIL);

    EventHandle eventHandle;
    eventHandle = m_pGameInstance->Subscribe< BossNameUIEVent>([this](const BossNameUIEVent& Event)
        {
            m_eTargetBossType = Event.m_eBossType;

            if (m_pText_Korean)
            {
                m_pText_Korean->Set_Position(Event.Korean_LocalPos.x, Event.Korean_LocalPos.y);

                m_pText_Korean->Set_Text(Event.BossName_Korean);
                m_pText_Korean->Set_Spacing(Event.Korean_Spacing);

            }
            if (m_pText_English)
            {
                m_pText_English->Set_Position(Event.English_LocalPos.x, Event.English_LocalPos.y);


                m_pText_English->Set_Text(Event.BossName_English);
                m_pText_English->Set_Spacing(Event.English_Spacing);
            }
              

            Set_Active(Event.bActive);

        });

    m_vecSubscribeNumbers.push_back(eventHandle);

    VerticalLineActive.Limit = 0.5f;
    VerticalLineActive.m_AlarmFunc = [this]()
        {
            if (m_pVerticalLine)
            {
                m_pVerticalLine->Set_Visible(true);
                m_pVerticalLine->Set_Active(true);
                m_pVerticalLine->Set_Alpha(1.f);


                m_pVerticleProgress->Set_TargetRatio(1.f);


            }

            if (m_pNameBack)
            {
                m_pNameBack->Set_Visible(true);
                m_pNameBack->Set_Active(true);

                m_pNameBack->Play_Animation(m_ShowAnim);
            }

            VerticalLineActive.Off();
        };

    VerticalLineActive.Off();

    /////////////////////////////////////////
    LineActive.Limit = 0.8f;
    LineActive.m_AlarmFunc = [this]()
        {
            if (m_pLine)
            {
                m_pLine->Set_Visible(true);
                m_pLine->Set_Active(true);

                if(m_pLineProgress)
                    m_pLineProgress->Set_TargetRatio(1.f);


            }

            if (m_pLeft)
            {
                m_pLeft->Set_Visible(true);
                m_pLeft->Set_Active(true);
                m_pLeft->Play_Animation(m_ShowAnim);

            }

            if (m_pRight)
            {
                m_pRight->Set_Visible(true);
                m_pRight->Set_Active(true);
                m_pRight->Play_Animation(m_ShowAnim);

            }

            LineActive.Off();
        };

    LineActive.Off();

    /////////////////////////////////////////
    TextActive.Limit = 1.5f;
    TextActive.m_AlarmFunc = [this]()
        {
            if (m_pText_English)
            {
                m_pText_English->Set_Alpha(0.f);
                m_pText_English->Set_Visible(true);
                m_pText_English->Set_Active(true);
                m_pText_English->Play_Animation(m_ShowAnim);

            }

            if (m_pText_Korean)
            {
                m_pText_Korean->Set_Alpha(0.f);

                m_pText_Korean->Set_Visible(true);
                m_pText_Korean->Set_Active(true);
                m_pText_Korean->Play_Animation(m_ShowAnim);

            }

            TextActive.Off();
        };

    TextActive.Off();


    HideAlarm.Limit = 5.f;
    HideAlarm.m_AlarmFunc = [this]()
        {
            Play_Animation(m_HideAnim);
            HideAlarm.Off();

        };

    HideAlarm.Off();
    //////////////////////
    ActiveFalseAlarm.Limit = 7.f;
    ActiveFalseAlarm.m_AlarmFunc = [this]()
        {
            Set_Active(false);
            ActiveFalseAlarm.Off();

        };

    ActiveFalseAlarm.Off();


    m_SoundAlarm.Limit = 0.5f;
    m_SoundAlarm.m_AlarmFunc = [this]()
        {
            //이름표 나오는 순간 단 한번ㄴ만 나와야됨. 조정해주세요...
            m_pGameInstance->Play_Sound("Boss_Title", 0.7f, false);
            m_bAfterShow = true;
            m_SoundAlarm.Off();
        };

    m_SoundAlarm.Off();


    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
_int Client::UIObj_BossName::Update_Priority(const _float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    return 0;
}

_int Client::UIObj_BossName::Update(const _float fTimeDelta)
{
    __super::Update(fTimeDelta);
    m_SoundAlarm.Update(fTimeDelta);

    VerticalLineActive.Update(fTimeDelta);
    LineActive.Update(fTimeDelta);
    TextActive.Update(fTimeDelta);
    HideAlarm.Update(fTimeDelta);
    ActiveFalseAlarm.Update(fTimeDelta);

   
    return 0;
}

_int Client::UIObj_BossName::Update_Late(const _float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
    return 0;

}

HRESULT Client::UIObj_BossName::Render(const _float fTimeDelta)
{
    __super::Render(fTimeDelta);
    return 0;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
void        Client::UIObj_BossName::After_ApplyData()
{
    //자식캐싱
    __super::After_ApplyData();

    m_pNameBack = Get_Child(L"NameBack");
    m_pLine = Get_Child(L"Line");
    if (m_pLine)
        m_pLineProgress = dynamic_cast<UI_Progress*>(m_pLine->Get_Component_FromName(Proto_UIProgress));

    m_pLogo= Get_Child(L"Logo");
    m_pVerticalLine= Get_Child(L"VerticalLine");
    if (m_pVerticalLine)
        m_pVerticleProgress = dynamic_cast<UI_Progress*>(m_pVerticalLine->Get_Component_FromName(Proto_UIProgress));


    m_pLeft = Get_Child(L"Logo_Left");
    m_pRight = Get_Child(L"Logo_Right");


    m_pText_English = dynamic_cast<UIObj_Text*>(Get_Child(L"Text_English"));
    m_pText_Korean = dynamic_cast<UIObj_Text*>(Get_Child(L"Text_Korean"));

}
void Client::UIObj_BossName::Set_Active(_bool _isActive)
{
    if (_isActive)
    {
        VerticalLineActive.Off();
        LineActive.Off();
        TextActive.Off();
        HideAlarm.Off();
        ActiveFalseAlarm.Off();
        m_SoundAlarm.Off();

        Reset();

        VerticalLineActive.On();
        LineActive.On();
        TextActive.On();
        HideAlarm.On();
        ActiveFalseAlarm.On();


        m_SoundAlarm.On();
       
        m_bIsVisible = true;

      
    }

    //보스 이름활성화!!
    else
    {
        if (m_bAfterShow)
        {
            UI_MasterEvent Event;
            Event.m_EventTarget = UI_EVENT_TARGET::SYSTEM;
            Event.m_ActionName = "ActiveUI";
            Event.m_bFlag = true;
            Event.m_Text = L"Boss_HPBar";


            m_pGameInstance->Publish(Event);

            //hpbar에게도 오너이름연결
            UIObj_HpBar::HpBarEvent HpEvent;
            HpEvent.eType = UIObj_HpBar::EVENTTYPE::INITIALIZE;

            wstring Text = m_pText_Korean->Get_Text();
            HpEvent.pArg = &Text;

            m_pGameInstance->Publish(HpEvent);


            if (m_eTargetBossType == BOSS_TYPE::OLIVER)
            {
                //보스에게 이벤트끝전달
                Monster_Boss_Intro::BossIntroEvent IntroEvent;
                IntroEvent.bFinish = true;

                m_pGameInstance->Publish(IntroEvent);
            }


            m_bAfterShow = false;

        }


       

    }
    __super::Set_Active(_isActive);

  




}


void Client::UIObj_BossName::Reset()
{
    
    VerticalLineActive.Off();
    VerticalLineActive.Elapsed = 0.f;

    LineActive.Off();
    LineActive.Elapsed = 0.f;

    TextActive.Off();
    TextActive.Elapsed = 0.f;

    HideAlarm.Off();
    HideAlarm.Elapsed = 0.f;

    ActiveFalseAlarm.Off();
    ActiveFalseAlarm.Elapsed = 0.f;

    m_SoundAlarm.Off();

    if (m_pNameBack)
    {
        m_pNameBack->Set_Alpha(0.f);
        m_pNameBack->Set_Visible(false);
        m_pNameBack->Set_Active(false);

        m_pNameBack->Set_Alpha(0.f);
        m_pNameBack->Set_CombinedAlpha(1.f);
        
        m_pNameBack->Reset_AnimationComp();

    }

    if (m_pLine)
    {
        m_pLine->Set_Visible(false);
        m_pLine->Set_Active(false);
        if (m_pLineProgress)
            m_pLineProgress->Reset();
        m_pLine->Reset_AnimationComp();
    }

    if (m_pLogo)
    {
        m_pLogo->Set_Visible(true);
        m_pLogo->Set_Active(false);
        m_pLogo->Set_Alpha(0.f);
        m_pLogo->Set_CombinedAlpha(1.f);
        m_pLogo->Reset_AnimationComp();
    }
    if (m_pVerticalLine)
    {
        m_pVerticalLine->Set_Visible(false);
        m_pVerticalLine->Set_Active(false);
        m_pVerticalLine->Set_Alpha(0.f);
        m_pVerticalLine->Set_CombinedAlpha(1.f);
        m_pVerticalLine->Reset_AnimationComp();
    }
    if (m_pLeft)
    {
        m_pLeft->Set_Visible(false);
        m_pLeft->Set_Active(false);
        m_pLeft->Set_Alpha(0.f);
        m_pLeft->Set_CombinedAlpha(1.f);
        m_pLeft->Reset_AnimationComp();
    }
    if (m_pRight)
    {
        m_pRight->Set_Visible(false);
        m_pRight->Set_Active(false);
        m_pRight->Set_Alpha(0.f);
        m_pRight->Set_CombinedAlpha(1.f);
        m_pRight->Reset_AnimationComp();
    }
       

    if (m_pText_English)
    {
        m_pText_English->Set_Visible(false);
        m_pText_English->Set_Active(false);
        m_pText_English->Set_Alpha(0.f);
        m_pText_English->Set_CombinedAlpha(1.f);
        m_pText_English->Reset_AnimationComp();
    }


    if (m_pText_Korean)
    {
        m_pText_Korean->Set_Visible(false);
        m_pText_Korean->Set_Active(false);
        m_pText_Korean->Set_Alpha(0.f);
        m_pText_Korean->Set_CombinedAlpha(1.f);
        m_pText_Korean->Reset_AnimationComp();

    }
       


    if (m_pVerticleProgress)
        m_pVerticleProgress->Reset();

    Reset_AnimationComp();
    Set_Alpha(1.f);
    Set_CombinedAlpha(1.f);
  
    

}


UIObj_BossName* Client::UIObj_BossName::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    UIObj_BossName* pInstance = new UIObj_BossName(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_BossName 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}

GameObject* Client::UIObj_BossName::Clone(void* pArg)
{
    UIObj_BossName* pInstance = new UIObj_BossName(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_BossName 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}
void Client::UIObj_BossName::Free()
{
    __super::Free();
}
