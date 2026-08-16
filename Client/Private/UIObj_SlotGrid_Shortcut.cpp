#include "Client_Define.h"
#include "UIObj_SlotGrid_Shortcut.h"
#include "UIObj_Slot.h"
#include "UIObj_ShortCutQuickSlot.h"

Client::UIObj_SlotGrid_Shortcut::UIObj_SlotGrid_Shortcut()
{
}

Client::UIObj_SlotGrid_Shortcut::UIObj_SlotGrid_Shortcut(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UIObj_SlotGrid(pDevice,pContext)
{
}

Client::UIObj_SlotGrid_Shortcut::UIObj_SlotGrid_Shortcut(const UIObj_SlotGrid_Shortcut& original)
    :UIObj_SlotGrid(original)
{
}

Client::UIObj_SlotGrid_Shortcut::~UIObj_SlotGrid_Shortcut()
{
}

//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_SlotGrid_Shortcut::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    return S_OK;
}

HRESULT Client::UIObj_SlotGrid_Shortcut::Initialize(void* arg)
{
    CHECK_FAILED(__super::Initialize(arg), E_FAIL);

    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

_int Client::UIObj_SlotGrid_Shortcut::Update_Priority(const _float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    return 0;
}

_int Client::UIObj_SlotGrid_Shortcut::Update(const _float fTimeDelta)
{
    __super::Update(fTimeDelta);
    return 0;
}

_int Client::UIObj_SlotGrid_Shortcut::Update_Late(const _float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
    return 0;
}


HRESULT Client::UIObj_SlotGrid_Shortcut::Render(const _float fTimeDelta)
{
    __super::Render(fTimeDelta);
    return 0;
}

void Client::UIObj_SlotGrid_Shortcut::Free()
{
    __super::Free();
}

/// /////////////////////////////////////////////////////////////////////
UIObj_SlotGrid_Shortcut* Client::UIObj_SlotGrid_Shortcut::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    UIObj_SlotGrid_Shortcut* pInstance = new UIObj_SlotGrid_Shortcut(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_SlotGrid_Shortcut 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}


GameObject* Client::UIObj_SlotGrid_Shortcut::Clone(void* pArg)
{
    UIObj_SlotGrid* pInstance = new UIObj_SlotGrid_Shortcut(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_SlotGrid_Shortcut 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////
void        Client::UIObj_SlotGrid_Shortcut::After_ApplyData()
{
    m_SlotType = SLOT_TYPE::SHORTCUT_QUICK;


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
                pSlot->Set_UseHoverOutline(false);
                pSlot->Ready_Slot();
       
                m_Slots.push_back(pSlot);

                m_ShortCutSlots.push_back(dynamic_cast<UIObj_ShortCutQuickSlot*>(pSlot));

                ++i;
            }
        }
    }
}
/******************************************************* 객체 준비 함수 *******************************************************/

