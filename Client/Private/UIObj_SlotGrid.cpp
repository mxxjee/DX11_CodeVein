#include "Client_Define.h"
#include "UIObj_SlotGrid.h"
#include "UIObj_Slot.h"
#include "InventoryManager.h"
Client::UIObj_SlotGrid::UIObj_SlotGrid()
{
}

Client::UIObj_SlotGrid::UIObj_SlotGrid(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UIObject(pDevice,pContext)
{
}

Client::UIObj_SlotGrid::UIObj_SlotGrid(const UIObj_SlotGrid& original)
    :UIObject(original)
{
}

Client::UIObj_SlotGrid::~UIObj_SlotGrid()
{
}

//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_SlotGrid::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    return S_OK;
}

HRESULT Client::UIObj_SlotGrid::Initialize(void* arg)
{
    CHECK_FAILED(__super::Initialize(arg), E_FAIL);

    m_pInventoryManager = InventoryManager::GetInstance();
    m_pGameInstance->Subscribe<UIObj_SlotGrid::SLOT_GRID_EVENT>([this](const UIObj_SlotGrid::SLOT_GRID_EVENT& Event)
        {
            ItemInfo* pInfo = static_cast<ItemInfo*>(Event.pArg);
            if (Event.eGridType != m_SlotGridType)
                return;

            //if (pInfo)
            //{
            //    //현재 내 그리드와 호환가능한 아이템인지확인
            //    if (!Is_Compatible(pInfo->m_eCategory))
            //        return;
            //}
           

            //action해시비교
            for (auto& pair : m_Actions)
            {
                if (pair.second)
                {
                    if (pair.first == Event.ActionHash)
                        pair.second(&Event);
                }
            }



        });



    //단순히 새로운 아이템을 새로운 슬롯에 추가할때
  /*  m_Actions.emplace(hash<string>{}("Add_Item"), [this](const void* pArg)
        {
            const SLOT_GRID_EVENT* GridEvent = static_cast<const SLOT_GRID_EVENT*>(pArg);
            ItemInfo* pInfo = static_cast<ItemInfo*>(GridEvent->pArg);

            if (GridEvent)
            {
                for (int i = 0; i < m_Slots.size(); ++i)
                {
                    if (m_Slots[i]->Can_Equip())
                    {
                        m_Slots[i]->Equip(pInfo);
                        return;
                    }


                }
            }

        });*/

    m_Actions.emplace(hash<string>{}("Refresh_Item"), [this](const void* pArg)
        {
            const SLOT_GRID_EVENT* GridEvent = static_cast<const SLOT_GRID_EVENT*>(pArg);
            ItemInfo* pInfo = static_cast<ItemInfo*>(GridEvent->pArg);

            if (GridEvent)
            {
                for (int i = 0; i < m_Slots.size(); ++i)
                {
                    if (!m_Slots[i]->Can_Equip() && m_Slots[i]->Get_ItemInfo()->ItemID == pInfo->ItemID)
                    {
                        m_Slots[i]->Refresh_Count(pInfo->itemCount);
                        return;

                    }
                }
            }

        });

    m_Actions.emplace(hash<string>{}("Update_All_TotalSlots"), [this](const void* pArg)
        {
            const UIObj_SlotGrid::SLOT_GRID_EVENT* pDesc=static_cast<const UIObj_SlotGrid::SLOT_GRID_EVENT*>(pArg);
            Update_All_TotalSlots(pDesc->pArg);

        });


    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////



_int Client::UIObj_SlotGrid::Update_Priority(const _float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    return 0;
}

_int Client::UIObj_SlotGrid::Update(const _float fTimeDelta)
{
    __super::Update(fTimeDelta);
    return 0;
}

_int Client::UIObj_SlotGrid::Update_Late(const _float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
    return 0;
}

HRESULT Client::UIObj_SlotGrid::Render(const _float fTimeDelta)
{
    __super::Render(fTimeDelta);
    return 0;
}
    
void Client::UIObj_SlotGrid::Update_All_TotalSlots(void* pArg)
{

    InventoryManager::SelectSlotInfo* pTargetInfo = static_cast<InventoryManager::SelectSlotInfo*>(pArg);
    CHECK_JUST_NULL(pArg);

  //일단 모든 슬롯 리셋.이후 다시세팅
	for (auto& pSlot : m_Slots)
	{
		pSlot->Reset_Slot();
	}

    ITEM_CATEGORY eCategory=ITEM_CATEGORY::END;
    switch (pTargetInfo->eType)
    {
    case Client::SLOT_TYPE::EQUIP_WEAPON:
        eCategory = ITEM_CATEGORY::EQUIP_WEAPON;
        break;
    case Client::SLOT_TYPE::EQUIP_ARMOR:
        eCategory = ITEM_CATEGORY::EQUIP_ARMOR;
        break;
    case Client::SLOT_TYPE::SKILL:
        eCategory = ITEM_CATEGORY::SKILL;
        break;
    case Client::SLOT_TYPE::PASSIVE:
        eCategory = ITEM_CATEGORY::PASSIVE;
        break;
    case Client::SLOT_TYPE::SHORTCUT:
        eCategory = ITEM_CATEGORY::ITEM;
        break;
    case Client::SLOT_TYPE::BLOODCODE:
        eCategory = ITEM_CATEGORY::BLOODCODE;
        break;
    default:
        break;
    }

    //자식들에게 각자 세팅하라고 전파
    vector<ItemInfo*>         CategoryItems = m_pInventoryManager->Get_CategoryList(eCategory);

	for (int i = 0; i < m_Slots.size(); ++i)
	{
        if (!CategoryItems.empty() && i < CategoryItems.size())
        {
            m_Slots[i]->Equip(CategoryItems[i]);
            m_Slots[i]->Setting_TotalSlot(CategoryItems[i]);
        }
    
        else
        {
            m_Slots[i]->UnEquip();
            m_Slots[i]->Setting_TotalSlot(nullptr);
        }
		m_Slots[i]->Set_VisibleColor();

	}
    
   
}

void Client::UIObj_SlotGrid::Set_Hover_AllSlots(bool bUseHover)
{
    for (auto& pSlot : m_Slots)
    {
        if (pSlot)
            pSlot->Set_UseHoverOutline(bUseHover);
    }
}

void Client::UIObj_SlotGrid::Free()
{
    __super::Free();
}

UIObj_SlotGrid* Client::UIObj_SlotGrid::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    UIObj_SlotGrid* pInstance = new UIObj_SlotGrid(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_SlotGrid 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}



GameObject* Client::UIObj_SlotGrid::Clone(void* pArg)
{
    UIObj_SlotGrid* pInstance = new UIObj_SlotGrid(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_SlotGrid 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}

_bool Client::UIObj_SlotGrid::Is_Compatible(ITEM_CATEGORY eCategory)
{
    switch (m_SlotType)
    {
    case Client::SLOT_TYPE::EQUIP_WEAPON:
        return eCategory==ITEM_CATEGORY::EQUIP_WEAPON;

    case Client::SLOT_TYPE::EQUIP_ARMOR:
        return eCategory == ITEM_CATEGORY::EQUIP_ARMOR;

    case Client::SLOT_TYPE::PASSIVE:
        return eCategory == ITEM_CATEGORY::PASSIVE;

    case Client::SLOT_TYPE::SKILL:
        return eCategory == ITEM_CATEGORY::SKILL;

    case Client::SLOT_TYPE::SHORTCUT:
        return eCategory == ITEM_CATEGORY::ITEM;

    case Client::SLOT_TYPE::BLOODCODE:
        return eCategory == ITEM_CATEGORY::BLOODCODE;
        break;
    case Client::SLOT_TYPE::QUICKSLOT:
        return eCategory == ITEM_CATEGORY::SKILL;

        break;
    case Client::SLOT_TYPE::END:
        return false;
        break;
    default:
        break;
    }
    return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
void        Client::UIObj_SlotGrid::After_ApplyData()
{
    //슬롯타입지정하기
    bool bUseHoverOutline = true;

    if (m_UIType == "EQUIP_WEAPON")
        m_SlotType = SLOT_TYPE::EQUIP_WEAPON;

    if (m_UIType == "EQUIP_ARMOR")
        m_SlotType = SLOT_TYPE::EQUIP_ARMOR;


    else if (m_UIType == "PASSIVE")
        m_SlotType = SLOT_TYPE::PASSIVE;

    else if (m_UIType == "SKILL")
        m_SlotType = SLOT_TYPE::SKILL;


    else if (m_UIType == "SHORTCUT")
        m_SlotType = SLOT_TYPE::SHORTCUT;


    else if (m_UIType == "BLOODCODE")
    {
        m_SlotType = SLOT_TYPE::BLOODCODE;
        bUseHoverOutline = false;
    }

    else if (m_UIType == "QUICKSLOT")
        m_SlotType = SLOT_TYPE::QUICKSLOT;

    else if (m_UIType == "TOTAL")
        m_SlotType = SLOT_TYPE::TOTAL;
    ///////////////////////////////////////////////////////////////////
    //자기자신과 타입이같은 슬롯을 캐싱
    int i = 0;
    for (auto& pChild : m_vecChildren)
    {
        if (pChild->Get_UIType_By_Size_t() == m_iUIType)
        {
            UIObj_Slot* pSlot = dynamic_cast<UIObj_Slot*>(pChild);
            if (pSlot)
            {
                pSlot->Set_ParentGrid(this);
                pSlot->Set_Idx(i);
                pSlot->Set_UseHoverOutline(bUseHoverOutline);
                pSlot->Ready_Slot();
                m_Slots.push_back(pSlot);
                ++i;
            }
        }
    }
}
/******************************************************* 객체 준비 함수 *******************************************************/
