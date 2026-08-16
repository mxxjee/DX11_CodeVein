#include "Client_Define.h"
#include "UIObj_Npc_ChoiceMenu.h"

Client::UIObj_Npc_ChoiceMenu::UIObj_Npc_ChoiceMenu()
{
}

Client::UIObj_Npc_ChoiceMenu::UIObj_Npc_ChoiceMenu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UIObject(pDevice, pContext)
{
}

Client::UIObj_Npc_ChoiceMenu::UIObj_Npc_ChoiceMenu(const UIObj_Npc_ChoiceMenu& original)
    :UIObject(original)
{
}

Client::UIObj_Npc_ChoiceMenu::~UIObj_Npc_ChoiceMenu()
{
}
//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_Npc_ChoiceMenu::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    return S_OK;
}

HRESULT Client::UIObj_Npc_ChoiceMenu::Initialize(void* arg)
{
    CHECK_FAILED(__super::Initialize(arg), E_FAIL);
    m_vecSubscribeNumbers.push_back(m_pGameInstance->Subscribe<NPCMENUEVENT>([this](const NPCMENUEVENT& e)
        {
 
            if (e.eType == NPCMENUEVENTTYPE::INITIALIZE)
                Change_Target(e.m_pTarget, e.TargetOffSet);

            else if (e.eType == NPCMENUEVENTTYPE::END_INTERACTION)
                Clear_Target();

        }));

    return S_OK;
}


/******************************************************* 객체 준비 함수 *******************************************************/

_int Client::UIObj_Npc_ChoiceMenu::Update_Priority(const _float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    return 0;
}

_int Client::UIObj_Npc_ChoiceMenu::Update(const _float fTimeDelta)
{
    __super::Update(fTimeDelta);
    return 0;
}

_int Client::UIObj_Npc_ChoiceMenu::Update_Late(const _float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
    return 0;
}

HRESULT Client::UIObj_Npc_ChoiceMenu::Render(const _float fTimeDelta)
{
    __super::Render(fTimeDelta);
    return 0;
}



void Client::UIObj_Npc_ChoiceMenu::Change_Target(GameObject* pTarget, _float3 Offset)
{
    CHECK_JUST_NULL(m_pWorldUIComp);
    m_pWorldUIComp->Set_Target(pTarget);
    m_pWorldUIComp->Set_OffSet(Offset);

}


void Client::UIObj_Npc_ChoiceMenu::Clear_Target()
{
    CHECK_JUST_NULL(m_pWorldUIComp);
    m_pWorldUIComp->Clear_Target();


}

UIObj_Npc_ChoiceMenu* Client::UIObj_Npc_ChoiceMenu::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    UIObj_Npc_ChoiceMenu* pInstance = new UIObj_Npc_ChoiceMenu(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_Npc_ChoiceMenu 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}

GameObject* Client::UIObj_Npc_ChoiceMenu::Clone(void* pArg)
{
    UIObj_Npc_ChoiceMenu* pInstance = new UIObj_Npc_ChoiceMenu(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_Npc_ChoiceMenu 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}

void Client::UIObj_Npc_ChoiceMenu::Free()
{
    __super::Free();
}



///////////////////////////////////////////////////////////////////////////////////////////////////////
void        Client::UIObj_Npc_ChoiceMenu::After_ApplyData()
{
    __super::After_ApplyData();


    //자식캐싱
    m_pWorldUIComp =dynamic_cast<UI_WorldComponent*>( Get_Component_FromName(Proto_UIWorld));



}
void        Client::UIObj_Npc_ChoiceMenu::Set_Active(_bool _isActive)
{
    __super::Set_Active(_isActive);

}