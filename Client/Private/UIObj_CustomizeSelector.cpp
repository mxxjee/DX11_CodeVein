#include "Client_Define.h"
#include "UIObj_CustomizeSelector.h"
#include "UIObj_CustomizeGrid.h"
#include "CustomizingManager.h"
#include "UISoundUtil.h"

Client::UIObj_CustomizeSelector::UIObj_CustomizeSelector()
{
}

Client::UIObj_CustomizeSelector::UIObj_CustomizeSelector(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :UIObject(pDevice,pContext)
{
}

Client::UIObj_CustomizeSelector::UIObj_CustomizeSelector(const UIObj_CustomizeSelector& original)
    :UIObject(original)
{
}

Client::UIObj_CustomizeSelector::~UIObj_CustomizeSelector()
{
}

HRESULT Client::UIObj_CustomizeSelector::Initialize(void* arg)
{
    CHECK_FAILED(__super::Initialize(arg), E_FAIL);
    m_pCustomizingManager = CustomizingManager::GetInstance();
    m_vecSubscribeNumbers.push_back(m_pGameInstance->Subscribe< CustomizeSelectorUIEvent>([this](const CustomizeSelectorUIEvent& e)
        {
            if (e.eType == EventType::SET_DEFAULTCOLOR)
            {
                switch (m_eCustomValueType)
                {
                case CUSTOMIZING_VALUE_TYPE::COLOR:
                {
                    CustomColorInfo* pDefaultInfo = m_pCustomizingManager->Get_InitialColor(m_eCustomizeType, m_iIdx);
                    if (pDefaultInfo)
                        Update_Display(&pDefaultInfo->vColor);
                }
                    break;


                case CUSTOMIZING_VALUE_TYPE::TEXTURE:
                case CUSTOMIZING_VALUE_TYPE::MESH:
                {
                    CustomItemGridInfo* pInfo= m_pCustomizingManager->Get_InitialTexture(m_eCustomizeType, m_iIdx);
                    if (pInfo)
                    {
                        _uint pDefaultInfo = pInfo->TexIdx;
                        if (pDefaultInfo)
                             Update_Display(&pDefaultInfo);
                    }
                    
                }
                break;

                case CUSTOMIZING_VALUE_TYPE::VALUE:
                {
                    CustomAlphaValueInfo* pInfo = m_pCustomizingManager->Get_InitialValue(m_eCustomizeType, m_iIdx);
                    if (pInfo)
                    {
                        _float pDefaultInfo = pInfo->fRatio;
                        Update_Display(&pDefaultInfo);
                    }

                }
                break;

                default:
                    break;
                }
            }

        }));


    return S_OK;
}


void        Client::UIObj_CustomizeSelector::After_ApplyData()
{
    //타입에따라서 customtype캐싱
    m_eCustomizeType = Get_CustomType(m_UIType);


    //메뉴 중 몇번쨰 selector인지
    m_iIdx = (int)m_fValue;

    //호버할 시 보일 텍스트
    m_HoverText = m_UIDataStr;


    //호버애니메이션 제어할것들
    m_pHover_Light = Get_Child(L"Hover_Light");
    m_pHover_Line = Get_Child(L"Hover_Line");

    if (m_pHover_Light)
        m_pHover_Light->Set_Alpha(0.f);

    if (m_pHover_Line)
        m_pHover_Line->Set_Alpha(0.f);

}


void Client::UIObj_CustomizeSelector::OnHoverEnter()
{
    //경우에상관없이 호버하는애도 저장하기
    m_pCustomizingManager->Set_HoverTargetField(this,m_iIdx);
    //cusominzing manager한테 focus된 인덱스 알리기
    m_pCustomizingManager->Set_FocusIdx(m_iIdx);


    //메뉴만 선택되었을때도 호버가안되넴..
    CHECK_TRUE(m_pCustomizingManager->Get_UIState() == CustomizingManager::CustomMizingstate::READY);

    CHECK_TRUE(m_pCustomizingManager->Get_UIState() == CustomizingManager::CustomMizingstate::ACTIVE_PICKER);

    __super::OnHoverEnter();

   


    //호버이벤트(연출)
    if (m_pHover_Light)
        m_pHover_Light->Set_Alpha(1.f);

    if (m_pHover_Line)
    {
        m_pHover_Line->Set_Alpha(0.5f);
        m_pHover_Line->Play_Animation("OnHover");

    }

    //TEXT보내기
    UIObj_CustomizeGrid::CustomizeGridUIEvent UIEvent;
    UIEvent.eventType = UIObj_CustomizeGrid::CustomizeGridEventType::UPDATE_HOVERTEXT;
    UIEvent.eTargetCategory = m_eCustomizeType;
    UIEvent.pArg = &m_HoverText;
    m_pGameInstance->Publish(UIEvent);


    m_pCustomizingManager->Hover_CustomizeSelector(this);

}


void Client::UIObj_CustomizeSelector::OnHoverExit()
{
    CHECK_TRUE(m_pCustomizingManager->Get_UIState() == CustomizingManager::CustomMizingstate::ACTIVE_PICKER);

    __super::OnHoverExit();

    //호버이벤트
    if (m_pHover_Light)
        m_pHover_Light->Set_Alpha(0.f);

    if (m_pHover_Line)
    {
        m_pHover_Line->Set_Alpha(0.f);
        m_pHover_Line->Stop_Animation();

    }

}

void Client::UIObj_CustomizeSelector::OnClick()
{

    CHECK_TRUE(m_pCustomizingManager->Get_UIState() == CustomizingManager::CustomMizingstate::ACTIVE_PICKER);


    __super::OnClick();
    

    //창을 연다.(customizemanger에게 요청)
    //TEXT보내기
    UIObj_CustomizeGrid::CustomizeGridUIEvent UIEvent;
    UIEvent.eventType = UIObj_CustomizeGrid::CustomizeGridEventType::UPDATE_HOVERTEXT;
    UIEvent.eTargetCategory = m_eCustomizeType;
    UIEvent.pArg = &m_HoverText;
    m_pGameInstance->Publish(UIEvent);


    m_pCustomizingManager->Request_Open_ActivePicker(m_eCustomizeType,m_eCustomValueType, this);

    PlayClickSound();


    
}

void Client::UIObj_CustomizeSelector::OnHoverEnter_Force()
{
    __super::OnHoverEnter();

    //cusominzing manager한테 focus된 인덱스 알리기
    m_pCustomizingManager->Set_FocusIdx(m_iIdx);


    //호버이벤트(연출)
    if (m_pHover_Light)
    {
        m_pHover_Light->Set_Alpha(1.f);

        m_pHover_Light->Set_Visible(true);
        m_pHover_Light->Set_Active(true);

    }
    if (m_pHover_Line)
    {
        m_pHover_Line->Set_Visible(true);
        m_pHover_Line->Set_Active(true);

        m_pHover_Line->Set_Alpha(0.5f);
        m_pHover_Line->Set_CombinedAlpha(0.5f);
        m_pHover_Line->Play_Animation("OnHover");

    }

    //TEXT보내기
    UIObj_CustomizeGrid::CustomizeGridUIEvent UIEvent;
    UIEvent.eventType = UIObj_CustomizeGrid::CustomizeGridEventType::UPDATE_HOVERTEXT;
    UIEvent.eTargetCategory = m_eCustomizeType;
    UIEvent.pArg = &m_HoverText;
    m_pGameInstance->Publish(UIEvent);


    m_pCustomizingManager->Hover_CustomizeSelector(this);
}

void Client::UIObj_CustomizeSelector::Update_Display(void* pArg)
{
    //색상이면, 미리보기 이미지 컬러를바꾼다
    
    //텍스처나 메시 선택이면 미리보기테스처를 바꾼다

    //알파나 다른값들이면 progress를바꾼다

    CHECK_JUST_NULL(m_pDisplay_ImgComp);

    switch (m_eCustomValueType)
    {
    case CUSTOMIZING_VALUE_TYPE::COLOR:
    {
        _float4* vColor = static_cast<_float4*>(pArg);
        m_pDisplay_ImgComp->Set_Color(*vColor);
    }
        break;
    case CUSTOMIZING_VALUE_TYPE::TEXTURE:
    case CUSTOMIZING_VALUE_TYPE::MESH:
    {
        _uint* iTex = static_cast<_uint*>(pArg);
        m_pDisplay_ImgComp->Change_Texture(*iTex);

    }
        break;
    case CUSTOMIZING_VALUE_TYPE::END:
        break;
    default:
        break;
    }
}

void Client::UIObj_CustomizeSelector::Set_Active(_bool _isActive)
{


    __super::Set_Active(_isActive);
    if (_isActive)
    {
        if (m_pHover_Light)
            m_pHover_Light->Set_Alpha(0.f);

        if (m_pHover_Line)
            m_pHover_Line->Set_Alpha(0.f);


    }
}



void Client::UIObj_CustomizeSelector::Free()
{
    __super::Free();
}


