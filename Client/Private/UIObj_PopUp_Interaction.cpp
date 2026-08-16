#include "Client_Define.h"
#include "UIObj_PopUp_Interaction.h"
#include "UIObj_Text.h"
#include "UIObj_FadeScreen.h"
#include "IInteractable.h"
#include "InteractionManager.h"


Client::UIObj_PopUp_Interaction::UIObj_PopUp_Interaction()
{
}

Client::UIObj_PopUp_Interaction::UIObj_PopUp_Interaction(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UIObject(pDevice,pContext)
{
}

Client::UIObj_PopUp_Interaction::UIObj_PopUp_Interaction(const UIObj_PopUp_Interaction& original)
    :UIObject(original)
{
}

Client::UIObj_PopUp_Interaction::~UIObj_PopUp_Interaction()
{
}
//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_PopUp_Interaction::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    return S_OK;
}

HRESULT Client::UIObj_PopUp_Interaction::Initialize(void* arg)
{
    CHECK_FAILED(__super::Initialize(arg), E_FAIL);
    
    m_vecSubscribeNumbers.push_back(m_pGameInstance->Subscribe<PopUp_Interaction_Event>([this](const PopUp_Interaction_Event& Event)
        {
            if (m_eInteractionType != Event.m_eType)
                return;

            if (m_pInteraction_Text)
                m_pInteraction_Text->Set_Text(Event.m_Text);


        }));

    return S_OK;
}

_int Client::UIObj_PopUp_Interaction::Update_Priority(const _float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    return 0;
}

_int Client::UIObj_PopUp_Interaction::Update(const _float fTimeDelta)
{
    __super::Update(fTimeDelta);
    if (m_pGameInstance->KeyDown(DIK_E))
    {
        //셀렉트 판넬 키기
        if (m_pPanel_Select)
        {
            m_pPanel_Select->Set_Visible(true);
            m_pPanel_Select->Set_Active(true);

        }


        //행동처리하기.
        Do_Interaction();
    }

    else if (m_pGameInstance->KeyUp(DIK_E))
    {
        if (m_pPanel_Select)
        {

            m_pPanel_Select->Set_Active(false);
            m_pPanel_Select->Set_Visible(false);
        
            Set_Active(false);
        }
            

    }
    return 0;
}

_int Client::UIObj_PopUp_Interaction::Update_Late(const _float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
    return 0;
}

HRESULT Client::UIObj_PopUp_Interaction::Render(const _float fTimeDelta)
{
    if (m_pPanel->Get_Alpha() <= 0.001f)
        return S_OK;

    __super::Render(fTimeDelta);

    //COUT("PanelAlpha:" << m_pPanel->Get_Alpha());

    return 0;
}

UIObj_PopUp_Interaction* Client::UIObj_PopUp_Interaction::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    UIObj_PopUp_Interaction* pInstance = new UIObj_PopUp_Interaction(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_PopUp_Interaction 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}


GameObject* Client::UIObj_PopUp_Interaction::Clone(void* pArg)
{
    UIObj_PopUp_Interaction* pInstance = new UIObj_PopUp_Interaction(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_PopUp_Interaction 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}

void Client::UIObj_PopUp_Interaction::Free()
{
    __super::Free();

}

void Client::UIObj_PopUp_Interaction::Do_Interaction()
{
    switch (m_eInteractionType)
    {
    case Client::UIObj_PopUp_Interaction::ITME_PICKUP:
    {
        //아이템얻기!!
        //아이템과 충돌하면 이벤트로 아이템 이 클래스로보내고, 아이템얻기 하도록 invnetorymanager함수호출
    }
        break;

    case Client::UIObj_PopUp_Interaction::SAVEPOINT:
    {
        //겨우살이 menu띄우기(페이드인 이벤트 먼저)->페이드인이 메뉴열기수행
        UIObj_FadeScreen::FadeScreenEvent FadeEvent;
        FadeEvent.eType = UIObj_FadeScreen::FadeScreenEventType::OPEN_MENU;
        FadeEvent.m_UIName =L"Mistletoe_Menu";
        FadeEvent.eMode = UI_MODE::SAVEPOINTMENU;
        FadeEvent.m_fSecond = 2.f;

        m_pGameInstance->Publish(FadeEvent);
        
        
    }
        break;

    default:
        break;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
void        Client::UIObj_PopUp_Interaction::After_ApplyData()
{
    __super::After_ApplyData();
    
    //타입지정..
    if (m_UIType == "ITEMPICKUP")
        m_eInteractionType = Interaction_Type::ITME_PICKUP;

    else if(m_UIType=="SAVEPOINT")
        m_eInteractionType = Interaction_Type::SAVEPOINT;

    
    else if (m_UIType == "LADDER_UP")
        m_eInteractionType = Interaction_Type::LADDER_UP;

    else if (m_UIType == "NPC")
        m_eInteractionType = Interaction_Type::NPC;


    else if (m_UIType == "CHEST")
        m_eInteractionType = Interaction_Type::CHEST;


    m_Progress = dynamic_cast<UI_Progress*>(Get_Component_FromName(Proto_UIProgress));



    m_pLeft = Get_Child(L"Left");
    m_pRight = Get_Child(L"Right");

    m_pInteraction_Text = dynamic_cast<UIObj_Text*>(Get_Child(L"Interaction_Text"));

    m_pKey = Get_Child(L"Interaction_Key");

    m_pPanel = Get_Child(L"Panel");

    m_pPanel_Select = Get_Child(L"Panel_Select");



}

void Client::UIObj_PopUp_Interaction::Set_Active(_bool _isActive)
{
    
    if (m_bIsActive == _isActive)
        return;

    //활성화되었을때 할일..
    if (_isActive)
    {
        m_Progress->Reset();
        m_Progress->Set_TargetRatio(1.f);

        if (m_pPanel)
        {
            m_pPanel->Set_Alpha(0.f);
            m_pPanel->Play_Animation(ShowAnimKey);
        }
       

    }

    else
    {
        if (m_pPanel)
            m_pPanel->Set_Alpha(0.f);

        if (m_pPanel_Select)
            m_pPanel_Select->Set_Visible(false);
    }

    
    __super::Set_Active(_isActive);



}