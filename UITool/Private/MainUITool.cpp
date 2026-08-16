#include "UITool_Define.h"
#include "MainUITool.h"
#include "GameInstance.h"
#include "ImguiManager.h"

#pragma region 임구이 윈도우

#include "LayerDebugWindow.h"
#include "InspectorUIWindow.h"
#include "FunctionDebugWindow.h"
#include "UIAnimationEditorWindow.h"


#pragma endregion
#include "UIToolLoader.h"
#include "Level_UIToolLoad.h"

#include "UITool_Macro_Component.h"

#include "Level_UIToolLoad.h"
#include "Mouse.h"
#include "Sample_Pars_UItool.h"
#include "CopyManager.h"






MainUITool::MainUITool()
{
}



HRESULT UITool::MainUITool::Initialize()
{
    srand((unsigned)time(NULL));

    g_vHDRColor = _float3(0.f,0.f,1.f);

    g_bDrawDebugCollider = true;
    g_toolType = TOOLTYPE::UI_TOOL;

    g_fFPSRate = FPS_144;
    g_bIMGUI_UIOn = true;

    ENGINE_DESC tdesc{};

    tdesc.iNumLevels = _uint(LEVEL::END);
    tdesc.hWnd = g_hWnd;
    tdesc.hInstance = g_hInstance;

    m_pGameInstance = GameInstance::GetInstance();
    m_pGameInstance->Initialize_Device(g_hWnd, WINMODE::WIN, WINCX, WINCY, &m_pDevice, &m_pContext);
    m_pGameInstance->Initialize(tdesc);
    Safe_AddRef(m_pGameInstance);

    m_pImGuiManager = ImguiManager::GetInstance();
    m_pImGuiManager->Init(g_hWnd, m_pDevice, m_pContext);
    m_pCopyManager = CopyManager::GetInstance();
    m_pCopyManager->Initialize();

    
    Create_Windows();

    CHECK_FAILED(Ready_Static(), E_FAIL);

    CHECK_FAILED(Start_Level(), E_FAIL);

  

    return S_OK;
}

HRESULT MainUITool::Create_Windows()
{
    {
        ImguiWindow::IMGUIWINDOW_DESC LayerDebugDesc;
        LayerDebugDesc.m_WindowTitle = "HierarchyWindow";

        LayerDebugWindow* pTestDebugWindow = LayerDebugWindow::Create(m_pDevice, m_pContext, &LayerDebugDesc);
        if (!pTestDebugWindow)
            return E_FAIL;

        //ImguiManager에게 추가한다.
        m_pImGuiManager->RegisterWindow(pTestDebugWindow);
    }

    {
        ImguiWindow::IMGUIWINDOW_DESC InspectorDebugDesc;
        InspectorDebugDesc.m_WindowTitle = "Inspector";

        InspectorUIWindow* pInspectorDebugWindow = InspectorUIWindow::Create(m_pDevice, m_pContext, &InspectorDebugDesc);
        if (!pInspectorDebugWindow)
            return E_FAIL;

        //ImguiManager에게 추가한다.
        m_pImGuiManager->RegisterWindow(pInspectorDebugWindow);
    }
    
    {
        FunctionDebugWindow::IMGUIWINDOW_DESC FunctionDebugDesc;
        FunctionDebugDesc.m_WindowTitle = "FunctionDebug";
        
        FunctionDebugWindow* pFunctionDebugWindow = FunctionDebugWindow::Create(m_pDevice, m_pContext, &FunctionDebugDesc);
        if (!pFunctionDebugWindow)
            return E_FAIL;

        m_pImGuiManager->RegisterWindow(pFunctionDebugWindow);
    }

    {
        UIAnimationEditorWindow::IMGUIWINDOW_DESC UIAnimationEditorWindowDesc;
        UIAnimationEditorWindowDesc.m_WindowTitle = "AnimationClipEditor";

        UIAnimationEditorWindow* pUIAnimationEditorWindow = UIAnimationEditorWindow::Create(m_pDevice, m_pContext, &UIAnimationEditorWindowDesc);
        if (!pUIAnimationEditorWindow)
            return E_FAIL;

        m_pImGuiManager->RegisterWindow(pUIAnimationEditorWindow);
    }
    return S_OK;
}


