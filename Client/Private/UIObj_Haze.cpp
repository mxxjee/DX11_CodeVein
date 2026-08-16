#include "Client_Define.h"
#include "UIObj_Haze.h"
#include "UIObj_BitmapFont.h"
#include "UIObj_Text.h"

Client::UIObj_Haze::UIObj_Haze()
{
}

Client::UIObj_Haze::UIObj_Haze(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UIObject(pDevice, pContext)
{
}

Client::UIObj_Haze::UIObj_Haze(const UIObj_Haze& original)
    :UIObject(original)
{
}

Client::UIObj_Haze::~UIObj_Haze()
{
}
//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_Haze::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    return S_OK;
}

HRESULT Client::UIObj_Haze::Initialize(void* arg)
{
    CHECK_FAILED(__super::Initialize(arg), E_FAIL);
    m_vecSubscribeNumbers.push_back(m_pGameInstance->Subscribe<HazeUIEvent>([this](const HazeUIEvent& event)
        {
            if (event.eType == HazeUIEventType::UPDATE_CURRENTHAZE)
            {
                if (m_pCurrentHazeText)
                {
                    int* iHaze = static_cast<int*>(event.pArg);
                    m_pCurrentHazeText->Set_Text(to_wstring(*iHaze));
                
                }
             }

            else if (event.eType == HazeUIEventType::UPDATE_NEWHAZE)
            {
                if (m_pPlusText)
                {

                    int* iHaze = static_cast<int*>(event.pArg);
                    if (*iHaze > 0)
                        m_pPlusText->Set_Text(L"+");

                    else
                        m_pPlusText->Set_Text(L"-");


                    m_pNewHazeText->Set_Text(to_wstring(*iHaze));



                    m_pNewHaze->Set_Visible(true);
                    m_pNewHaze->Set_Active(true);

                    m_pGlow->Set_Visible(true);
                    m_pGlow->Set_Active(true);

                }
            }

        }));
    
    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////

_int Client::UIObj_Haze::Update_Priority(const _float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    return 0;
}

_int Client::UIObj_Haze::Update(const _float fTimeDelta)
{
    __super::Update(fTimeDelta);

   

    return 0;
}

_int Client::UIObj_Haze::Update_Late(const _float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
    return 0;
}

HRESULT Client::UIObj_Haze::Render(const _float fTimeDelta)
{

    __super::Render(fTimeDelta);
    return S_OK;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
void        Client::UIObj_Haze::After_ApplyData()
{
    //자식캐싱
    __super::After_ApplyData();

    m_pGlow = Get_Child(L"Glow");
    m_pNewHaze = Get_Child(L"NewHaze");


    m_pCurrentHazeText = dynamic_cast<UIObj_BitmapFont*>(Get_Child(L"Haze_Text"));
    if (m_pNewHaze)
    {
        m_pNewHazeText = dynamic_cast<UIObj_BitmapFont*>(m_pNewHaze->Get_Child(L"NewHaze_Text"));
        m_pPlusText = dynamic_cast<UIObj_Text*>(m_pNewHaze->Get_Child(L"Plus_Text"));

    }

}

UIObj_Haze* Client::UIObj_Haze::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    UIObj_Haze* pInstance = new UIObj_Haze(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_Haze 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}

GameObject* Client::UIObj_Haze::Clone(void* pArg)
{
    UIObj_Haze* pInstance = new UIObj_Haze(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_Haze 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}

void Client::UIObj_Haze::Free()
{
    __super::Free();
}
