#include "UITool_Define.h"
#include "LayerDebugWindow.h"
#include "GameInstance.h"
#include "UIObject.h"
#include "Layer.h"
#include "UITool_Macro.h"
#include "UITool_Macro_Component.h"
#include "UIFactory.h"
#include "FunctionDebugWindow.h"
#include "ImguiManager.h"




LayerDebugWindow::LayerDebugWindow(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :ImguiWindow(pDevice,pContext),
    pGameInstance(GameInstance::GetInstance())
{
    Safe_AddRef(pGameInstance);
}

_uint UITool::LayerDebugWindow::Update_Contents(_float fTimeDelta)
{
    pGameInstance->Render_Search();

    if (ImGui::BeginChild("Hierarchy_List"))
    {
        pGameInstance->Render_UIHierarchy();

        /////////오른쪽메뉴
        if (ImGui::BeginPopupContextWindow("EmptySpaceContext", ImGuiMouseButton_Right))
        {
            if (ImGui::BeginMenu("UI"))
            {
                //ui만들기 위한 메뉴 보여줌(ui 종류)
                if (ImGui::BeginMenu("Prefabs"))
                {
                    Show_PrefabMenu();
                    ImGui::EndMenu();
                }

                if (ImGui::BeginMenu("Objects"))
                {
                    Show_CreateUIMenu();
                    ImGui::EndMenu();
                }

                ImGui::EndMenu();
            }

            ImGui::EndPopup();
        }

        ImGui::EndChild(); // Hierarchy_List 종료
    }

    //삭제요청
    UIObject* pUI = pGameInstance->Get_UISelectObject();
    if (pUI)
    {
        if (pGameInstance->KeyDown(DIK_DELETE))
        {
            if (pUI->Get_Parent() == nullptr)
                pUI->Set_Dead(true);

            else
            {
                if (pGameInstance->Get_UISelectObject() == pUI)
                {
                    pGameInstance->Set_SelectObject(nullptr);
                    ImguiManager::GetInstance()->Reset_All_Window();
                }

                pUI->Detach_Me_From_Parent();
            }

        }

        return 0;
    }
}

void LayerDebugWindow::Show_CreateUIMenu()
{
    if (ImGui::MenuItem("Image"))
    {
        Create_UIObject(UITYPE::IMAGE);
    }

    if (ImGui::MenuItem("Button"))
    {
        Create_UIObject(UITYPE::BUTTON);
    }

    if (ImGui::MenuItem("Progress"))
    {
        Create_UIObject(UITYPE::PROGRESSBAR,5);
    }

    if (ImGui::MenuItem("Animation"))
    {
        Create_UIObject(UITYPE::ANIMATION);
    }

    if (ImGui::MenuItem("Sprite"))
    {
        Create_UIObject(UITYPE::SPRITE, 13);
    }

    if (ImGui::MenuItem("Text"))
    {
        Create_UIObject(UITYPE::TEXT);
    }

  

    if (ImGui::MenuItem("BitmapText"))
    {
        Create_UIObject(UITYPE::BITMAPTEXT);
    }



    if (ImGui::MenuItem("WorldUI"))
    {
        Create_UIObject(UITYPE::WORLDUI);
    }

    if (ImGui::MenuItem("Slotgrid"))
    {
        Create_UIObject(UITYPE::SLOTGRID);
    }

    if (ImGui::MenuItem("Empty"))
    {
        Create_UIObject(UITYPE::ROOT);
    }




}

void LayerDebugWindow::Show_PrefabMenu()
{

    if (ImGui::MenuItem("HpBar"))
    {
        Create_PrefabType(UI_PREFAB::HPBAR);
    }
}

HRESULT LayerDebugWindow::Create_UIObject(UITYPE eUIType, _uint passNum)
{
    _wstring layer = Layer_UIs;


    _uint CurrentLevel = pGameInstance->Get_Current_LevelID();

    GameObject* pOut = nullptr;
    UIObject* pNewUI = nullptr;

    UIObject::UIOBJECT_DESC UIDesc;
    UIDesc.fCX = 100.f;
    UIDesc.fCY = 100.f;
    UIDesc.fX = WINCX / 2.f;
    UIDesc.fY = WINCY / 2.f;
    UIDesc.iShaderNumber = passNum;


    
    if (FAILED(pGameInstance->Add_GameObject_To_Layer(0, L"Prototype_UIObject", CurrentLevel, layer, &pOut, &UIDesc)))
        return E_FAIL;


    if (pOut)
        pNewUI = dynamic_cast<UIObject*>(pOut);

   /*타입에 따라 다르게 component붙이기*/
    switch (eUIType)
    {
    case UITYPE::ROOT:
    {
        UI_Render::UICOMPDESC Desc;
        Desc.pOwner = pNewUI;
     
        if (FAILED(pNewUI->Add_NewRenderComponent(0, Proto_UIRender, Proto_UIRender, &Desc)))
            return E_FAIL;


    }

        break;
    case UITYPE::IMAGE:
    {
        UI_Image::UIIMAGEDESC Desc;
        Desc.pOwner = pNewUI;
        
        if(FAILED(pNewUI->Add_NewRenderComponent(0, Proto_UIImage, Proto_UIImage, &Desc)))
            return E_FAIL;

     
     
    }
        break;
    case UITYPE::BUTTON:
    {
        //Image+Button

        UI_Image::UIIMAGEDESC Desc;
        Desc.pOwner = pNewUI;

        
        if (FAILED(pNewUI->Add_NewRenderComponent(0, Proto_UIImage, Proto_UIImage, &Desc)))
            return E_FAIL;

        UI_Button::BUTTON_DESC ButtonDesc;
        ButtonDesc.pOwner = pNewUI;
        

        if (FAILED(pNewUI->Add_NewUIComponent(UITYPE::BUTTON,0, Proto_UIButton, Proto_UIButton, &ButtonDesc)))
            return E_FAIL;

    }
        break;
    case UITYPE::PROGRESSBAR:
    {
        

        UI_Image::UIIMAGEDESC Desc;
        Desc.pOwner = pNewUI;


        if (FAILED(pNewUI->Add_NewRenderComponent(0, Proto_UIImage, Proto_UIImage, &Desc)))
            return E_FAIL;

        UI_Progress::UI_PROGRESSDESC ProgressDesc;
        ProgressDesc.pOwner = pNewUI;


        if (FAILED(pNewUI->Add_NewUIComponent(UITYPE::PROGRESSBAR, 0, Proto_UIProgress, Proto_UIProgress, &ProgressDesc)))
            return E_FAIL;
    }
        break;


    case UITYPE::ANIMATION:
    {

        UI_Image::UIIMAGEDESC Desc;
        Desc.pOwner = pNewUI;


        if (FAILED(pNewUI->Add_NewRenderComponent(0, Proto_UIImage, Proto_UIImage, &Desc)))
            return E_FAIL;

        UI_Animation::tagUICompDesc UIDesc;
        UIDesc.pOwner = pNewUI;


        if (FAILED(pNewUI->Add_NewUIComponent(UITYPE::ANIMATION, 0, Proto_UIAnimation, Proto_UIAnimation, &UIDesc)))
            return E_FAIL;
    }
    break;

    case UITYPE::SPRITE:
    {
        UI_Sprite::SPRITEDESC Desc;
        Desc.pOwner = pNewUI;
      

        if (FAILED(pNewUI->Add_NewRenderComponent(0, Proto_UISprite, Proto_UISprite, &Desc)))
            return E_FAIL;

    }
        break;

    case UITYPE::TEXT:
    {

        UI_Text::TEXTDESC Desc;
        Desc.pOwner = pNewUI;
        Desc.ProtoFontName = Font_Default;


        if (FAILED(pNewUI->Add_NewRenderComponent(0, Proto_UIText, Proto_UIText, &Desc)))
            return E_FAIL;

    }
    break;
    case UITYPE::BITMAPTEXT:
    {

        UI_BitmapText::BITMAP_TEXTDESC Desc;
        Desc.pOwner = pNewUI;
        Desc.ProtoBitmapFontName = BitmapFont_Rajdhani_LightGray;


        if (FAILED(pNewUI->Add_NewRenderComponent(0, Proto_BitmapText, Proto_BitmapText, &Desc)))
            return E_FAIL;

    }

    break;


    case UITYPE::WORLDUI:
    {

        UI_WorldComponent::WorldUICompDesc Desc;
        Desc.pOwner = pNewUI;


        if (FAILED(pNewUI->Add_NewRenderComponent(0, Proto_UIWorld, Proto_UIWorld, &Desc)))
            return E_FAIL;
    }
    break;

    case UITYPE::SLOTGRID:
    {

        GRID_DESC  Desc;
        Desc.ProtoNewTexKey = Proto_UITexture(L"Customize_Hair");
        Desc.pOwner = pNewUI;


        if (FAILED(pNewUI->Add_NewRenderComponent(0, Proto_UISlotGrid, Proto_UISlotGrid, &Desc)))
            return E_FAIL;
    }
    break;

    case UITYPE::END:
        break;
    default:
        break;
    }
  
    return S_OK;

}

HRESULT LayerDebugWindow::Create_PrefabType(UI_PREFAB ePrefabType)
{
    _wstring layer = Layer_UIs;


    _uint CurrentLevel = pGameInstance->Get_Current_LevelID();


    switch (ePrefabType)
    {
    case UITool::UI_PREFAB::HPBAR:
        return UIFactory::CreateHpBar(layer, CurrentLevel);
        break;
    case UITool::UI_PREFAB::END:
        break;
    default:
        break;
    }
    return S_OK;
}

LayerDebugWindow* UITool::LayerDebugWindow::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
    LayerDebugWindow* pInstance = new LayerDebugWindow(pDevice, pContext);


    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Create : LayerDebugWindow");
        Safe_Release(pInstance);
    }


    return pInstance;
}


void UITool::LayerDebugWindow::Free()
{
    Safe_Release(pGameInstance);
    __super::Free();
}
