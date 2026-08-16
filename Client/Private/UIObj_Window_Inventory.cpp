#include "Client_Define.h"
#include "UIObj_Window_Inventory.h"
#include "UIObj_SlotGrid.h"

Client::UIObj_Window_Inventory::UIObj_Window_Inventory()
{
}

Client::UIObj_Window_Inventory::UIObj_Window_Inventory(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UIObject(pDevice,pContext)
{
}

Client::UIObj_Window_Inventory::UIObj_Window_Inventory(const UIObj_Window_Inventory& original)
    :UIObject(original)
{
}

Client::UIObj_Window_Inventory::~UIObj_Window_Inventory()
{
}

//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_Window_Inventory::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    return S_OK;
}

HRESULT Client::UIObj_Window_Inventory::Initialize(void* arg)
{
    CHECK_FAILED(__super::Initialize(arg), E_FAIL);


    return S_OK;
}


/******************************************************* 객체 준비 함수 *******************************************************/



_int Client::UIObj_Window_Inventory::Update_Priority(const _float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
	return 0;
}

_int Client::UIObj_Window_Inventory::Update(const _float fTimeDelta)
{
    __super::Update(fTimeDelta);
    return 0;
}

_int Client::UIObj_Window_Inventory::Update_Late(const _float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
    return 0;
}

HRESULT Client::UIObj_Window_Inventory::Render(const _float fTimeDelta)
{
    __super::Render(fTimeDelta);
    return 0;
}

void Client::UIObj_Window_Inventory::Free()
{
    __super::Free();
}


UIObj_Window_Inventory* Client::UIObj_Window_Inventory::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    UIObj_Window_Inventory* pInstance = new UIObj_Window_Inventory(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_Window_Inventory 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}


GameObject* Client::UIObj_Window_Inventory::Clone(void* pArg)
{
    UIObj_Window_Inventory* pInstance = new UIObj_Window_Inventory(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_Window_Inventory 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
void        Client::UIObj_Window_Inventory::After_ApplyData()
{
    //자식캐싱
    const wstring wstrs[] = { L"Inventory_Main_WeaponEquips",L"Inventory_Main_ArmorEquip",L"Inventory_Quick_Rights",L"Inventory_Quick_Lefts", L"Inventory_Passives",L"Inventory_ItemSlots",L"Inventory_BloodCode"};
    const SLOT_TYPE Types[] = { SLOT_TYPE::EQUIP_WEAPON,SLOT_TYPE::EQUIP_ARMOR,SLOT_TYPE::SKILL,SLOT_TYPE::SKILL,SLOT_TYPE::PASSIVE,SLOT_TYPE::QUICKSLOT,SLOT_TYPE::BLOODCODE };

    for (_uint i = 0; i < 7; ++i)
    {
        UIObject* pEquipGrid = Get_Child(wstrs[i]);
        if (pEquipGrid)
        {
            UIObj_SlotGrid* ppGrid = dynamic_cast<UIObj_SlotGrid*>(pEquipGrid);
            if (ppGrid)
            {
                ppGrid->Set_Idx(i);
                m_GridsMap.emplace(Types[i], ppGrid);
                
            }
        }
    }
   
  


}
/******************************************************* 객체 준비 함수 *******************************************************/


