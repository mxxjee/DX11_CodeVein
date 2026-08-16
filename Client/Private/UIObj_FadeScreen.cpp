#include "Client_Define.h"
#include "UIObj_FadeScreen.h"
#include "UI_Animation.h"
#include "UIObj_Cursor.h"
#include "Player.h"
#include "InteractionManager.h"

Client::UIObj_FadeScreen::UIObj_FadeScreen()
{
}

Client::UIObj_FadeScreen::UIObj_FadeScreen(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UIObject(pDevice,pContext)
{
}

Client::UIObj_FadeScreen::UIObj_FadeScreen(const UIObj_FadeScreen& original)
    :UIObject(original)
{
}

Client::UIObj_FadeScreen::~UIObj_FadeScreen()
{
}



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_FadeScreen::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    return S_OK;
}

HRESULT Client::UIObj_FadeScreen::Initialize(void* arg)
{

    m_vecSubscribeNumbers.push_back(m_pGameInstance->Subscribe<FadeScreenEvent>([this](const FadeScreenEvent& event)
        {
           
            FadeOutClip->Reset_EndFunction();

            Set_Active(true);
            m_Alarm.Off();

            switch (event.eType)
            {
            case FadeScreenEventType::ENTER_SCENE:
                m_iNextLevel = event.m_iNextLevel;
                m_bSavePoint = event.m_bSavePoint;

                m_CurrentEventType = FadeScreenEventType::ENTER_SCENE;
                m_bForceLoad = event.m_bForceLoad;

                if (event.m_fSecond > 0.f)
                {
                    m_Alarm.Limit = event.m_fSecond;
                    m_Alarm.m_AlarmFunc = [this]()
                        {
                            Play_FadeOut();
                            m_Alarm.Off();
                        };

                    m_Alarm.On();
                }

                else
                        Play_FadeOut();

    

                break;

            case FadeScreenEventType::AUTO_FADE:
                m_CurrentEventType = FadeScreenEventType::AUTO_FADE;
                if (FadeOutClip)
                    FadeOutClip->Bind_EndFunction(event.m_EndFunc);

                m_fSecond = event.m_fSecond;
                m_bSavePoint = event.m_bSavePoint;

                Play_FadeOut();
               /* if (event.m_fSecond > 0.f)
                {
                    m_Alarm.Limit = event.m_fSecond;
                    m_Alarm.m_AlarmFunc = [this,event]()
                        {
                            Play_FadeIn();
                            m_Alarm.Off();
                        };

                    m_Alarm.On();
                }*/

                break;

            case FadeScreenEventType::EXIT_SCENE:
                
                m_CurrentEventType = FadeScreenEventType::EXIT_SCENE;
                if (event.m_fSecond > 0.f)
                {
                    m_Alarm.Limit = event.m_fSecond;
                    m_Alarm.m_AlarmFunc = [this]()
                        {
                           
                            Play_FadeIn();
                            m_bEnterExit = true;
                            m_Alarm.Off();
                        };

                    m_Alarm.On();
                }

                else
                    Play_FadeIn();
                break;

            case FadeScreenEventType::OPEN_MENU:
            {
                m_bSavePoint = false;
                //메뉴를 열었을땐 일단 fadeout시키고, n초뒤에 알아서 메뉴오픈+페이드인
                Play_FadeOut();
                m_CurrentEventType = FadeScreenEventType::OPEN_MENU;
                if (event.m_fSecond > 0.f)
                {
                    m_Alarm.Limit = event.m_fSecond;
                    m_Alarm.m_AlarmFunc = [this,FadeEvent=event]()
                        {
                           
                            UI_MasterEvent Event;
                            Event.m_ActionName = "OpenWindow";
                            Event.m_Text = FadeEvent.m_UIName;
                            Event.m_bFlag = true;
                            Event.m_EventTarget = UI_EVENT_TARGET::SYSTEM;

                            m_pGameInstance->Publish(Event);

                            Play_FadeIn();


                            m_pGameInstance->Change_UIMode(FadeEvent.eMode);
                            m_Alarm.Off();


                        };

                    m_Alarm.On();
                }
                    
            }
                break;
            
            
            default:
                break;
            }

        }));
    CHECK_FAILED(__super::Initialize(arg), E_FAIL);



    
    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////




_int Client::UIObj_FadeScreen::Update_Priority(const _float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    return 0;
}

_int Client::UIObj_FadeScreen::Update(const _float fTimeDelta)
{
    __super::Update(fTimeDelta);
    m_Alarm.Update(fTimeDelta);

    if (m_CurrentEventType == FadeScreenEventType::ENTER_SCENE)
    {
        //애님에ㅣ션끝나면 씬이동시키기 
        if (!m_pUIAnimComp->Is_Playing())
        {
            Set_Alpha(1.f);

            LevelChangeEvent Event;
            Event.eNextLevel = m_iNextLevel;
            m_pGameInstance->Publish_Stack(Event);


            m_CurrentEventType = FadeScreenEventType::END;
            m_bForceLoad = false;

        }
    }
    else if (m_CurrentEventType == FadeScreenEventType::AUTO_FADE)
    {
        //애님에ㅣ션끝나면 endfunc시키기
        if (!m_pUIAnimComp->Is_Playing())
        {
            Set_Alpha(1.f);
            m_Alarm.Limit = m_fSecond;
            m_Alarm.m_AlarmFunc = [this]()
                {

                    Play_FadeIn();
                    m_bEnterExit = true;
                    m_Alarm.Off();
                };

            m_Alarm.On();
            m_CurrentEventType = FadeScreenEventType::END;
        }
    }


    return 0;
}

_int Client::UIObj_FadeScreen::Update_Late(const _float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);

    CHECK_NULL_RESULT(m_pUIAnimComp, 0);
    CHECK_TRUE_RESULT(m_bForceLoad,0);


 

    return 0;
}

