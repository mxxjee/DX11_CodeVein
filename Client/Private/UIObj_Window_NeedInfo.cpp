#include "Client_Define.h"
#include "UIObj_Window_NeedInfo.h"
#include "UIObj_Text.h"
#include "InventoryManager.h"

Client::UIObj_Window_NeedInfo::UIObj_Window_NeedInfo()
{
}

Client::UIObj_Window_NeedInfo::UIObj_Window_NeedInfo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UIObject(pDevice,pContext)
{
}

Client::UIObj_Window_NeedInfo::UIObj_Window_NeedInfo(const UIObj_Window_NeedInfo& original)
    :UIObject(original)
{
}

Client::UIObj_Window_NeedInfo::~UIObj_Window_NeedInfo()
{
}

//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_Window_NeedInfo::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    return S_OK;
}

HRESULT Client::UIObj_Window_NeedInfo::Initialize(void* arg)
{
    CHECK_FAILED(__super::Initialize(arg), E_FAIL);

    m_vecSubscribeNumbers.push_back(m_pGameInstance->Subscribe< NeedInfoUIEvent>([this](const NeedInfoUIEvent& e)
        {
            CHECK_JUST_NULL(e.pInfo);


            CHECK_JUST_NULL(m_HaveHaze_Text);
            CHECK_JUST_NULL(m_NeedHaze_Text);

            //현재 헤이즈양가져오기
            
            m_HaveHaze_Text->Set_Text(to_wstring(InventoryManager::GetInstance()->Get_Haze()));
            m_NeedHaze_Text->Set_Text(to_wstring(e.pInfo->m_iHaze));


        }));


    return S_OK;
}


/******************************************************* 객체 준비 함수 *******************************************************/

void        Client::UIObj_Window_NeedInfo::After_ApplyData()
{
    m_HaveHaze_Text = dynamic_cast<UIObj_Text*>(Get_Child(L"HaveHaze_Text"));
    m_NeedHaze_Text = dynamic_cast<UIObj_Text*>(Get_Child(L"NeedHaze_Text"));

}


UIObj_Window_NeedInfo* Client::UIObj_Window_NeedInfo::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    UIObj_Window_NeedInfo* pInstance = new UIObj_Window_NeedInfo(pDevice, pContext);

        MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_Window_NeedInfo 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}

GameObject* Client::UIObj_Window_NeedInfo::Clone(void* pArg)
{

    UIObj_Window_NeedInfo* pInstance = new UIObj_Window_NeedInfo(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_Window_NeedInfo 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}

void Client::UIObj_Window_NeedInfo::Free()
{
    __super::Free();
}