void MainUITool::Update(const _float fTimeDelta)
{
    m_pMouse->Update_Priority(fTimeDelta);
    m_pMouse->Update(fTimeDelta);
    
    if (m_pGameInstance->KeyPress(DIK_LCONTROL) && m_pGameInstance->KeyDown(DIK_D))
    {
        //Clear sElectObject;
        m_pGameInstance->Set_SelectObject(nullptr);
        m_pImGuiManager->Reset_All_Window();
    }

    m_pGameInstance->Update_Level(fTimeDelta);
    m_pGameInstance->Update_Cameras(fTimeDelta);


    m_pMouse->Update_Late(fTimeDelta);
    m_pCopyManager->Update_Late(fTimeDelta);

    m_pImGuiManager->Update_Priority(fTimeDelta);
    m_pImGuiManager->Update(fTimeDelta);

}

void MainUITool::Render(const _float fTimeDelta)
{
    static _float4 clearcolor = _float4(0.7f, 0.6f, 1.f, 1.f);

    m_pGameInstance->Draw_Begin(&clearcolor);

    m_pGameInstance->Render_Grid();
    m_pGameInstance->Draw(fTimeDelta);

    m_pImGuiManager->Render();

    m_pGameInstance->Draw_Text_Begin();
    m_pGameInstance->Render_Texts_Stacked();
    m_pGameInstance->Draw_Text_End();


    m_pGameInstance->Draw_End();
}


MainUITool* UITool::MainUITool::Create()
{
    MainUITool* pInstance = new MainUITool;

    MSG_FAIL(pInstance->Initialize(), L"MainUITool 생성에 실패했습니다!", L"경고!!!", nullptr);

    return pInstance;
}


