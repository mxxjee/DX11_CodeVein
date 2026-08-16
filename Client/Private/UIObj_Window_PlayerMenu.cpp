#include "Client_Define.h"
#include "UIObj_Window_PlayerMenu.h"
#include "UIObj_Window_Inventory.h"
#include "Transform.h"
#include "UIObj_Slot.h"
#include "UIObj_Text.h"
#include "UI_Button.h"
#include "UI_Progress.h"


Client::UIObj_Window_PlayerMenu::UIObj_Window_PlayerMenu()
{
}

Client::UIObj_Window_PlayerMenu::UIObj_Window_PlayerMenu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UIObject(pDevice,pContext)
{
}

Client::UIObj_Window_PlayerMenu::UIObj_Window_PlayerMenu(const UIObj_Window_PlayerMenu& original)
    :UIObject(original)
{
}

Client::UIObj_Window_PlayerMenu::~UIObj_Window_PlayerMenu()
{
}

//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_Window_PlayerMenu::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    return S_OK;
}

HRESULT Client::UIObj_Window_PlayerMenu::Initialize(void* arg)
{
    CHECK_FAILED(__super::Initialize(arg), E_FAIL);



    m_vecSubscribeNumbers.push_back(m_pGameInstance->Subscribe<PlayerMenuEvent>([this](const PlayerMenuEvent& Event)
        {
            if (m_pSlotName)
                m_pSlotName->Set_Text(Event.SlotName);

            if (m_pItemName)
                m_pItemName->Set_Text(Event.ItemName);


        }));
    return S_OK;
}


/******************************************************* 객체 준비 함수 *******************************************************/



_int Client::UIObj_Window_PlayerMenu::Update_Priority(const _float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    return 0;
}

_int Client::UIObj_Window_PlayerMenu::Update(const _float fTimeDelta)
{
    __super::Update(fTimeDelta);
    return 0;
}

_int Client::UIObj_Window_PlayerMenu::Update_Late(const _float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
    return 0;
}

HRESULT Client::UIObj_Window_PlayerMenu::Render(const _float fTimeDelta)
{
    __super::Render(fTimeDelta);
    return 0;
}

void Client::UIObj_Window_PlayerMenu::Free()
{
    __super::Free();
}

UIObj_Window_PlayerMenu* Client::UIObj_Window_PlayerMenu::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    UIObj_Window_PlayerMenu* pInstance = new UIObj_Window_PlayerMenu(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_Window_PlayerMenu 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}