HRESULT Client::UIObj_FadeScreen::Render(const _float fTimeDelta)
{


    __super::Render(fTimeDelta);
    return 0;
}

void Client::UIObj_FadeScreen::Play_FadeIn()
{
    Play_Animation("FadeIn");
    


}

void Client::UIObj_FadeScreen::Play_FadeOut()
{
    Set_Alpha(0.f);

    Play_Animation("FadeOut");
    MOUSELOCK_EVENT mouseEvent;
    mouseEvent.bLock = true;
    m_pGameInstance->Publish(mouseEvent);

    UIObj_Cursor::CursorEvent cursorEvent;
    cursorEvent.bEnable = false;
    m_pGameInstance->Publish(cursorEvent);


}

///////////////////////////////////////////////////////////////////////////////////////////////////////
void        Client::UIObj_FadeScreen::After_ApplyData()
{
    Component*  pComp= Get_Component_FromName(Proto_UIAnimation);
    if (pComp)
    {
        m_pUIAnimComp = dynamic_cast<UI_Animation*>(pComp);

        FadeInClip = m_pUIAnimComp->Get_AnimationClip("FadeIn");
        if (FadeInClip)
            FadeInClip->Bind_EndFunction([this]()
                {
                    if (m_bSavePoint)
                    {
                        PlayerCheckPointEvnet CheckPointEvent;
                        CheckPointEvent.eCheckPointType = CheckPointEventType::END;
                        m_pGameInstance->Publish(CheckPointEvent);

                        InteractionManager::GetInstance()->Set_SceneChanging(false);
                    }

                    Set_Active(false);


                });


        FadeOutClip = m_pUIAnimComp->Get_AnimationClip("FadeOut");
        if (FadeOutClip)
        {
            //세이브 포인트를 통해서 FadeOut이 진행되었냐
            FadeOutClip->Bind_EndFunction([this]()
                {

                });
        }

    }




}
///////////////////////////////////////////////////////////////////////////////////////////////////////


UIObj_FadeScreen* Client::UIObj_FadeScreen::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    UIObj_FadeScreen* pInstance = new UIObj_FadeScreen(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_FadeScreen 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}


GameObject* Client::UIObj_FadeScreen::Clone(void* pArg)
{
    UIObj_FadeScreen* pInstance = new UIObj_FadeScreen(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_FadeScreen 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}

void Client::UIObj_FadeScreen::Free()
{
    __super::Free();
}
