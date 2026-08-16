#include "Client_Define.h"
#include "UIObj_ShortCutMenu.h"

#include "UIObj_SlotGrid_Shortcut.h"
#include "UIObj_Text.h"
#include "UIObj_BitmapFont.h"
#include "InventoryManager.h"

#include "UIObj_ShortCutQuickSlot.h"
#include "UISoundUtil.h"



/////////////////////////////////////////////
Client::UIObj_ShortCutMenu::UIObj_ShortCutMenu()
{
}

Client::UIObj_ShortCutMenu::UIObj_ShortCutMenu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UIObject(pDevice,pContext)
{
}

Client::UIObj_ShortCutMenu::UIObj_ShortCutMenu(const UIObj_ShortCutMenu& original)
    :UIObject(original)
{
}

Client::UIObj_ShortCutMenu::~UIObj_ShortCutMenu()
{
}
///////////////////////////////////////////////////////


//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_ShortCutMenu::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    return S_OK;
}

HRESULT Client::UIObj_ShortCutMenu::Initialize(void* arg)
{
    CHECK_FAILED(__super::Initialize(arg), E_FAIL);
    m_pInventoryManager = InventoryManager::GetInstance();


    return S_OK;
}

_int Client::UIObj_ShortCutMenu::Update_Priority(const _float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    return 0;
}

_int Client::UIObj_ShortCutMenu::Update(const _float fTimeDelta)
{

    __super::Update(fTimeDelta);
    //왼쪽누르면 포커스되는 인덱스 1줄이기
    if (m_pGameInstance->KeyDown(DIK_LEFTARROW))
    {
        m_pInventoryManager->Change_Shortcut_Focus(-1);
        Change_State(ShortCutMenu_State::EXTRA);

        PlaySelectSound();
        
    }



    if (m_pGameInstance->KeyDown(DIK_RIGHTARROW))
    {
        m_pInventoryManager->Change_Shortcut_Focus(1);
        Change_State(ShortCutMenu_State::EXTRA);
        PlaySelectSound();
    }

    return 0;
}

_int Client::UIObj_ShortCutMenu::Update_Late(const _float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);

   

    Update_State(fTimeDelta);
    return 0;
}

HRESULT Client::UIObj_ShortCutMenu::Render(const _float fTimeDelta)
{

    __super::Render(fTimeDelta);
    return 0;
}


UIObj_ShortCutMenu* Client::UIObj_ShortCutMenu::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    UIObj_ShortCutMenu* pInstance = new UIObj_ShortCutMenu(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_ShortCutMenu 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}

GameObject* Client::UIObj_ShortCutMenu::Clone(void* pArg)
{

    UIObj_ShortCutMenu* pInstance = new UIObj_ShortCutMenu(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_ShortCutMenu 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}

void Client::UIObj_ShortCutMenu::Free()
{
 
    __super::Free();
}

void Client::UIObj_ShortCutMenu::Publish_ModeEvent(ShortCutMenu_State eMode)
{
    UIObj_ShortCutQuickSlot::tagShortCutQuickSlotEvent Event;
    Event.eType = UIObj_ShortCutQuickSlot::EventType::UPDATE_MENUMODE;
    Event.ShortCutMenuMode = _UINT(eMode);


    m_pGameInstance->Publish(Event);


}

void Client::UIObj_ShortCutMenu::Publish_SlotDataEvent()
{
    UIObj_ShortCutQuickSlot::tagShortCutQuickSlotEvent Event;
    Event.eType = UIObj_ShortCutQuickSlot::EventType::UPDATE_EQUIPDATA;


    m_pGameInstance->Publish(Event);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
void        Client::UIObj_ShortCutMenu::After_ApplyData()
{
    //자식캐싱
    UIObject* pSlotGrid = Get_Child(L"SlotGrid");
    if (pSlotGrid)
    {
        m_pSlotGrid = dynamic_cast<UIObj_SlotGrid_Shortcut*>(pSlotGrid);

    }

    const wstring ArrowNames[] = { L"Arrow_Left_Back",L"Arrow_Right_Back" };
    
    for (int i = 0; i < 2; ++i)
    {

        m_Arrows.push_back(Get_Child(ArrowNames[i]));
        vInitPosX[i] = m_Arrows.back()->Get_LocalTransform().m_fX;
    }

    UIObject* pText_Name = Get_Child(L"Text_Name");
    if (pText_Name)
    {
        m_pText_Name = dynamic_cast<UIObj_Text*>(pText_Name);
    }




}
/******************************************************* 객체 준비 함수 *******************************************************/


void        Client::UIObj_ShortCutMenu::Set_Active(_bool _isActive)
{
    /*입네토리 매니저에게 물어봐서 active판단*/
    if (_isActive)
    {
        bool bActive = m_pInventoryManager->Can_Active_ShortCutMenu();

        //슬롯들이 참조할 정보를 update한다.
        m_pInventoryManager->Refresh_Display_QuickSlots();

        __super::Set_Active(bActive); 
        //안불러와져서 직접호출
        m_eState = ShortCutMenu_State::DEFAULT;
        Publish_ModeEvent(ShortCutMenu_State::DEFAULT);
        Publish_SlotDataEvent();

        int i = 0;
        for (auto& pArrow : m_Arrows)
        {
            pArrow->Set_Position(vInitPosX[i],pArrow->Get_LocalTransform().m_fY);
            ++i;
        }


        if (m_pText_Name)
        { 
            pFocusInfo = m_pInventoryManager->Get_ShorcutFocusItem();
            if(pFocusInfo)
                m_pText_Name->Set_Text(pFocusInfo->ItemName);

        }
    }

    else
        __super::Set_Active(_isActive);


   
}

void Client::UIObj_ShortCutMenu::Change_State(ShortCutMenu_State State)
{
    if (m_pText_Name)
    {
        pFocusInfo = m_pInventoryManager->Get_ShorcutFocusItem();
        CHECK_JUST_NULL(pFocusInfo);    
        m_pText_Name->Set_Text(pFocusInfo->ItemName);

    }


    switch (State)
    {
    case Client::UIObj_ShortCutMenu::ShortCutMenu_State::EXTRA:
    {
        m_fTime = 0.f;
        if (m_eState != State)
        {
            for (auto& p : m_Arrows)
                p->Play_Animation(ExtraAnimKey);

            //슬롯들에게 이벤트전송~
            Publish_ModeEvent(ShortCutMenu_State::EXTRA);

         
        }

        

    }
        break;
    case Client::UIObj_ShortCutMenu::ShortCutMenu_State::DEFAULT:
        if (m_eState != State)
        {
            for (auto& p : m_Arrows)
                p->Play_Animation(DefaultAnimKey);


            //슬롯들에게 이벤트전송~
            Publish_ModeEvent(ShortCutMenu_State::DEFAULT);



        }
        break;
    default:
        break;
    }

    m_eState = State;
}

void Client::UIObj_ShortCutMenu::Update_State(const _float fTimeDelta)
{
    switch (m_eState)
    {
    case ShortCutMenu_State::EXTRA:
    {
        m_fTime += fTimeDelta;
        if (m_fTime >= 3.f)
        {
            Change_State(ShortCutMenu_State::DEFAULT);
        }
    }
        break;

    case ShortCutMenu_State::DEFAULT:
        break;

    default:
        break;
    }
}

