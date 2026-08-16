#include "Client_Define.h"
#include "UIObj_GlowButton.h"
#include "UIObj_Selector.h"
#include "UIObj_FadeScreen.h"
#include "UIObj_FocusMenu.h"
#include "UI_Text.h"
#include "DialogueManager.h"
#include "UISoundUtil.h"


Client::UIObj_GlowButton::UIObj_GlowButton()
{
}

Client::UIObj_GlowButton::UIObj_GlowButton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UIObject(pDevice,pContext)
{
}

Client::UIObj_GlowButton::UIObj_GlowButton(const UIObj_GlowButton& original)
    :UIObject(original)
{
}

Client::UIObj_GlowButton::~UIObj_GlowButton()
{
}


//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_GlowButton::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    return S_OK;
}

HRESULT Client::UIObj_GlowButton::Initialize(void* arg)
{
    CHECK_FAILED(__super::Initialize(arg), E_FAIL);

    if (m_wstrName == L"PressAnyKey")
        m_bGlowLine = false;

    return S_OK;
}


/******************************************************* 객체 준비 함수 *******************************************************/


_int Client::UIObj_GlowButton::Update_Priority(const _float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    return 0;
}

_int Client::UIObj_GlowButton::Update(const _float fTimeDelta)
{
    __super::Update(fTimeDelta);
    return 0;
}

_int Client::UIObj_GlowButton::Update_Late(const _float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
    return 0;
}

HRESULT Client::UIObj_GlowButton::Render(const _float fTimeDelta)
{
    __super::Render(fTimeDelta);
    return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////
UIObj_GlowButton* Client::UIObj_GlowButton::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    UIObj_GlowButton* pInstance = new UIObj_GlowButton(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_GlowButton 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}


GameObject* Client::UIObj_GlowButton::Clone(void* pArg)
{
    UIObj_GlowButton* pInstance = new UIObj_GlowButton(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_GlowButton 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}
void Client::UIObj_GlowButton::OnHoverEnter()
{
    __super::OnHoverEnter();

   //메뉴한테 전달만하자
    UIObj_FocusMenu::FocusMenuUpdateEvent Event;
    Event.iCurrentIdx = m_iIdx;
    Event.m_iObjID = m_pParent->Get_ObjectID();

    m_pGameInstance->Publish(Event);

    


}
void Client::UIObj_GlowButton::OnClick()
{
    __super::OnClick();


}

void Client::UIObj_GlowButton::OnHoverEnter_With_Keyboard()
{
    __super::OnHoverEnter();

}

void Client::UIObj_GlowButton::Execute_By_Event(const string& strActionName, void* pArg)
{
    //클릭시 호출되는함수 
    UI_MasterEvent* pMasterEvent = static_cast<UI_MasterEvent*>(pArg);

    if (strActionName == "QuitGame")
    {
        PostQuitMessage(0);
    }

    else if (strActionName == "Config")
    {
        ////씬이동( 커마테스트->메인씬)

        //UIObj_FadeScreen::FadeScreenEvent Event;
        //Event.eType = UIObj_FadeScreen::FadeScreenEventType::ENTER_SCENE;
        //Event.m_iNextLevel = LEVEL::CUSTOMIZE;
        //m_pGameInstance->Publish(Event);
    }

    
    else if (strActionName == "GameStart")
    {
        //씬이동(이벤트 지연처리)
        m_pGameInstance->Play_Sound("Title_Enter",UIVolume);

        UIObj_FadeScreen::FadeScreenEvent Event;
        Event.eType = UIObj_FadeScreen::FadeScreenEventType::ENTER_SCENE;
        Event.m_iNextLevel = LEVEL::CUSTOMIZE;
        m_pGameInstance->Publish(Event);


    }

    else if (strActionName == "OpenSceneChangeMenu")
    {
        //m_pGameInstance->Close_All_Menu();

        //씬이동 창 열기..
        UI_MasterEvent Event;
        Event.m_ActionName = "OpenWindow";
        Event.m_Text = L"SceneSelectMenu";
        Event.m_bPersistent = true;
        Event.m_EventTarget = UI_EVENT_TARGET::SYSTEM;
        m_pGameInstance->Publish(Event);

    }
    
    ///////////////////HoverEvent//////////////
    else if (strActionName == "ChangeTexture")
    {
        //FocusMenu에게 텍스처바구라고 명령-> FocusMenu가 scenemenu에게 전달->scenemnu가 slot에게전달
        UIObj_FocusMenu::FocusMenuToParentEvent Event;
        Event.m_iObjID = m_pParent->Get_ObjectID();//focus menu id 
        Event.m_Text = pMasterEvent->m_Text;
        m_pGameInstance->Publish(Event);
        
    }

    //////////npc데이터
    else if (strActionName == "SELECT_CHOICE")
    {
        //dialogue managerㅇ게 현재 ㅅ너택한 idex 를 넘기기
        PlayClickSound();
        DialogueManager::GetInstance()->Select_Choice(m_iIdx);

    }
 
    //levelup
    else if (strActionName == "LevelUp")
    {
        //dialogue managerㅇ게 현재 ㅅ너택한 idex 를 넘기기
        PlayOpenSound();
        UI_MasterEvent Event;
        Event.m_ActionName = "OpenWindow";
        Event.m_Text = L"Levelup_window";
        Event.m_bFlag = true;
        Event.m_bPersistent = true;
        Event.m_EventTarget = UI_EVENT_TARGET::SYSTEM;
        m_pGameInstance->Publish(Event);



    }
}




void Client::UIObj_GlowButton::Free()
{
    __super::Free();
}


/////////////////////////////////////////////////////////////////
void    Client::UIObj_GlowButton::Set_Text(wstring str)
{
    if (m_pTextComp)
        m_pTextComp->Set_Text(str);


}

void        Client::UIObj_GlowButton::After_ApplyData()
{
    __super::After_ApplyData();

    m_pTextComp = dynamic_cast<UI_Text*>(Get_Component_FromName(Proto_UIText));


}
