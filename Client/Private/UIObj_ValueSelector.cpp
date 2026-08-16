#include "Client_Define.h"
#include "UIObj_ValueSelector.h"
#include "UIObj_Text.h"
#include "Mouse.h"
#include "CustomizingManager.h"

#include "Camera.h"
#include "UISoundUtil.h"


Client::UIObj_ValueSelector::UIObj_ValueSelector()
{
}

Client::UIObj_ValueSelector::UIObj_ValueSelector(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UIObj_CustomizeSelector(pDevice, pContext)
{

}

Client::UIObj_ValueSelector::UIObj_ValueSelector(const UIObj_ValueSelector& original)
    :UIObj_CustomizeSelector(original)
{
}

Client::UIObj_ValueSelector::~UIObj_ValueSelector()
{
}   
//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_ValueSelector::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    return S_OK;
}

HRESULT Client::UIObj_ValueSelector::Initialize(void* arg)
{
    CHECK_FAILED(__super::Initialize(arg), E_FAIL);
    
    m_vecSubscribeNumbers.push_back(m_pGameInstance->Subscribe< ValueSelectorEvent>([this](const  ValueSelectorEvent& e)
        {
            if (e.ownerID != m_iObjectID)
                return;
            for (auto& pObj : m_Arrows)
                pObj->Set_Active(true);

            Set_Enter(e.bValue);


        }));

    return S_OK;
}


/******************************************************* 객체 준비 함수 *******************************************************/

void Client::UIObj_ValueSelector::Free()
{
    __super::Free();
}

void        Client::UIObj_ValueSelector::After_ApplyData()
{
    __super::After_ApplyData();

    //타입에따라서 customtype value캐싱
    m_eCustomValueType = CUSTOMIZING_VALUE_TYPE::VALUE;

    //열릴창을 정의한다
    //m_WindowName = L"ColorPalette";

    //미리보기용 
    UIObject* pFill = Get_Child(L"Value_Fill");
    if (pFill)
    {
        m_pDisplay_ImgComp = dynamic_cast<UI_Image*>(pFill->Get_Component_FromName(Proto_UIImage));
        m_pProgressComp = dynamic_cast<UI_Progress*>(pFill->Get_Component_FromName(Proto_UIProgress));


    }


    wstring ArrowName[] = {L"Down_arrow",L"Up_arrow"};
    for (auto& wstr : ArrowName)
    {
        UIObject* pChild = Get_Child(wstr);
        if (pChild)
            m_Arrows.push_back(pChild);

    }

    m_pFill_Info = Get_Child(L"Fill_Info");
    if (m_pFill_Info)
    {
        UIObject* pText = Get_Child(L"Text_Percent_Ratio");
        if (pText)
            m_pRatioText = dynamic_cast<UIObj_Text*>(pText);

    }


    if (m_wstrName.find(L"Horizontal") != wstring::npos)
        m_eSelectorType = SelectorType::HORIZONTAL;

}


void    Client::UIObj_ValueSelector::OnHoverEnter()
{
    __super::OnHoverEnter();
    m_pGameInstance->Get_CurrentCamera()->Camera_Lock(true);


}

void    Client::UIObj_ValueSelector::OnHoverExit()
{
    __super::OnHoverExit();
    m_pGameInstance->Get_CurrentCamera()->Camera_Lock(false);


}
void        Client::UIObj_ValueSelector::OnClick()
{
    __super::OnClick();


    //클릭한부분이 얘 중간위치y보다 위면 up/아니면 down
    CHECK_FALSE(m_bEnter);


    if (m_eSelectorType == SelectorType::VERTICAL)
    {
        _float CenterY = m_Combined.m_fY;
        POINT pt = m_pMouse->Get_MousePos();

        if (CenterY > pt.y)
        {
            m_fRatio += 0.1f;
            m_fRatio = clamp<_float>(m_fRatio, 0.f, 1.f);
            m_pCustomizingManager->Update_CustomData(m_eCustomValueType, &m_fRatio);


        }

        else
        {
            m_fRatio -= 0.1f;
            m_fRatio = clamp<_float>(m_fRatio, 0.f, 1.f);
            m_pCustomizingManager->Update_CustomData(m_eCustomValueType, &m_fRatio);

       
        
        }
    }

    else
    {
        _float CenterX = m_Combined.m_fX;
        POINT pt = m_pMouse->Get_MousePos();

        if (CenterX > pt.x)
        {
            m_fRatio -= 0.1f;
            m_fRatio = clamp<_float>(m_fRatio, 0.f, 1.f);
            m_pCustomizingManager->Update_CustomData(m_eCustomValueType, &m_fRatio);


        }

        else
        {
            m_fRatio += 0.1f;
            m_fRatio = clamp<_float>(m_fRatio, 0.f, 1.f);
            m_pCustomizingManager->Update_CustomData(m_eCustomValueType, &m_fRatio);



        }
    }

   
    PlayArrowSound();

}
void        Client::UIObj_ValueSelector::On_Close()
{
    for (auto& pObj : m_Arrows)
        pObj->Set_Active(false);

    m_bEnter = false;
}
//////////////////////////////////////////////////////////////////////////////////////////////
UIObj_ValueSelector* Client::UIObj_ValueSelector::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    UIObj_ValueSelector* pInstance = new UIObj_ValueSelector(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_ValueSelector 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}

GameObject* Client::UIObj_ValueSelector::Clone(void* pArg)
{
    UIObj_ValueSelector* pInstance = new UIObj_ValueSelector(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_ValueSelector 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}

void Client::UIObj_ValueSelector::Update_Display(void* pArg)
{
    __super::Update_Display(pArg);
    CHECK_JUST_NULL(m_pProgressComp);
    CHECK_JUST_NULL(m_pRatioText);

    _float* pRatio = static_cast<_float*>(pArg);
    m_fRatio = *pRatio;

    m_pProgressComp->Set_TargetRatio(*pRatio);
    m_pProgressComp->Set_RenderRato(*pRatio);
    m_pProgressComp->Set_Ratio(*pRatio);

    _float ppRatio = *pRatio * 100.f;

    int iDisplayValue = (_int)round(ppRatio);
    m_pRatioText->Set_Text(to_wstring((int)iDisplayValue));

}


void Client::UIObj_ValueSelector::Set_Active(_bool _isActive)
{


    __super::Set_Active(_isActive);
    if (_isActive)
    {
    


        for (auto& pObj : m_Arrows)
            pObj->Set_Active(false);



    }
}
