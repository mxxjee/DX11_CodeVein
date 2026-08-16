#include "Client_Define.h"
#include "UIObj_ShopSlotGrid.h"
#include "ShopManager.h"
#include "UIObj_ShopSlot.h"

Client::UIObj_ShopSlotGrid::UIObj_ShopSlotGrid()
{
}

Client::UIObj_ShopSlotGrid::UIObj_ShopSlotGrid(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UIObject(pDevice,pContext)
{
}

Client::UIObj_ShopSlotGrid::UIObj_ShopSlotGrid(const UIObj_ShopSlotGrid& original)
    :UIObject(original)
{
}

Client::UIObj_ShopSlotGrid::~UIObj_ShopSlotGrid()
{
}
//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_ShopSlotGrid::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    return S_OK;
}

HRESULT Client::UIObj_ShopSlotGrid::Initialize(void* arg)
{
    CHECK_FAILED(__super::Initialize(arg), E_FAIL);
    m_pShopManager = ShopManager::GetInstance();

    m_vecSubscribeNumbers.push_back(m_pGameInstance->Subscribe< ShopSlotGridUIEvent>([this](const ShopSlotGridUIEvent& e)
        {
            //만약, 클릭한 카테고리가바뀐상황이라면 
            if (e.eventType == ShopSlotGridEventType::UPDATE_FOCUSCATEGORY)
            {
                //판단하여 자기를 킬지안킬지 정함.
                if (m_pShopManager->Get_FocusCategory() == m_eItemCategory)
                {
                    Set_Visible(true);
                    Set_Active(true);
                }

                else
                {
                    Set_Visible(false);
                    Set_Active(false);
                }
            }


            else if (e.eventType == ShopSlotGridEventType::INITIALIZE_SLOTS)
            {
                if (e.m_eCategory != m_eItemCategory)
                    return;

                Set_Slots(e.m_Items);
            }
        }));



    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////


_int Client::UIObj_ShopSlotGrid::Update_Priority(const _float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    return 0;
}

_int Client::UIObj_ShopSlotGrid::Update(const _float fTimeDelta)
{
    __super::Update(fTimeDelta);
    return 0;
}

_int Client::UIObj_ShopSlotGrid::Update_Late(const _float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
    return 0;

}

HRESULT Client::UIObj_ShopSlotGrid::Render(const _float fTimeDelta)
{
    __super::Render(fTimeDelta);
    return S_OK;

}

void Client::UIObj_ShopSlotGrid::Free()
{
    __super::Free();
}




UIObj_ShopSlotGrid* Client::UIObj_ShopSlotGrid::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    UIObj_ShopSlotGrid* pInstance = new UIObj_ShopSlotGrid(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_ShopSlotGrid 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}


GameObject* Client::UIObj_ShopSlotGrid::Clone(void* pArg)
{
    UIObj_ShopSlotGrid* pInstance = new UIObj_ShopSlotGrid(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_ShopSlotGrid 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
void        Client::UIObj_ShopSlotGrid::After_ApplyData()
{
   
    //그리드 타입지정하기
    if (m_UIType == "EQUIP_ARMOR")
        m_eItemCategory = ITEM_CATEGORY::EQUIP_ARMOR;

    else if (m_UIType == "EQUIP_WEAPON")
        m_eItemCategory = ITEM_CATEGORY::EQUIP_WEAPON;

    else if (m_UIType == "SHORTCUT")
        m_eItemCategory = ITEM_CATEGORY::ITEM;




    //slots캐싱
    for (auto& pChild : m_vecChildren)
    {
        UIObj_ShopSlot* pSlot = dynamic_cast<UIObj_ShopSlot*>(pChild);
        if (pSlot)
            m_ShopSlots.push_back(pSlot);

    }

    __super::After_ApplyData();
   
}
/******************************************************* 객체 준비 함수 *******************************************************/

/******************************************************* 이벤트로 인해 실행되는함수 *******************************************************/
void Client::UIObj_ShopSlotGrid::Set_Slots(vector<ItemInfo*> Items)
{
    for (size_t i = 0; i < m_ShopSlots.size(); ++i)
    {
        if(Items[i]!=nullptr)
            m_ShopSlots[i]->Set_ItemInfo(Items[i]);
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
