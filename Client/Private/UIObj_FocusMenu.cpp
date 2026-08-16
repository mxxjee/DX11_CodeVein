#include "Client_Define.h"
#include "UIObj_FocusMenu.h"
#include "UIObj_Selector.h"
#include "UIObj_GlowButton.h"
#include "UIObj_SceneSelectMenu.h"
#include "UIObj_SceneSlot.h"

#include "InteractionManager.h"
#include "IInteractable.h"

#include "UISoundUtil.h"


Client::UIObj_FocusMenu::UIObj_FocusMenu()
{
}

Client::UIObj_FocusMenu::UIObj_FocusMenu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UIObject(pDevice, pContext)
{
}

Client::UIObj_FocusMenu::UIObj_FocusMenu(const UIObj_FocusMenu& original)
    :UIObject(original)
{
}

Client::UIObj_FocusMenu::~UIObj_FocusMenu()
{
}


//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_FocusMenu::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    return S_OK;
}

HRESULT Client::UIObj_FocusMenu::Initialize(void* arg)
{
    CHECK_FAILED(__super::Initialize(arg), E_FAIL);



    m_vecSubscribeNumbers.push_back(m_pGameInstance->Subscribe< FocusMenuUpdateEvent>([this](const FocusMenuUpdateEvent& Event)
        {
            if (Event.m_iObjID != m_iObjectID)
                return;

            if (!m_bIsActive)
                return;

            //이전꺼 exit처리
            m_Buttons[m_iCurrentIdx]->OnHoverExit();

            m_iCurrentIdx = Event.iCurrentIdx;
            Change_Idx(m_iCurrentIdx);
        }));


    m_vecSubscribeNumbers.push_back(m_pGameInstance->Subscribe< FocusMenuToParentEvent>([this](const FocusMenuToParentEvent& Event)
        {
            if (Event.m_iObjID != m_iObjectID)
                return;

            if (!m_bIsActive)
                return;

            UIObj_SceneSlot::SceneSlotUpdateColorEvent ColorEvent;
            ColorEvent.m_iObjectID = m_pParent->Get_ObjectID();//sceneslot id
            ColorEvent.TexKey = Event.m_Text;
             m_pGameInstance->Publish(ColorEvent);

            
        }));
    return S_OK;
}


/******************************************************* 객체 준비 함수 *******************************************************/


_int Client::UIObj_FocusMenu::Update_Priority(const _float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    return 0;
}

_int Client::UIObj_FocusMenu::Update(const _float fTimeDelta)
{
    To_Update_HoverLine.Update(fTimeDelta);
    __super::Update(fTimeDelta);
    return 0;
}

_int Client::UIObj_FocusMenu::Update_Late(const _float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
 
    CHECK_FALSE_RESULT(m_bIsActive,0);
    if (m_bInput)
    {

        if (m_pGameInstance->KeyDown(DIK_E))
        {
           
            //선택
            m_Buttons[m_iCurrentIdx]->OnClick();

        }


        if (m_pGameInstance->KeyDown(DIK_UPARROW))
        {
            //이전꺼 exit처리

            m_Buttons[m_iCurrentIdx]->OnHoverExit();
            UIObject* pHover = m_pGameInstance->Get_HoveredUI();
            if (pHover)
                pHover->OnHoverExit();
            


            --m_iCurrentIdx;
            m_iCurrentIdx = clamp<int>(m_iCurrentIdx, 0, m_iMaxIdx);
            
            Change_Idx(m_iCurrentIdx);
     


        }

        else if (m_pGameInstance->KeyDown(DIK_DOWNARROW))
        {

            //이전꺼 exit처리
            UIObject* pHover = m_pGameInstance->Get_HoveredUI();
            if (pHover)
                pHover->OnHoverExit();

            m_Buttons[m_iCurrentIdx]->OnHoverExit();


            ++m_iCurrentIdx;
            m_iCurrentIdx = clamp<int>(m_iCurrentIdx, 0, m_iMaxIdx);
            Change_Idx(m_iCurrentIdx);



        }

    }

    return 0;
}

void Client::UIObj_FocusMenu::Change_Idx(_uint iNewIdx)
{

    UITransform MenuLocal = m_Buttons[iNewIdx]->Get_LocalTransform();
    if (m_pSelector)
        m_pSelector->MoveTo( MenuLocal.m_fY);

    m_Buttons[iNewIdx]->OnHoverEnter_With_Keyboard();
    PlayHoverSound();
}

HRESULT Client::UIObj_FocusMenu::Render(const _float fTimeDelta)
{
    __super::Render(fTimeDelta);
    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////
UIObj_FocusMenu* Client::UIObj_FocusMenu::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    UIObj_FocusMenu* pInstance = new UIObj_FocusMenu(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_FocusMenu 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}

GameObject* Client::UIObj_FocusMenu::Clone(void* pArg)
{
    UIObj_FocusMenu* pInstance = new UIObj_FocusMenu(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_FocusMenu 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}

//////////////////////////////////////////////////////////////////////////////

void Client::UIObj_FocusMenu::Free()
{
    __super::Free();
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
void        Client::UIObj_FocusMenu::After_ApplyData()
{
    __super::After_ApplyData();

    m_iCurrentIdx = 0;

    _uint i = 0;
    for (auto& pChild : m_vecChildren)
    {
        UIObj_GlowButton* pButton = dynamic_cast<UIObj_GlowButton*>(pChild);
        if (pButton)
        {
            pButton->Set_Idx(i);
            ++i;
            m_Buttons.push_back(pButton);
        }
     
    }
    m_iMaxIdx = m_Buttons.size() - 1;
    m_pSelector = dynamic_cast<UIObj_Selector*>(Get_Child(L"Selector"));


    //savepoint메뉴 끌때 exit처리해야하므로.. 
    if (m_UIType == "SAVEPOINT")
        m_eType = FOCUSMENUTYPE::SAVEPOINT;


    else
        m_eType = FOCUSMENUTYPE::OTHER;

}
/******************************************************* 객체 준비 함수 *******************************************************/
void Client::UIObj_FocusMenu::Set_Active(_bool _isActive)
{
    if (_isActive)
    {
        To_Update_HoverLine.Elapsed = 0;
        To_Update_HoverLine.Limit = 0.01f;
        To_Update_HoverLine.m_AlarmFunc = [this]()
            {
                m_bInput = true;

                 Change_Idx(m_iCurrentIdx);

                To_Update_HoverLine.Off();
            };

        To_Update_HoverLine.On();
    }

    else
    {
   
        To_Update_HoverLine.Off();
    }

    __super::Set_Active(_isActive);

   
}