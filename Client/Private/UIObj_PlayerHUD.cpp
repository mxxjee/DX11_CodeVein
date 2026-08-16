#include "Client_Define.h"
#include "UIObj_PlayerHUD.h"
#include "UIObj_ProgressBar.h"
#include "UIObj_SlotGrid.h"
#include "UIObj_ShortCutMenu.h"
#include "GameInstance.h"




/////////////////////////////////////////////
Client::UIObj_PlayerHUD::UIObj_PlayerHUD()
{
}

Client::UIObj_PlayerHUD::UIObj_PlayerHUD(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UIObject(pDevice,pContext)
{
}

Client::UIObj_PlayerHUD::UIObj_PlayerHUD(const UIObj_PlayerHUD& original)
    :UIObject(original)
{
}

Client::UIObj_PlayerHUD::~UIObj_PlayerHUD()
{
}
///////////////////////////////////////////////////////

//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_PlayerHUD::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    return S_OK;
}

HRESULT Client::UIObj_PlayerHUD::Initialize(void* arg)
{
    CHECK_FAILED(__super::Initialize(arg), E_FAIL);


    return S_OK;
}


/******************************************************* 객체 준비 함수 *******************************************************/



_int Client::UIObj_PlayerHUD::Update_Priority(const _float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    return 0;
}

_int Client::UIObj_PlayerHUD::Update(const _float fTimeDelta)
{
    __super::Update(fTimeDelta);
    return 0;
}

_int Client::UIObj_PlayerHUD::Update_Late(const _float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
    return 0;
}

HRESULT Client::UIObj_PlayerHUD::Render(const _float fTimeDelta)
{
    __super::Render(fTimeDelta);
    return 0;
}


UIObj_PlayerHUD* Client::UIObj_PlayerHUD::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    UIObj_PlayerHUD* pInstance = new UIObj_PlayerHUD(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_PlayerHUD 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}

GameObject* Client::UIObj_PlayerHUD::Clone(void* pArg)
{
    UIObj_PlayerHUD* pInstance = new UIObj_PlayerHUD(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_PlayerHUD 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}

void Client::UIObj_PlayerHUD::Free()
{
    m_ProgressBars.clear();
    __super::Free();
}





HRESULT Client::UIObj_PlayerHUD::Add_Child_OnLoad(GameObject* pObj)
{
    __super::Add_Child_OnLoad(pObj);


    return S_OK;
}



HRESULT Client::UIObj_PlayerHUD::Add_Child_OnLoad(UIObject* pObj)
{
    __super::Add_Child_OnLoad(pObj);

  
    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
void        Client::UIObj_PlayerHUD::After_ApplyData()
{
    //자식캐싱
    UIObject* pMainHPObj = Get_Child(L"HPBar");
    if (pMainHPObj)
        m_ProgressBars.push_back(dynamic_cast<UIObj_ProgressBar*>(pMainHPObj));

    UIObject* pGuardBarObj = Get_Child(L"GuardBar");
    if (pGuardBarObj)
        m_pGuardBar = pGuardBarObj;


    m_QuickSlotGrid.reserve(2);

    //연혈스킬 1, 2 퀵슬롯설정
    UIObject* pQuickSlotGrid = Get_Child(L"Player_QuickSlot1");
    if (pQuickSlotGrid)
        m_QuickSlotGrid.push_back(dynamic_cast<UIObj_SlotGrid*>(pQuickSlotGrid));

    pQuickSlotGrid = Get_Child(L"Player_QuickSlot2");
    if (pQuickSlotGrid)
        m_QuickSlotGrid.push_back(dynamic_cast<UIObj_SlotGrid*>(pQuickSlotGrid));

 
}
/******************************************************* 객체 준비 함수 *******************************************************/

void        Client::UIObj_PlayerHUD::Set_Active(_bool _isActive)
{
    __super::Set_Active(_isActive);



}