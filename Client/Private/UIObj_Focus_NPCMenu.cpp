#include "Client_Define.h"
#include "UIObj_Focus_NPCMenu.h"
#include "UIObj_GlowButton.h"

Client::UIObj_Focus_NPCMenu::UIObj_Focus_NPCMenu()
{
}

Client::UIObj_Focus_NPCMenu::UIObj_Focus_NPCMenu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UIObj_FocusMenu(pDevice,pContext)
{
}

Client::UIObj_Focus_NPCMenu::UIObj_Focus_NPCMenu(const UIObj_Focus_NPCMenu& original)
    :UIObj_FocusMenu(original)
{
}

Client::UIObj_Focus_NPCMenu::~UIObj_Focus_NPCMenu()
{
}

//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_Focus_NPCMenu::Initialize_Prototype(_uint iLevel)
{
    CHECK_FAILED(__super::Initialize_Prototype(iLevel),E_FAIL);
    return S_OK;
}

HRESULT Client::UIObj_Focus_NPCMenu::Initialize(void* arg)
{
    CHECK_FAILED(UIObject::Initialize(arg), E_FAIL);



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


 
    /*npc 선택지 바꾸는 이벤트*/
    m_vecSubscribeNumbers.push_back(m_pGameInstance->Subscribe< FocusNPCMenuUIEvent>([this](const FocusNPCMenuUIEvent& e)
        {
            if (e.eType == FocusNPCMenuEventType::UPDATE_TEXT)
            {
                CHECK_TRUE(m_Buttons.size() <= e.m_iIdx);
                m_Buttons[e.m_iIdx]->Set_Text(e.Text);

            }

        }));
    return S_OK;
}


/******************************************************* 객체 준비 함수 *******************************************************/


_int Client::UIObj_Focus_NPCMenu::Update_Priority(const _float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    return 0;
}

_int Client::UIObj_Focus_NPCMenu::Update(const _float fTimeDelta)
{
    __super::Update(fTimeDelta);
    return 0;
}

_int Client::UIObj_Focus_NPCMenu::Update_Late(const _float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
    return 0;
}

HRESULT Client::UIObj_Focus_NPCMenu::Render(const _float fTimeDelta)
{
    __super::Render(fTimeDelta);
    return 0;
}

void Client::UIObj_Focus_NPCMenu::Free()
{
    __super::Free();
}

//////////////////////////////////////////////////////////////////////////////////////////////
UIObj_Focus_NPCMenu* Client::UIObj_Focus_NPCMenu::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    UIObj_Focus_NPCMenu* pInstance = new UIObj_Focus_NPCMenu(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_Focus_NPCMenu 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}


GameObject* Client::UIObj_Focus_NPCMenu::Clone(void* pArg)
{
    UIObj_Focus_NPCMenu* pInstance = new UIObj_Focus_NPCMenu(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_Focus_NPCMenu 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}
//////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
void        Client::UIObj_Focus_NPCMenu::After_ApplyData()
{
    __super::After_ApplyData();


}
/******************************************************* 객체 준비 함수 *******************************************************/

void Client::UIObj_Focus_NPCMenu::Set_Active(_bool _isActive)
{
  
    __super::Set_Active(_isActive);


}
