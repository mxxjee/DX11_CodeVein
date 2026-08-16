#include "Client_Define.h"
#include "UIObj_ShopCategory.h"
#include "ShopManager.h"

Client::UIObj_ShopCategory::UIObj_ShopCategory()
{
}

Client::UIObj_ShopCategory::UIObj_ShopCategory(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UIObject(pDevice,pContext)
{
}

Client::UIObj_ShopCategory::UIObj_ShopCategory(const UIObj_ShopCategory& original)
    :UIObject(original)
{
}

Client::UIObj_ShopCategory::~UIObj_ShopCategory()
{
}
//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_ShopCategory::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    return S_OK;
}

HRESULT Client::UIObj_ShopCategory::Initialize(void* arg)
{
    CHECK_FAILED(__super::Initialize(arg), E_FAIL);

    m_pShoptManager = ShopManager::GetInstance();

    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////


_int Client::UIObj_ShopCategory::Update_Priority(const _float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    return 0;
}

_int Client::UIObj_ShopCategory::Update(const _float fTimeDelta)
{
    __super::Update(fTimeDelta);
    return 0;
}

_int Client::UIObj_ShopCategory::Update_Late(const _float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
    return 0;
}

HRESULT Client::UIObj_ShopCategory::Render(const _float fTimeDelta)
{
    __super::Render(fTimeDelta);
    return S_OK;
}

void Client::UIObj_ShopCategory::Free()
{
    __super::Free();
}

UIObj_ShopCategory* Client::UIObj_ShopCategory::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    UIObj_ShopCategory* pInstance = new UIObj_ShopCategory(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_ShopCategory 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}

GameObject* Client::UIObj_ShopCategory::Clone(void* pArg)
{
    UIObj_ShopCategory* pInstance = new UIObj_ShopCategory(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_ShopCategory 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
void        Client::UIObj_ShopCategory::After_ApplyData()
{
    __super::After_ApplyData();
    //그리드 타입지정하기

    if (m_UIType == "EQUIP_ARMOR")
        m_eItemCategory = ITEM_CATEGORY::EQUIP_ARMOR;

    else if (m_UIType == "EQUIP_WEAPON")
        m_eItemCategory = ITEM_CATEGORY::EQUIP_WEAPON;

    else if (m_UIType == "SHORTCUT")
        m_eItemCategory = ITEM_CATEGORY::ITEM;
}

void        Client::UIObj_ShopCategory::Execute_By_Event(const string& strActionName, void* pArg)
{
    if (strActionName == "OnCategoryClick")
    {
        //shopmanager에게 갱신시켜주기
        m_pShoptManager->Set_FocusCategory(m_eItemCategory);

    }
}
/******************************************************* 객체 준비 함수 *******************************************************/
