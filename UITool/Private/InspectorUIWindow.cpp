#include "UITool_Define.h"
#include "InspectorUIWindow.h"
#include "GameInstance.h"
#include "UIObject.h"
#include "UITool_Macro_Component.h"
#include "Dummy.h"




InspectorUIWindow::InspectorUIWindow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :ImguiWindow(pDevice, pContext),
    pGameInstance(GameInstance::GetInstance())
{

}



HRESULT UITool::InspectorUIWindow::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_ComponentVec.reserve(10);
    m_ComponentProtoVec.reserve(10);
    m_ComponentVec.push_back("None");
    m_ComponentVec.push_back("ButtonComponent");
    m_ComponentVec.push_back("PrgoressComponent");
    m_ComponentVec.push_back("AnimationComponent");
    m_ComponentVec.push_back("UISpriteComponent");
    m_ComponentVec.push_back("UIEventReactorComponent");
    m_ComponentVec.push_back("UIImageComponent");
    m_ComponentVec.push_back("UIWorldComponent");

    m_ComponentProtoVec.push_back(L"");
    m_ComponentProtoVec.push_back(Proto_UIButton);
    m_ComponentProtoVec.push_back(Proto_UIProgress);
    m_ComponentProtoVec.push_back(Proto_UIAnimation);
    m_ComponentProtoVec.push_back(Proto_UISprite);
    m_ComponentProtoVec.push_back(Proto_UIEventReactor);
    m_ComponentProtoVec.push_back(Proto_UIImage);
    m_ComponentProtoVec.push_back(Proto_UIWorld);

    ////타입채우기

    m_Basekey = Get_UIBaseStr();
    m_Chars = Get_CategoryStr();


    return S_OK;
   
}

_uint UITool::InspectorUIWindow::Update_Contents(_float fTimeDelta)
{
    m_pSelectObject = pGameInstance->Get_UISelectObject();

    
	if (m_pSelectObject)
		m_pSelectObject->Render_IMGUI();


    ImGui::Separator();
    
    ImGui::Spacing();
    ImGui::Spacing();

    if (ImGui::Button("Add Component"))
	{
      
        m_bClicked = true;
		
        
        

    }

    if (m_bClicked)
    {
        int i = 0;
        if (ImGui::Combo("ComponentList", &i, m_ComponentVec.data(), m_ComponentVec.size()))
        {

            Component* pOut = nullptr;
            if (i == 1)     //BUTTON COMPONENT
            {
                UI_Button::BUTTON_DESC Desc;
                Desc.pOwner = m_pSelectObject;
                Desc.ClickTexKey = DEFAULT_TEXTUREKEY;
                Desc.HoverTexKey = DEFAULT_TEXTUREKEY;
                Desc.ClickTexKey= DEFAULT_TEXTUREKEY;
                Desc.DisableKey= DEFAULT_TEXTUREKEY;
                m_pSelectObject->Add_NewUIComponent(UITYPE::BUTTON, 0, m_ComponentProtoVec[i], m_ComponentProtoVec[i], &Desc);
            }

            else if (i ==2)     //BUTTON COMPONENT
            {
                UI_Progress::UIProgressDesc Desc;
                Desc.pOwner = m_pSelectObject;

                m_pSelectObject->Add_NewUIComponent(UITYPE::PROGRESSBAR, 0, m_ComponentProtoVec[i], m_ComponentProtoVec[i], &Desc);
            }

            else if (i == 3)     //ANIMATION COMPONENT
            {
                UI_Animation::UICOMPDESC Desc;
                Desc.pOwner = m_pSelectObject;

                m_pSelectObject->Add_NewUIComponent(UITYPE::ANIMATION, 0, m_ComponentProtoVec[i], m_ComponentProtoVec[i], &Desc);
            }

            else if (i == 4)     //SPRITE COMPONENT
            {
                UI_Sprite::tagSpriteCompDesc Desc;
                Desc.pOwner = m_pSelectObject;

                m_pSelectObject->Add_NewUIComponent(UITYPE::SPRITE, 0, m_ComponentProtoVec[i], m_ComponentProtoVec[i], &Desc);
            }

            else if (i ==5)     //eVENT REACTOR
            {
                UI_EventReactor::UICOMPDESC Desc;
                Desc.pOwner = m_pSelectObject;

                m_pSelectObject->Add_NewUIComponent(UITYPE::EVENTREACTOR, 0, m_ComponentProtoVec[i], m_ComponentProtoVec[i], &Desc);
            }

            else if (i == 6)     //UIImage
            {
                UI_Image::UIIMAGEDESC Desc;
                Desc.pOwner = m_pSelectObject;

                Base* pComp = pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, 0, Proto_UIImage, &Desc);
                UIComponent* pUIComp = dynamic_cast<UIComponent*>(pComp);


                m_pSelectObject->Add_NewRenderComponent(Proto_UIImage, pUIComp);

            }

            else if (i == 7)     //UIWorld
            {
                UI_WorldComponent::WorldUICompDesc Desc;
                Desc.pOwner = m_pSelectObject;
                Desc.TargetCameraIdx = _UINT(CAMERA::FREE);
                Desc.bDistanceScaling = false;

                Base* pComp = pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, 0, Proto_UIWorld, &Desc);
                UIComponent* pUIComp = dynamic_cast<UIComponent*>(pComp);

                m_pSelectObject->Add_NewUIComponent(UITYPE::WORLDUI, 0, m_ComponentProtoVec[i], m_ComponentProtoVec[i], &Desc);

                if (m_pSelectObject)
                {
                    _float2 Scale = _float2(m_pSelectObject->Get_LocalTransform().m_fSizeX, m_pSelectObject->Get_LocalTransform().m_fSizeY);
                    m_pSelectObject->Set_Size(Scale.x * 0.01f, Scale.y * 0.01f);

                }

            }

            m_bClicked = false;
        }

    }

    //BaseKey와 구체화할 클래스선택
    if (m_pSelectObject)
    {
        int BaseSelect = Get_BaseUITypeIdx(m_pSelectObject->Get_BaseType());
        if (ImGui::Combo("BaseKey", &BaseSelect, m_Basekey.data(), m_Basekey.size()))
        {

            m_pSelectObject->Set_BaseType(m_Basekey[BaseSelect]);
        }

        int UITypeSelect = Get_UITypeIdx(m_pSelectObject->Get_UIType());
        if (ImGui::Combo("TypeKey", &UITypeSelect, m_Chars.data(), m_Chars.size()))
        {

            m_pSelectObject->Set_UIType(m_Chars[UITypeSelect]);
        }

    }
    

    /////////////////////////////
    int i = 0;


    //if (ImGui::Combo("ComponentList", &i, m_ComponentVec.data(), m_ComponentVec.size()))
    //{

    //    Component* pOut = nullptr;

    //    UIComponent::UICOMPDESC Desc;
    //    Desc.pOwner = m_pSelectObject;

    //    m_pSelectObject->Add_NewUIComponent(UITYPE(i + 2), 0, m_ComponentProtoVec[i], m_ComponentProtoVec[i], &Desc);
    //    m_bClicked = false;
    //}

    return 0;
}

_int UITool::InspectorUIWindow::Reset()
{
    m_pSelectObject = nullptr;
    return 0;
}


InspectorUIWindow* UITool::InspectorUIWindow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    InspectorUIWindow* pInstance = new InspectorUIWindow(pDevice, pContext);


    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Create : InspectorUIWindow");
        Safe_Release(pInstance);
    }


    return pInstance;
}



void UITool::InspectorUIWindow::Free()
{
	Safe_Release(pGameInstance);
	__super::Free();
}
