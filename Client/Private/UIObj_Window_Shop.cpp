#include "Client_Define.h"
#include "UIObj_Window_Shop.h"
#include "ShopManager.h"
#include "UISoundUtil.h"

Client::UIObj_Window_Shop::UIObj_Window_Shop()
{
}

Client::UIObj_Window_Shop::UIObj_Window_Shop(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UIObject(pDevice,pContext)
{
}

Client::UIObj_Window_Shop::UIObj_Window_Shop(const UIObj_Window_Shop& original)
    :UIObject(original)
{
}

Client::UIObj_Window_Shop::~UIObj_Window_Shop()
{
}
//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_Window_Shop::Initialize_Prototype(_uint iLevel)
{
	m_iLevel = iLevel;
	return S_OK;
}

HRESULT Client::UIObj_Window_Shop::Initialize(void* arg)
{
	CHECK_FAILED(__super::Initialize(arg), E_FAIL);
    
    m_pShopManager = ShopManager::GetInstance();

    m_vecSubscribeNumbers.push_back(m_pGameInstance->Subscribe< WINDOWSHOPEVENT>([this](const WINDOWSHOPEVENT& e)
        {
            if (e.eType == WINDOWSHOPEVNETTPYE::INITIALIZE)
                Change_Target(e.m_pTarget, e.TargetOffSet);

            else if (e.eType == WINDOWSHOPEVNETTPYE::END_INTERACTION)
                Clear_Target();

            else if (e.eType == WINDOWSHOPEVNETTPYE::UPDATE_HIGHLIGHT)
                Update_HighlightPos(e.m_eCategory);
        }));


	return S_OK;
}


/******************************************************* 객체 준비 함수 *******************************************************/



_int Client::UIObj_Window_Shop::Update_Priority(const _float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    return 0;
}

_int Client::UIObj_Window_Shop::Update(const _float fTimeDelta)
{
    __super::Update(fTimeDelta);
    return 0;
}

_int Client::UIObj_Window_Shop::Update_Late(const _float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
    return 0;
}

HRESULT Client::UIObj_Window_Shop::Render(const _float fTimeDelta)
{
    if (m_Local.m_fAlpha < 0.01f)
        return S_OK;

    __super::Render(fTimeDelta);
    return S_OK;
}

void Client::UIObj_Window_Shop::Free()
{
    __super::Free();
}

//////////////////////////////////////////////////////////////////////////////////////////////
UIObj_Window_Shop* Client::UIObj_Window_Shop::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    UIObj_Window_Shop* pInstance = new UIObj_Window_Shop(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_Window_Shop 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}

GameObject* Client::UIObj_Window_Shop::Clone(void* pArg)
{
    UIObj_Window_Shop* pInstance = new UIObj_Window_Shop(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_Window_Shop 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}
//////////////////////////////////////////////////////////////////////////////

//////////////////////                                                                                                          /////////////////////////////////////////////////////////////////////////////////
void        Client::UIObj_Window_Shop::After_ApplyData()
{
    m_pWorldUIComp = dynamic_cast<UI_WorldComponent*>(Get_Component_FromName(Proto_UIWorld));

	//자식캐싱
    m_CategoryHighlight = Get_Child(L"Category_Highlight");
    m_Category_Icon_Weapon = Get_Child(L"Category_Icon_Weapon");
    m_Category_Icon_Armor = Get_Child(L"Category_Icon_Armor");
    m_Category_Icon_Items = Get_Child(L"Category_Icon_Items");



    ///처음 설정
    //1.뭐팔건지 ui에띄우기
    m_pShopManager->Set_AllItemInfo_To_UI();
    //
    ////weapon활성화
    //m_pShopManager->Set_FocusCategory(ITEM_CATEGORY::EQUIP_WEAPON);


	
}
/******************************************************* 객체 준비 함수 *******************************************************/
void Client::UIObj_Window_Shop::Change_Target(GameObject* pTarget, _float3 Offset)
{
    CHECK_JUST_NULL(m_pWorldUIComp);
    m_pWorldUIComp->Set_Target(pTarget);
    m_pWorldUIComp->Set_OffSet(Offset);

}


void Client::UIObj_Window_Shop::Clear_Target()
{
    CHECK_JUST_NULL(m_pWorldUIComp);
    m_pWorldUIComp->Clear_Target();


}

void Client::UIObj_Window_Shop::Update_HighlightPos(ITEM_CATEGORY eCategory)
{
    CHECK_JUST_NULL(m_CategoryHighlight);
    CHECK_JUST_NULL(m_Category_Icon_Weapon);
    CHECK_JUST_NULL(m_Category_Icon_Armor);
    CHECK_JUST_NULL(m_Category_Icon_Items);

    PlayClickSound();

    switch (eCategory)
    {
    case Client::ITEM_CATEGORY::EQUIP_WEAPON:
        m_CategoryHighlight->Set_Position(m_Category_Icon_Weapon->Get_LocalTransform().m_fX, m_Category_Icon_Weapon->Get_LocalTransform().m_fY);
        break;

    case Client::ITEM_CATEGORY::EQUIP_ARMOR:
        m_CategoryHighlight->Set_Position(m_Category_Icon_Armor->Get_LocalTransform().m_fX, m_Category_Icon_Armor->Get_LocalTransform().m_fY);

        break;

    case Client::ITEM_CATEGORY::ITEM:
        m_CategoryHighlight->Set_Position(m_Category_Icon_Items->Get_LocalTransform().m_fX, m_Category_Icon_Items->Get_LocalTransform().m_fY);
        break;

    default:
        break;
    }
}
