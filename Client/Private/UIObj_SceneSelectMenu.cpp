#include "Client_Define.h"
#include "UIObj_SceneSelectMenu.h"
#include "UIObj_SceneSlot.h"
#include "UISoundUtil.h"


/////////////////////////////////////////////
Client::UIObj_SceneSelectMenu::UIObj_SceneSelectMenu()
{
}

Client::UIObj_SceneSelectMenu::UIObj_SceneSelectMenu(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UIObject(pDevice, pContext)
{
}

Client::UIObj_SceneSelectMenu::UIObj_SceneSelectMenu(const UIObj_SceneSelectMenu& original)
    :UIObject(original)
{
}

Client::UIObj_SceneSelectMenu::~UIObj_SceneSelectMenu()
{
}
///////////////////////////////////////////////////////

//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_SceneSelectMenu::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    return S_OK;
}

HRESULT Client::UIObj_SceneSelectMenu::Initialize(void* arg)
{
    CHECK_FAILED(__super::Initialize(arg), E_FAIL);
   
    
    m_vecSubscribeNumbers.push_back(m_pGameInstance->Subscribe< SceneSlotMenuEvent>([this](const SceneSlotMenuEvent& Event)
        {
            if (Event.eType == SelectMenuEvenType::MINUS_IDX)
            {
                --m_iIdx;
                m_iIdx = clamp<int>(m_iIdx, 0, m_iMaxIdx);
                Change_Idx();
            }
        }));


    m_vecSubscribeNumbers.push_back(m_pGameInstance->Subscribe< SceneSlotMenuEvent>([this](const SceneSlotMenuEvent& Event)
        {
            if (Event.eType == SelectMenuEvenType::PLUS_IDX)
            {
                ++m_iIdx;
                m_iIdx = clamp<int>(m_iIdx, 0, m_iMaxIdx);
                Change_Idx();
            }
        }));
    return S_OK;


}
_int Client::UIObj_SceneSelectMenu::Update_Priority(const _float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);

    //왼쪽누르면 포커스되는 인덱스 1줄이기
    if (m_pGameInstance->KeyDown(DIK_LEFTARROW))
    {
        
       
        --m_iIdx;
        m_iIdx = clamp<int>(m_iIdx, 0, m_iMaxIdx);
        Change_Idx();

    }



    if (m_pGameInstance->KeyDown(DIK_RIGHTARROW))
    {
       
        ++m_iIdx;
        m_iIdx = clamp<int>(m_iIdx, 0, m_iMaxIdx);
        Change_Idx();
    }


    return 0;
}

_int Client::UIObj_SceneSelectMenu::Update(const _float fTimeDelta)
{
    __super::Update(fTimeDelta);

    return 0;
}

_int Client::UIObj_SceneSelectMenu::Update_Late(const _float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);

    return 0;
}

HRESULT Client::UIObj_SceneSelectMenu::Render(const _float fTimeDelta)
{
    __super::Render(fTimeDelta);
    return 0;
}


UIObj_SceneSelectMenu* Client::UIObj_SceneSelectMenu::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    UIObj_SceneSelectMenu* pInstance = new UIObj_SceneSelectMenu(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_SceneSelectMenu 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}
GameObject* Client::UIObj_SceneSelectMenu::Clone(void* pArg)
{
    UIObj_SceneSelectMenu* pInstance = new UIObj_SceneSelectMenu(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_SceneSelectMenu 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}




///////////////////////////////////////////////////////////////////////////////////////////////////////
void        Client::UIObj_SceneSelectMenu::After_ApplyData()
{
    __super::After_ApplyData();
    //자식캐싱
   

    //sceneslotMenu캐싱
    wstring strs[] = { L"SceneSlot",L"SceneSlot1",L"SceneSlot2",L"SceneSlot3"};
    _uint i = 0;
    for (auto& str : strs)
    {
        UIObject* pObj = Get_Child(str);
        if (pObj)
        {
            UIObj_SceneSlot* pSlot = dynamic_cast<UIObj_SceneSlot*>(pObj);
            if (pSlot)
            {
                pSlot->Set_Idx(i);
                m_pSceneSlots.push_back(pSlot);
                ++i;

            }
        }
        

    }
    m_iMaxIdx = m_pSceneSlots.size() - 1;


}
/******************************************************* 객체 준비 함수 *******************************************************/

void Client::UIObj_SceneSelectMenu::Change_Idx()
{
    PlayArrowSound();

    //현재 포커싱된 idx빼고 상태변경
    //이벤트전송
    UIObj_SceneSlot::SceneSlotEvent SlotEvent;
    SlotEvent.m_iFocusIdx = m_iIdx;

    //단 1회 보내기!
    if (m_pSceneSlots[0] && m_Init)
    {
        UITransform Transform=m_pSceneSlots[0]->Get_LocalTransform();
        for (auto& pSlot : m_pSceneSlots)
        {
            pSlot->Set_InitOrigin(_float2(Transform.m_fX, Transform.m_fY));
            pSlot->Set_InitSize(_float2(Transform.m_fSizeX, Transform.m_fSizeY));

        }
        m_Init = false;

    }

    m_pGameInstance->Publish(SlotEvent);

}

void        Client::UIObj_SceneSelectMenu::Set_Active(_bool _isActive)
{
    if (_isActive)
    {
        //0부터시작
        m_iIdx = 0;
        Change_Idx();


    }


    __super::Set_Active(_isActive);
    

}

void Client::UIObj_SceneSelectMenu::Free()
{

    __super::Free();
}
