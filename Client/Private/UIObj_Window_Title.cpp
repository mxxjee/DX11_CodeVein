#include "Client_Define.h"
#include "UIObj_Window_Title.h"
#include "UIObj_Selector.h"

#include "UIObj_FocusMenu.h"
#include "UIObj_FadeScreen.h"

#include "UISoundUtil.h"

Client::UIObj_Window_Title::UIObj_Window_Title()
{
}

Client::UIObj_Window_Title::UIObj_Window_Title(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UIObject(pDevice, pContext)
{
}

Client::UIObj_Window_Title::UIObj_Window_Title(const UIObj_Window_Title& original)
    :UIObject(original)
{
}

Client::UIObj_Window_Title::~UIObj_Window_Title()
{
}

//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_Window_Title::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    return S_OK;
}

HRESULT Client::UIObj_Window_Title::Initialize(void* arg)
{
    CHECK_FAILED(__super::Initialize(arg), E_FAIL);

    return S_OK;
}


/******************************************************* 객체 준비 함수 *******************************************************/


_int Client::UIObj_Window_Title::Update_Priority(const _float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    return 0;
}

_int Client::UIObj_Window_Title::Update(const _float fTimeDelta)
{
    __super::Update(fTimeDelta);
    return 0;
}

_int Client::UIObj_Window_Title::Update_Late(const _float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
    if (m_bStartInit)
    {
        if (m_pGameInstance->IsAnyKeyPressed())
        {
           PlayClickSound();

            m_bStartInit = false;
            //다음메뉴불러오기
            if(m_pPressAnyKey)
                m_pPressAnyKey->Set_Active(false);

            if (m_pFocusMenu)
                m_pFocusMenu->Set_Active(true);
        }
    
    }

    /*else
    {
        if(m_pGameInstance->KeyDown(DIK_E))
        {
            m_subMenus[m_iCurrentButtonIdx]->OnClick();
        }
        if (m_pGameInstance->KeyDown(DIK_UPARROW))
        {
            --m_iCurrentButtonIdx;
            m_iCurrentButtonIdx = clamp<_uint>(m_iCurrentButtonIdx, 0, m_iTitleButtonIdx);
            m_subMenus[m_iCurrentButtonIdx]->OnHoverEnter();

        }

        else if (m_pGameInstance->KeyDown(DIK_DOWNARROW))
        {
            ++m_iCurrentButtonIdx;
            m_iCurrentButtonIdx = clamp<_uint>(m_iCurrentButtonIdx, 0, m_iTitleButtonIdx);
            m_subMenus[m_iCurrentButtonIdx]->OnHoverEnter();

        }
          
      


    }*/
    return 0;
}

HRESULT Client::UIObj_Window_Title::Render(const _float fTimeDelta)
{

    __super::Render(fTimeDelta);
    return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////
UIObj_Window_Title* Client::UIObj_Window_Title::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    UIObj_Window_Title* pInstance = new UIObj_Window_Title(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_Window_Title 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}

GameObject* Client::UIObj_Window_Title::Clone(void* pArg)
{
    UIObj_Window_Title* pInstance = new UIObj_Window_Title(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_Window_Title 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}
//////////////////////////////////////////////////////////////////////////////



void Client::UIObj_Window_Title::Free()
{
    __super::Free();
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
void        Client::UIObj_Window_Title::After_ApplyData()
{
    __super::After_ApplyData();

   m_pPressAnyKey = m_pGameInstance->Find_UI_ByName(L"PressAnyKey");
    if (m_pPressAnyKey)
        m_pPressAnyKey->Set_Active(true);

    m_pFocusMenu= dynamic_cast<UIObj_FocusMenu*>(m_pGameInstance->Find_UI_ByName(L"SelectMenu"));

    
    if (m_pFocusMenu)
    {
        m_ConfigButton = m_pFocusMenu->Get_Child(L"Config");
        if (m_ConfigButton)
            m_ConfigButton->Bind_OnClickEvent([this]()
                {
                    m_pGameInstance->Play_Sound("Title_Enter",UIVolume);

                    UIObj_FadeScreen::FadeScreenEvent Event;
                    Event.eType = UIObj_FadeScreen::FadeScreenEventType::ENTER_SCENE;
                    Event.m_iNextLevel = LEVEL::CUSTOMIZE;
                    m_pGameInstance->Publish(Event);
                });

    }
   
}
/******************************************************* 객체 준비 함수 *******************************************************/
