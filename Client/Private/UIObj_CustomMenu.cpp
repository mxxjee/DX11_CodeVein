#include "Client_Define.h"
#include "UIObj_CustomMenu.h"
#include "UIObject.h"

#include "UIObj_Selector.h"
#include "UIObj_CustomMenuButton.h"



Client::UIObj_CustomMenu::UIObj_CustomMenu()
{
}

Client::UIObj_CustomMenu::UIObj_CustomMenu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UIObject(pDevice,pContext)
{
}

Client::UIObj_CustomMenu::UIObj_CustomMenu(const UIObj_CustomMenu& original)
    :UIObject(original)
{
}

Client::UIObj_CustomMenu::~UIObj_CustomMenu()
{
}

//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_CustomMenu::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    return S_OK;
}

HRESULT Client::UIObj_CustomMenu::Initialize(void* arg)
{
    CHECK_FAILED(__super::Initialize(arg), E_FAIL);
    m_Buttons.reserve(CUSTOMIZING_TYPE::END);

    /*새로운 마녀 바꼇다고 알려질 시,  하이라이트 ui도같이 옮겨라(selector)*/
    m_vecSubscribeNumbers.push_back(m_pGameInstance->Subscribe< CustomMenuUIEvent>([this](const CustomMenuUIEvent& e)
    {

            UIObject* pObj = m_Buttons[_UINT(e.m_eCustomType)];
            if (pObj)
            {
                m_pSelector->MoveTo(pObj->Get_LocalTransform().m_fY);

                if (m_pSelector->Is_Active() == false)
                {
                    m_pSelector->Set_Visible(true);
                    m_pSelector->Set_Active(true);

                }
            }

    }));

    return S_OK;
}


/******************************************************* 객체 준비 함수 *******************************************************/


//////////////////////////////////////////////////////////////////////////////////////////////
UIObj_CustomMenu* Client::UIObj_CustomMenu::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    UIObj_CustomMenu* pInstance = new UIObj_CustomMenu(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_CustomMenu 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}

GameObject* Client::UIObj_CustomMenu::Clone(void* pArg)
{
    UIObj_CustomMenu* pInstance = new UIObj_CustomMenu(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_CustomMenu 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}


//////////////////////                                                                                                          /////////////////////////////////////////////////////////////////////////////////
void        Client::UIObj_CustomMenu::After_ApplyData()
{
  
    //자식캐싱
    //custommenubutton인 애들만 저장한다.
    //selector도 저장한다.

    for (auto& pObj : m_vecChildren)
    {
        UIObj_CustomMenuButton* pButton = dynamic_cast<UIObj_CustomMenuButton*>(pObj);
        if (pButton)
            m_Buttons.push_back(pButton);
    }
    m_pSelector = dynamic_cast<UIObj_Selector*>(Get_Child(L"Selector"));




}
/******************************************************* 객체 준비 함수 *******************************************************/

void Client::UIObj_CustomMenu::Free()
{
    __super::Free();
}
