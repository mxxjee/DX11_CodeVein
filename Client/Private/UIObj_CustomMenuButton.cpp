#include "Client_Define.h"
#include "UIObj_CustomMenuButton.h"
#include "CustomizingManager.h"
#include "UIObj_Text.h"
#include "UIObj_FadeScreen.h"

#include "UISoundUtil.h"


Client::UIObj_CustomMenuButton::UIObj_CustomMenuButton()
{
}

Client::UIObj_CustomMenuButton::UIObj_CustomMenuButton(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UIObject(pDevice, pContext)
{
}

Client::UIObj_CustomMenuButton::UIObj_CustomMenuButton(const UIObj_CustomMenuButton& original)
    :UIObject(original)
{
}

Client::UIObj_CustomMenuButton::~UIObj_CustomMenuButton()
{
}

//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_CustomMenuButton::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    return S_OK;
}

HRESULT Client::UIObj_CustomMenuButton::Initialize(void* arg)
{
    CHECK_FAILED(__super::Initialize(arg), E_FAIL);
    m_pCustomizingManager = CustomizingManager::GetInstance();

    m_vecSubscribeNumbers.push_back(m_pGameInstance->Subscribe< CustomMenuButtonUIEvent>([this](const CustomMenuButtonUIEvent& e)
        {
            switch (e.eType)
            {
            case CustomMenuButtonUIType::ENTER_MENU:
            {
                //오브젝트 텍스트 색깔어둡게 처리..
                if (m_pText)
                    m_pText->Set_Color(_float4(0.5f, 0.5f, 0.5f, 1.f));
            }
                break;

            case CustomMenuButtonUIType::EXIT_MENU:
            {
                //오브젝트 텍스트 색깔 밝게
                if (m_pText)
                    m_pText->Set_Color(_float4(1.f, 1.f, 1.f, 1.f));
            }
            break;

            default:
                break;
            }

        }));

    return S_OK;
}


/******************************************************* 객체 준비 함수 *******************************************************/

//////////////////////////////////////////////////////////////////////////////////////////////
UIObj_CustomMenuButton* Client::UIObj_CustomMenuButton::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    UIObj_CustomMenuButton* pInstance = new UIObj_CustomMenuButton(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_CustomMenuButton 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}

GameObject* Client::UIObj_CustomMenuButton::Clone(void* pArg)
{
    UIObj_CustomMenuButton* pInstance = new UIObj_CustomMenuButton(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_CustomMenuButton 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}


//////////////////////               

//////////////////////                                                                                                          /////////////////////////////////////////////////////////////////////////////////
void        Client::UIObj_CustomMenuButton::After_ApplyData()
{
   //1.타입에 따라서  customtype지정
    m_eCustomType = Get_CustomType(m_UIType);


    ////2.Desc작성

    //m_eButtonDesc.OpenMenuName = stringToWstring(m_UIType) + L"_Window";


    m_pText = dynamic_cast<UIObj_Text*>(Get_Child(L"Text"));

}

void Client::UIObj_CustomMenuButton::OnHoverEnter()
{
    //만약 ui창이 이미 focus상태라면, 다른메뉴 호버불가한다.
    CustomizingManager::CustomMizingstate eCurrentState = m_pCustomizingManager->Get_UIState();

    COUT("HoverTarget:" << Get_CustomType_Str(m_eCustomType) << "\n");

    CHECK_TRUE(_UINT(eCurrentState) > _UINT(CustomizingManager::CustomMizingstate::READY));
    __super::OnHoverEnter();
    

 

    //cusominzing manager한테 focus된 카테고리 알리기
    m_pCustomizingManager->Set_FocusType(m_eCustomType);
    PlayHoverSound();
    
}
void        Client::UIObj_CustomMenuButton::OnClick()
{

    //만약 ui창이 이미 focus상태라면, 다른메뉴 호버불가한다.
    CustomizingManager::CustomMizingstate eCurrentState = m_pCustomizingManager->Get_UIState();
    CHECK_TRUE(_UINT(eCurrentState) > _UINT(CustomizingManager::CustomMizingstate::READY));


    __super::OnClick();
 

    m_pCustomizingManager->On_FocusMenuEnter();
    PlayClickSound();
    
}


/******************************************************* 객체 준비 함수 *******************************************************/

void Client::UIObj_CustomMenuButton::Free()
{
    __super::Free();
}



