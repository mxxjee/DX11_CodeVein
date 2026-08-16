#include "Client_Define.h"
#include "UIObj_ColorSelector.h"

#include "CustomizingManager.h"
#include "UIObj_CustomizeGrid.h"

Client::UIObj_ColorSelector::UIObj_ColorSelector()
{
}

Client::UIObj_ColorSelector::UIObj_ColorSelector(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:UIObj_CustomizeSelector(pDevice,pContext)
{
}

Client::UIObj_ColorSelector::UIObj_ColorSelector(const UIObj_ColorSelector& original)
	:UIObj_CustomizeSelector(original)
{
}

Client::UIObj_ColorSelector::~UIObj_ColorSelector()
{
}
//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT Client::UIObj_ColorSelector::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    return S_OK;
}

HRESULT Client::UIObj_ColorSelector::Initialize(void* arg)
{
    CHECK_FAILED(__super::Initialize(arg), E_FAIL);
    m_vecSubscribeNumbers.push_back(m_pGameInstance->Subscribe< ColorSelectorUIEvent>([this](const ColorSelectorUIEvent& e)
        {

            if (e.eType == EventType::CHANGE_CLOTHESIDX)
            {
                if (m_eCustomizeType == CUSTOMIZING_TYPE::CLOTHES && e.iSlotIdx==m_iIdx)
                {
                    CustomColorInfo* pDefaultInfo = m_pCustomizingManager->Get_InitialColor(m_eCustomizeType, e.Idx);
                    if (pDefaultInfo)
                    {
                        Update_Display(&pDefaultInfo->vColor);
                    }
                }
            }
        }));

    return S_OK;
}


/******************************************************* 객체 준비 함수 *******************************************************/
//////////////////////////////////////////////////////////////////////////////////////////////
UIObj_ColorSelector* Client::UIObj_ColorSelector::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    UIObj_ColorSelector* pInstance = new UIObj_ColorSelector(pDevice, pContext);

    MSG_FAIL(pInstance->Initialize_Prototype(iLevel), L"UIObj_ColorSelector 원본 생성 실패", L"경고!!!", nullptr);

    return pInstance;
}

GameObject* Client::UIObj_ColorSelector::Clone(void* pArg)
{
    UIObj_ColorSelector* pInstance = new UIObj_ColorSelector(*this);

    MSG_FAIL(pInstance->Initialize(pArg), L"UIObj_ColorSelector 복사 실패", L"경고!!!", nullptr);

    return pInstance;
}


//////////////////////               
void        Client::UIObj_ColorSelector::After_ApplyData()
{
    __super::After_ApplyData();

    //타입에따라서 customtype value캐싱
    m_eCustomValueType = CUSTOMIZING_VALUE_TYPE::COLOR;

    //열릴창을 정의한다
    m_WindowName = L"ColorPalette";

    //미리보기용 
    UIObject* pFill = Get_Child(L"Color_Fill");
    if (pFill)
    {
        m_pDisplay_ImgComp = dynamic_cast<UI_Image*>(pFill->Get_Component_FromName(Proto_UIImage));
    }



}

void Client::UIObj_ColorSelector::OnHoverEnter()
{
    __super::OnHoverEnter();

   

}

void Client::UIObj_ColorSelector::OnHoverExit()
{
    __super::OnHoverExit();

  

}

void Client::UIObj_ColorSelector::Free()
{
    __super::Free();
}