HRESULT UITool::MainUITool::Ready_Static()
{
    m_pLoader = UIToolLoader::Create(m_pDevice, m_pContext, LEVEL::STATIC);

    //CHECK_FAILED(m_pGameInstance->Load_Font(Font_Default, L"../../Resources/Font/NanumMyeongjo.spritefont"), E_FAIL);

    SHADERENTRY postexentry[] = {
    {"VS_MAIN", "PS_MAIN"},
    {"VS_MAIN", "PS_MAIN_COMMNET"},
    {"VS_MAIN", "PS_MAIN_MASKING"},
    {"VS_MAIN", "PS_MAIN_COLOR_MUL"},
    {"VS_MAIN", "PS_NO_DISCARD" },

    {"VS_MAIN", "PS_PROGRESS_HORIZONTAL" },
    {"VS_MAIN", "PS_PROGRESS_HORIZONTAL_RE" },
    {"VS_MAIN", "PS_PROGRESS_VERTICAL" },
    {"VS_MAIN", "PS_PROGRESS_VERTICAL_RE" },
    {"VS_MAIN", "PS_PROGRESS_RADIAL" },
    {"VS_MAIN","PS_PROGRESS_CENTER"},
    {"VS_MAIN","PS_NOISE"},
    {"VS_MAIN","PS_SMOKE_TEST"},
    {"VS_MAIN","PS_SPRITE"},
    {"VS_MAIN","PS_HPBarInner"},
    {"VS_MAIN","PS_WorldDissolve"},
    {"VS_MAIN","PS_BlurUI"},
    {"VS_MAIN","PS_MinimapUI"},
     {"VS_MAIN","PS_CENTERED_MINIMAP"},
     {"VS_MAIN","PS_Distortion"},
     {"VS_MAIN","PS_WorldUI_Blur"},
     {"VS_MAIN","PS_TEXT_TEST"},
     {"VS_MAIN","PS_TITLE"},
      {"VS_MAIN","PS_LIGHT"}


    };
    SHADERENTRIES entries;
    entries.pEntries = postexentry;
    entries.iNumpass = sizeof(postexentry) / sizeof(SHADERENTRY);
    /* For.Prototype_Component_Shader_VTXPosTex */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Com_Shader_VTXPosTex,
        Shader::Create(m_pDevice, m_pContext, L"../../Shader/Shader_VtxPosTex.hlsl", VTXPOSTEX::Elements, VTXPOSTEX::iNumElements, entries)), E_FAIL);


    ////////////////////
    SHADERENTRY poscolorentry[2] = {
    {"VS_MAIN", "PS_MAIN"},
     {"VS_MAIN", "PS_SELECT"}
    };
    SHADERENTRIES poscolorentries;
    poscolorentries.pEntries = poscolorentry;
    poscolorentries.iNumpass = 2;
    /* For.Prototype_Component_Shader_VTXPoscolor */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Com_Shader_VTXPosColor,
        Shader::Create(m_pDevice, m_pContext, L"../../Shader/Shader_VtxPosColor.hlsl", VTXPOSCOR::Elements, VTXPOSCOR::iNumElements, poscolorentries)), E_FAIL);



    ////////////Font Shader///////////
    //SHADERENTRY Fontentry[] = {
    //{"VS_MAIN", "PS_MAIN"}

    //};
    //SHADERENTRIES Fontentries;
    //Fontentries.pEntries = Fontentry;
    //Fontentries.iNumpass =1;
    ///* For.Shader_Font */
    //CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Com_Shader_Font,
    //    Shader::Create(m_pDevice, m_pContext, L"../../Shader/Shader_Font.hlsl", FONTTEX::Elements, FONTTEX::iNumElements, Fontentries)), E_FAIL);

    //Base* pOutLineShader = m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, _UINT(LEVEL::STATIC),
    //    Proto_Com_Shader_Font);

    //if(pOutLineShader)
    //    m_pGameInstance->Set_OutlineShader(dynamic_cast<Shader*>(pOutLineShader));

    //Safe_Release(pOutLineShader);
    

    /* For.Prototype_Component_Texture_BitmapFont */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Texture(L"BitmapFont"),
        NewTexture::Create(m_pDevice, m_pContext, L"../../Resources/Font/Bitmapfont/")), E_FAIL);


    CHECK_FAILED(m_pGameInstance->Load_Font(Font_SquadaOne, L"../../Resources/Font/SquadaOne-Regular.spritefont"), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Load_Font(Font_Rajdhani_Bold, L"../../Resources/Font/Rajdhani Bold.spritefont"), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Load_Font(Font_Rajdhani_Light, L"../../Resources/Font/Rajdhani-Light.spritefont"), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Load_Font(Font_Rajdhani_Medium, L"../../Resources/Font/Rajdhani-Medium.spritefont"), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Load_Font(Font_Rajdhani_SemiBold, L"../../Resources/Font/Rajdhani-SemiBold.spritefont"), E_FAIL);


    CHECK_FAILED(m_pGameInstance->Load_Font(Font_Yoon310, L"../../Resources/Font/Yoon310.spritefont"), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Load_Font(Font_Yoon320, L"../../Resources/Font/Yoon320.spritefont"), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Load_Font(Font_Yoon330, L"../../Resources/Font/Yoon330.spritefont"), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Load_Font(Font_A2Z_5_Medium, L"../../Resources/Font/A2Z5Medium.spritefont"), E_FAIL);
     
    CHECK_FAILED(m_pGameInstance->Load_Font(Font_Bookk_Bold, L"../../Resources/Font/Bookk-Bold.spritefont"), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Load_Font(Font_Bookk_Bold_Eng, L"../../Resources/Font/BokkMyungjo_Eng.spritefont"), E_FAIL);

    
    CHECK_FAILED(m_pGameInstance->Load_Font(Font_Shine_Bold, L"../../Resources/Font/Shine_Bold.spritefont"), E_FAIL);

    //Bitmapfont
   CHECK_FAILED(m_pGameInstance->Add_BitmapFont(BitmapFont_Rajdhani_Medium, L"../../Resources/Font/BitmapFont/Rajdhani/Rajdhani.fnt", "Rajdhani/Rajdhani_0"), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_BitmapFont(BitmapFont_Rajdhani_LightGray, L"../../Resources/Font/BitmapFont/Rajdhani_LightGrayOutline/Rajdhani_LightGrayOutline.fnt", "Rajdhani_LightGrayOutline/Rajdhani_LightGrayOutline"), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Add_BitmapFont(BitmapFont_Squada_One, L"../../Resources/Font/BitmapFont/Squada One/Squada One.fnt", "Squada One/Squada One"), E_FAIL);



    m_pMouse = Mouse::GetInstance();
    m_pMouse->Initialize(m_pDevice, m_pContext);

    return S_OK;
}

HRESULT UITool::MainUITool::Start_Level()
{
    m_pGameInstance->Add_Level(CAST(_uint)(LEVEL::LOADING), Level_UIToolLoad::Create(m_pDevice, m_pContext, LEVEL::MAIN));
    m_pGameInstance->Change_Level(CAST(_uint)(LEVEL::LOADING));


    return S_OK;
}

void MainUITool::Free()
{
    
    __super::Free();
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pLoader);
    Safe_Release(m_pMouse);

    m_pCopyManager->DestroyInstance();
    m_pImGuiManager->DestroyInstance();
    m_pGameInstance->Release_Engine();
    Safe_Release(m_pGameInstance);


}
 