GameObject* Client::UIObj_Window_PlayerMenu::Clone(void* pArg)
{
    UIObj_Window_PlayerMenu* pInstance = new UIObj_Window_PlayerMenu(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_Window_PlayerMenu 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}

void Client::UIObj_Window_PlayerMenu::OnHoverEnter_HoverOutline(const _float2& vPos)
{
    if (!m_pHoverOutline)
        m_pHoverOutline = m_pGameInstance->Find_PersistentUI_ByName(L"Hover_Outline");
    CHECK_JUST_NULL(m_pHoverOutline);


   
    m_pHoverOutline->Set_Position(vPos.x, vPos.y);
    m_pHoverOutline->Set_Visible(true);
    m_pHoverOutline->Play_Animation(HoverAnimName);

}

void Client::UIObj_Window_PlayerMenu::OnHoverExit_HoverOutline()
{
    CHECK_JUST_NULL(m_pHoverOutline);

    m_pHoverOutline->Play_Animation(HoverExitAnimName);
}

wstring Client::UIObj_Window_PlayerMenu::Get_CategoryName(wstring Name)
{
    wstring wstrs[] = { L"Category_Status",L"Category_Grid",L"Category_Multi",L"Category_Prize",L"Category_ItemPalatte",L"Category_EmotionPalatte",L"Category_PhotoMode",L"Category_Hint",L"Category_Setting",L"Category_Exit" };
    wstring CategoryName[] = { L"스테이터스",L"소지 아이템",L"멀티 플레이",L"포상",L"아이템 팔레트",L"이모션 팔레트",L"포토 모드",L"탐색",L"컨피그",L"게임 종료" };
    
    for (int i = 0; i < wstrs->size(); ++i)
    {
        if (Name == wstrs[i])
            return CategoryName[i];
    }


    return L"";
}



///////////////////////////////////////////////////////////////////////////////////////////////////////
void        Client::UIObj_Window_PlayerMenu::After_ApplyData()
{
    __super::After_ApplyData();

    //자식캐싱

    m_pProgress = dynamic_cast<UI_Progress*>(Get_Component_FromName(Proto_UIProgress));

    UIObject* pObj = Get_Child(L"Inventory_Main");
    if (pObj)
    {
        m_pWindow_Inventory = dynamic_cast<UIObj_Window_Inventory*>(pObj);

    }

    pObj = Get_Child(L"Text_SlotName");
    if (pObj)
    {
        m_pSlotName = dynamic_cast<UIObj_Text*>(pObj);

    }


    pObj = Get_Child(L"Text_ItemName");
    if (pObj)
    {
        m_pItemName = dynamic_cast<UIObj_Text*>(pObj);

    }

    pObj = Get_Child(L"Inventory_Category_Icons");
    if (pObj)
    {
        wstring wstrs[] = { L"Category_Status",L"Category_Grid",L"Category_Multi",L"Category_Prize",L"Category_ItemPalatte",L"Category_EmotionPalatte",L"Category_PhotoMode",L"Category_Hint",L"Category_Setting",L"Category_Exit" };
        for (auto& s : wstrs)
        {
            UIObject* pChild = pObj->Get_Child(s);
            if (pChild)
            {
                UI_Button* pButton = dynamic_cast<UI_Button*>(pChild->Get_Component_FromName(Proto_UIButton));
                if (pButton)
                    pButton->Set_OnHoverFunc([this,pChild]()
                        {
                            UIObj_Window_PlayerMenu::PlayerMenuEvent Event;
                            Event.SlotName = Get_CategoryName(pChild->Get_Name());
                            Event.ItemName = L"";

                            m_pGameInstance->Publish(Event);
                        });

}
        }

    }

    /*킬때 제어가 필요한애들*/
    m_pShortCutMenu = m_pGameInstance->Find_PersistentUI_ByName(L"ShortCut_Menu");
    m_pQuickSlots.push_back(m_pGameInstance->Find_PersistentUI_ByName(L"Player_QuickSlot1"));
    m_pQuickSlots.push_back(m_pGameInstance->Find_PersistentUI_ByName(L"Player_QuickSlot2"));

}
/******************************************************* 객체 준비 함수 *******************************************************/


void Client::UIObj_Window_PlayerMenu::Set_Visible(_bool _isVisible)
{
    __super::Set_Visible(_isVisible);

    if (m_pSlotName)
        m_pSlotName->Set_Text(L"");

    if (m_pItemName)
        m_pItemName->Set_Text(L"");


    if (_isVisible)
    {
        if (m_pShortCutMenu)
            m_pShortCutMenu->Set_Active(false);

        for (auto& pSlot : m_pQuickSlots)
            pSlot->Set_Active(false);
    }

    else
    {
        CHECK_TRUE(m_pGameInstance->Get_Current_LevelID() == _UINT(LEVEL::BASE));

        //만약 하나만남앗는데 이게나라면..?
        if (m_pGameInstance->Get_WindowStackCount() == 1 &&
            m_pGameInstance->Get_Top_At_WindowStack()==this)
        {
            if (m_pShortCutMenu)
                m_pShortCutMenu->Set_Active(true);

            for (auto& pSlot : m_pQuickSlots)
                pSlot->Set_Active(true);
        }
    }

}

void Client::UIObj_Window_PlayerMenu::Set_Active(_bool _isActive)
{
    if (_isActive)
    {
        m_pProgress->Set_Ratio(0.f);
        m_pProgress->Set_TargetRatio(0.f);
        m_pProgress->Set_RenderRato(0.f);

        m_pProgress->Set_TargetRatio(1.f);




    }



    __super::Set_Active(_isActive);

}
