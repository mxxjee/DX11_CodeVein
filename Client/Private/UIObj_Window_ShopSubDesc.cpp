#include "Client_Define.h"
#include "UIObj_Window_ShopSubDesc.h"
#include "InventoryManager.h"

#include "UIObj_Text.h"

Client::UIObj_Window_ShopSubDesc::UIObj_Window_ShopSubDesc()
{
}

Client::UIObj_Window_ShopSubDesc::UIObj_Window_ShopSubDesc(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UIObject(pDevice, pContext)
{
}

Client::UIObj_Window_ShopSubDesc::UIObj_Window_ShopSubDesc(const UIObj_Window_ShopSubDesc& original)
    :UIObject(original)
{
}

Client::UIObj_Window_ShopSubDesc::~UIObj_Window_ShopSubDesc()
{
}
//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_Window_ShopSubDesc::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    return S_OK;
}

HRESULT Client::UIObj_Window_ShopSubDesc::Initialize(void* arg)
{
    CHECK_FAILED(__super::Initialize(arg), E_FAIL);

    m_vecSubscribeNumbers.push_back(m_pGameInstance->Subscribe< ShopSubDescUIEvent>([this](const ShopSubDescUIEvent& e)
        {
            if (e.eCategory != m_eCategory)
            {
                Set_Visible(false);
                Set_Active(false);
                return;
            }

            else
            {
                if (!m_bIsActive || !m_bIsVisible)
                {
                    Set_Visible(true);
                    Set_Active(true);
                }
             
                Update_SubDesc(e.pInfo);
            }




        }));


    return S_OK;
}


/******************************************************* 객체 준비 함수 *******************************************************/

_int Client::UIObj_Window_ShopSubDesc::Update_Priority(const _float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    return 0;
}

_int Client::UIObj_Window_ShopSubDesc::Update(const _float fTimeDelta)
{
    __super::Update(fTimeDelta);
    return 0;
}

_int Client::UIObj_Window_ShopSubDesc::Update_Late(const _float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
    return 0;
}

HRESULT Client::UIObj_Window_ShopSubDesc::Render(const _float fTimeDelta)
{
    __super::Render(fTimeDelta);
    return S_OK;
}

void Client::UIObj_Window_ShopSubDesc::Free()
{
    __super::Free();

}
//////////////////////////////////////////////////////////////////////////////////////////////
UIObj_Window_ShopSubDesc* Client::UIObj_Window_ShopSubDesc::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    UIObj_Window_ShopSubDesc* pInstance = new UIObj_Window_ShopSubDesc(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_Window_ShopSubDesc 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}

GameObject* Client::UIObj_Window_ShopSubDesc::Clone(void* pArg)
{
    UIObj_Window_ShopSubDesc* pInstance = new UIObj_Window_ShopSubDesc(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_Window_ShopSubDesc 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}
//////////////////////////////////////////////////////////////////////////////

void Client::UIObj_Window_ShopSubDesc::Update_SubDesc(ItemInfo* pInfo)
{
    if (pInfo->m_eCategory == ITEM_CATEGORY::ITEM)
    {
        CHECK_JUST_NULL(m_pCurrentCount);
        CHECK_JUST_NULL(m_pMaxCount);


        //현재값 세팅하기
        ItemInfo* InvenItem=InventoryManager::GetInstance()->Get_Item(pInfo->ItemID);
        if (InvenItem == nullptr)
            m_pCurrentCount->Set_Text(to_wstring(0));

        else
            m_pCurrentCount->Set_Text(to_wstring(InvenItem->itemCount));

        m_pMaxCount->Set_Text(to_wstring(pInfo->maxItemSize));
    }

    else
    {
        CHECK_JUST_NULL(AttackType);
        CHECK_JUST_NULL(PhysicsType);


        AttackType->Set_Text(pInfo->m_AttackType);
        PhysicsType->Set_Text(to_wstring((int)pInfo->Get_Value(ITEM_VALUE::PHYSICS_DMG)));
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
void        Client::UIObj_Window_ShopSubDesc::After_ApplyData()
{
    __super::After_ApplyData();

    if (m_UIType == "EQUIP_ARMOR")
        m_eCategory = ITEM_CATEGORY::EQUIP_ARMOR;

    else if (m_UIType == "EQUIP_WEAPON")
        m_eCategory = ITEM_CATEGORY::EQUIP_WEAPON;

    else if (m_UIType == "SHORTCUT")
        m_eCategory = ITEM_CATEGORY::ITEM;

    m_pMaxCount = dynamic_cast<UIObj_Text*>(Get_Child(L"Max_Count"));
    m_pCurrentCount = dynamic_cast<UIObj_Text*>(Get_Child(L"Current_Count"));


    AttackType = dynamic_cast<UIObj_Text*>(Get_Child(L"Text_AttackType_Input"));
    PhysicsType= dynamic_cast<UIObj_Text*>(Get_Child(L"Text_Physics_Input"));


}

