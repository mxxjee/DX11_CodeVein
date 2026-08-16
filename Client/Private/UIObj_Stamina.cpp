#include "Client_Define.h"
#include "UIObj_Stamina.h"

Client::UIObj_Stamina::UIObj_Stamina()
{
}

Client::UIObj_Stamina::UIObj_Stamina(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UIObj_ProgressBar(pDevice,pContext)
{
}

Client::UIObj_Stamina::UIObj_Stamina(const UIObj_Stamina& original)
    :UIObj_ProgressBar(original)
{
}

Client::UIObj_Stamina::~UIObj_Stamina()
{
}


//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_Stamina::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    return S_OK;
}

HRESULT Client::UIObj_Stamina::Initialize(void* arg)
{
    CHECK_FAILED(__super::Initialize(arg), E_FAIL);

  
    //스테미너 부족시 이벤트(실행시 blink)
    UIOwnerEventInfo Info;
    Info.Hash = hash<string>{}("OnStaminaLow");
    Info.m_Fuc = [this](UI_MasterEvent& e)
        {
            CHECK_TRUE(pRedBlinkImage->Is_Active());

            if (pRedBlinkImage)
                pRedBlinkImage->Set_Active(true);
        };
    m_Actions.emplace("OnStaminaLow" , Info);

    //스테미너 red 비활성화이벤트(다시채워졌음을 알림)
    Info = {};
    Info.Hash = hash<string>{}("OnStaminaRecovered");
    Info.m_Fuc = [this](UI_MasterEvent& e)
        {
            if (!pRedBlinkImage->Is_Active())
                return;

            if (pRedBlinkImage)
                pRedBlinkImage->Set_Active(false);
        };
    m_Actions.emplace("OnStaminaRecovered", Info);


    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////
_int Client::UIObj_Stamina::Update_Priority(const _float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    return 0;
}

_int Client::UIObj_Stamina::Update(const _float fTimeDelta)
{
    
    return 0;
}

_int Client::UIObj_Stamina::Update_Late(const _float fTimeDelta)
{
    __super::Update_Late(fTimeDelta);
    return 0;
}

HRESULT Client::UIObj_Stamina::Render(const _float fTimeDelta)
{
    __super::Render(fTimeDelta);
    return S_OK;
}

UIObj_Stamina* Client::UIObj_Stamina::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    UIObj_Stamina* pInstance = new UIObj_Stamina(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_Stamina 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}

GameObject* Client::UIObj_Stamina::Clone(void* pArg)
{
    UIObj_Stamina* pInstance = new UIObj_Stamina(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_Stamina 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}

void Client::UIObj_Stamina::Free()
{
    Safe_Release(pRedBlinkImage);
    __super::Free();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
void        Client::UIObj_Stamina::After_ApplyData()
{
    __super::After_ApplyData();
    //자식캐싱
    UIObject* pTarget = Get_Child(L"Stamina_Red");
    if (pTarget)
    {
        pRedBlinkImage = pTarget;
        Safe_AddRef(pRedBlinkImage);
    }

}
/******************************************************* 객체 준비 함수 *******************************************************/
void Client::UIObj_Stamina::Execute_By_Event(const string& strActionName, void* pArg)
{
    //hitrate깜빡거리기
    UI_MasterEvent* Event = static_cast<UI_MasterEvent*>(pArg);
    auto iter = m_Actions.find(strActionName);
    CHECK_TRUE(iter == m_Actions.end());

    if (Event->m_ActionStrHash == iter->second.Hash)
    {
        if (iter->second.m_Fuc)
            iter->second.m_Fuc(*Event);
    }
}

////////////////////////////////////////////////////////////////////////////
void Client::UIObj_Stamina::Set_TargetValue(float* pTarget)
{
    m_fCurrent = pTarget;
    
    if (m_pMainProgress)
        m_pMainProgress->Set_Current(m_fCurrent);

    if (m_pBackProgress)
        m_pBackProgress->Set_Current(m_fCurrent);


}

void Client::UIObj_Stamina::Set_MaxValue(float* pMax)
{
    m_fCurrent = pMax;
    //처음에 채워진상태로바로계산
    if (m_pMainProgress)
    {
        m_pMainProgress->Set_Max(m_fCurrent);
        m_pMainProgress->Compute_Ratio();

    }
    if (m_pBackProgress)
    {
        m_pBackProgress->Set_Max(m_fCurrent);
        m_pMainProgress->Compute_Ratio();

    }

    

}
//////////////////////////////////////////////