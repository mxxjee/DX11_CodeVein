#include "Client_Define.h"
#include "UIObj_ProgressBar.h"
#include "UI_Progress.h"



Client::UIObj_ProgressBar::UIObj_ProgressBar()
{
}

Client::UIObj_ProgressBar::UIObj_ProgressBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UIObject(pDevice, pContext)
{
}

Client::UIObj_ProgressBar::UIObj_ProgressBar(const UIObj_ProgressBar& original)
    :UIObject(original)
{
}

UIObj_ProgressBar::~UIObj_ProgressBar()
{
}
////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_ProgressBar::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    return S_OK;
}

HRESULT Client::UIObj_ProgressBar::Initialize(void* arg)
{
    CHECK_FAILED(__super::Initialize(arg), E_FAIL);
    

    return S_OK;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
void        Client::UIObj_ProgressBar::After_ApplyData()
{
    //프로그레스 컴포넌트캐싱
    UIObject* pMainObj = Get_Child(L"Fill_Main");
    Cache_ProgressComponent(pMainObj,ProgressCompType::MAIN);


    UIObject* pBackObj = Get_Child(L"Fill_Back");
    Cache_ProgressComponent(pBackObj, ProgressCompType::BACK);

}
float Client::UIObj_ProgressBar::Get_TargetRatio(bool bMain)
{
    UI_Progress* pTarget = bMain ? m_pMainProgress : m_pBackProgress;
    CHECK_NULL_RESULT(pTarget, 0.f);

    return pTarget->Get_TargetRatio();
}
float Client::UIObj_ProgressBar::Get_RenderRatio(bool bMain)
{
    UI_Progress* pTarget = bMain ? m_pMainProgress : m_pBackProgress;
    CHECK_NULL_RESULT(pTarget, 0.f);

    return pTarget->Get_RenderRatio();
}
void Client::UIObj_ProgressBar::Set_CurrentFloat(_float* pCurrent)
{
    if (m_pMainProgress)
        m_pMainProgress->Set_Current(pCurrent);
 
    if (m_pBackProgress)
        m_pBackProgress->Set_Current(pCurrent);

    m_fCurrent = pCurrent;

}
void Client::UIObj_ProgressBar::Set_MaxFloat(_float* fMax)
{
    if (m_pMainProgress)
        m_pMainProgress->Set_Max(fMax);

    if (m_pBackProgress)
        m_pBackProgress->Set_Max(fMax);
    m_fMax = fMax;
}

/******************************************************* 객체 준비 함수 *******************************************************/


_int Client::UIObj_ProgressBar::Update_Priority(const _float fTimeDelta)
{
    __super::Update_Priority(fTimeDelta);
    return 0;
}

_int Client::UIObj_ProgressBar::Update(const _float fTimeDelta)
{
   __super::Update(fTimeDelta);

    return 0;
}

_int Client::UIObj_ProgressBar::Update_Late(const _float fTimeDelta)
{
   
    __super::Update_Late(fTimeDelta);
    return 0;
}

HRESULT Client::UIObj_ProgressBar::Render(const _float fTimeDelta)
{
    __super::Render(fTimeDelta);
    return S_OK;
}

UIObj_ProgressBar* Client::UIObj_ProgressBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    UIObj_ProgressBar* pInstance = new UIObj_ProgressBar(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_ProgressBar 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}

GameObject* Client::UIObj_ProgressBar::Clone(void* pArg)
{
    UIObj_ProgressBar* pInstance = new UIObj_ProgressBar(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_ProgressBar 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}

void Client::UIObj_ProgressBar::Free()
{
    Safe_Release(m_pMainObj);
    Safe_Release(m_pBackObj);
    __super::Free();
}

/////////////////////////////////////ETC//////////////////////////////////////
void Client::UIObj_ProgressBar::Cache_ProgressComponent(UIObject* pObj,ProgressCompType eType)
{
    CHECK_JUST_NULL(pObj);

	Base* pBase = pObj->Get_Component_FromName(Proto_UIProgress);
	if (pBase)
	{
		UI_Progress* pProgressComp = dynamic_cast<UI_Progress*>(pBase);
        if (pProgressComp)
        {
            if (eType == ProgressCompType::MAIN)
            {
                m_pMainObj = pObj;
                m_pMainProgress = dynamic_cast<UI_Progress*>(pProgressComp);
                Safe_AddRef(m_pMainObj);
            }

            else
            {
                m_pBackObj = pObj;
                m_pBackProgress = dynamic_cast<UI_Progress*>(pProgressComp);
                Safe_AddRef(m_pBackObj);
            }
               
        }


	}


}


/////////////////////////////////////////////////////////////////
void        Client::UIObj_ProgressBar::Set_MinHue(bool bMain, _float vMinHue)
{
    UIObject* pTarget = bMain ? m_pMainObj : m_pBackObj;
    CHECK_JUST_NULL(pTarget);

    UI_Image* pImage = dynamic_cast<UI_Image*>(pTarget->Get_UIRenderComponent());
    CHECK_JUST_NULL(pImage);

    
    pImage->Set_MinHue(vMinHue);
}
void        Client::UIObj_ProgressBar::Set_MaxHue(bool bMain, _float vMaxHue)
{
    UIObject* pTarget = bMain ? m_pMainObj : m_pBackObj;
    CHECK_JUST_NULL(pTarget);

    UI_Image* pImage = dynamic_cast<UI_Image*>(pTarget->Get_UIRenderComponent());
    CHECK_JUST_NULL(pImage);


    pImage->Set_MinHue(vMaxHue);
}
void        Client::UIObj_ProgressBar::Set_UseRandomColor(bool bMain, bool b)
{
    UIObject* pTarget = bMain ? m_pMainObj : m_pBackObj;
    CHECK_JUST_NULL(pTarget);

    UI_Image* pImage = dynamic_cast<UI_Image*>(pTarget->Get_UIRenderComponent());
    CHECK_JUST_NULL(pImage);


    pImage->Set_UseRandomColor(b);

}

void Client::UIObj_ProgressBar::Init_Ratio(bool bMain, _float fValue)
{

    UI_Progress* pTarget = bMain ? m_pMainProgress : m_pBackProgress;
    CHECK_JUST_NULL(pTarget);

    pTarget->Set_PreRatio(fValue);
    pTarget->Set_Ratio(fValue);
    pTarget->Set_TargetRatio(fValue);
    pTarget->Set_RenderRato(fValue);
}

void Client::UIObj_ProgressBar::Compute_Ratio(bool bMain)
{
    UI_Progress* pTarget = bMain ? m_pMainProgress : m_pBackProgress;
    CHECK_JUST_NULL(pTarget);

    pTarget->Compute_Ratio();
    

}

void Client::UIObj_ProgressBar::Set_TargetRatio(bool bMain, _float Value)
{
    UI_Progress* pTarget = bMain ? m_pMainProgress : m_pBackProgress;
    CHECK_JUST_NULL(pTarget);

    pTarget->Set_TargetRatio(Value);
}

void Client::UIObj_ProgressBar::Set_RenderRatio(bool bMain, _float fValue)
{
    UI_Progress* pTarget = bMain ? m_pMainProgress : m_pBackProgress;
    CHECK_JUST_NULL(pTarget);

    pTarget->Set_RenderRato(fValue);
}
