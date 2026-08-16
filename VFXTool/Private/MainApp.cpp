#include "VFXTool_Define.h"
#include "MainApp.h"
#include "GameInstance.h"

#include "Loader.h"
#include "Level_Load.h"

#include "ImguiManager.h"
#include "Mouse.h"
#include "Window_VFXEditor.h"

MainApp::MainApp()
{
}


HRESULT VFXTool::MainApp::Initialize()
{
    ENGINE_DESC EngineDesc{};

    EngineDesc.iNumLevels = _uint(LEVEL::END);
    EngineDesc.hWnd = g_hWnd;
    EngineDesc.hInstance = g_hInstance;

    g_vHDRColor = _float3(0.22f, 0.22f, 0.22f);

    m_pGameInstance = GameInstance::GetInstance();
    m_pGameInstance->Initialize_Device(g_hWnd, WINMODE::WIN, WINCX, WINCY, &m_pDevice, &m_pContext);
    m_pGameInstance->Initialize(EngineDesc);
    Safe_AddRef(m_pGameInstance);

    m_pImGuiManager = ImguiManager::GetInstance();
    m_pImGuiManager->Init(g_hWnd, m_pDevice, m_pContext);
    
    Ready_Static();
    Start_Level();
    Create_Windows();

    g_bDrawRTV = false;
    g_fFPSRate = FPS_144;

    return S_OK;
}

HRESULT MainApp::Create_Windows()
{
    ImguiWindow::IMGUIWINDOW_DESC VFXEditorDesc{};
    VFXEditorDesc.m_WindowTitle = "VFX_Editor";

    Window_VFXEditor* pVFXEditor = Window_VFXEditor::Create(m_pDevice, m_pContext, &VFXEditorDesc);
    if (pVFXEditor == nullptr)
        return E_FAIL;

    // ImguiManager에 추가
    m_pImGuiManager->RegisterWindow(pVFXEditor);

    return S_OK;
}

HRESULT VFXTool::MainApp::Ready_Static()
{
    m_pLoader = Loader::Create(m_pDevice, m_pContext, LEVEL::STATIC);

    CHECK_FAILED(m_pGameInstance->Load_Font(L"Font_Default", L"../../Resources/Font/Terraria20.spritefont"), E_FAIL);
    CHECK_FAILED(m_pGameInstance->Load_Font(L"Font_M10", L"../../Resources/Font/Gyungi_M10.spritefont"), E_FAIL);

    SHADERENTRY postexentry[5] = {
    {"VS_MAIN", "PS_MAIN"},
    {"VS_MAIN", "PS_MAIN_COMMNET"},
    {"VS_MAIN", "PS_MAIN_MASKING"},
    {"VS_MAIN", "PS_MAIN_COLOR_MUL"},
    {"VS_MAIN", "PS_NO_DISCARD" }
    };

    SHADERENTRIES entries;
    entries.pEntries = postexentry;
    entries.iNumpass = 5;
    /* For.Prototype_Component_Shader_VTXPosTex */
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), L"Prototype_Component_Shader_VTXPosTex",
        Shader::Create(m_pDevice, m_pContext, L"../Shader/Shader_VtxPosTex.hlsl", VTXPOSTEX::Elements, VTXPOSTEX::iNumElements, entries)), E_FAIL);

    return S_OK;
}

HRESULT VFXTool::MainApp::Start_Level()
{
    m_pGameInstance->Add_Level(CAST(_uint)(LEVEL::LOADING), Level_Load::Create(m_pDevice, m_pContext, LEVEL::MAIN));
    m_pGameInstance->Change_Level(CAST(_uint)(LEVEL::LOADING));

    return S_OK;
}


void MainApp::Update(const _float fTimeDelta)
{
    Show_FPS(fTimeDelta);

    // 렌더타겟 켜고 끄기
    if (m_pGameInstance->KeyDown(DIK_F10))
    {
        g_bDrawRTV = !g_bDrawRTV;
    }

    if (m_pLoader != nullptr && m_pLoader->Is_LoadComplete())
        Safe_Release(m_pLoader);

    m_pGameInstance->Update_Level(fTimeDelta);
    m_pImGuiManager->Update_Priority(fTimeDelta);
    m_pImGuiManager->Update(fTimeDelta);
}

void MainApp::Render(const _float fTimeDelta)
{
    static _float4 clearcolor = _float4(1.f, 1.f, 1.f, 1.f);

    m_pGameInstance->Draw_Begin(&clearcolor);

    m_pGameInstance->Draw(fTimeDelta);

    m_pImGuiManager->Render();

    m_pGameInstance->Draw_Text_Begin();
    m_pGameInstance->Render_Texts_Stacked();
    m_pGameInstance->Draw_Text_End();

    m_pGameInstance->Draw_End();
}

void VFXTool::MainApp::Show_FPS(const _float fTimeDelta)
{
    ++m_iCurrentFPS;
    static _float Timer = {};
    Timer += fTimeDelta;

    if (Timer >= 1.f)
    {
        m_iFPS = m_iCurrentFPS;

        m_wstrFPS = L"FPS : " + to_wstring(m_iFPS);

        m_iCurrentFPS = 0;
        Timer = 0.f;
    }

    //m_pGameInstance->Add_Text_Stack({ .eSetting{TEXTDRAW_SETTING::NORMAL}, .vPosition{100.f, 0.f}, .vSize{1.f, 1.f}, .wstrDrawText{m_wstrFPS}, .wstrFontName{Font_Default} });
    //m_pGameInstance->Add_Text_Stack({ .eSetting{TEXTDRAW_SETTING::DIR4}, .vPosition{100.f, 50.f}, .vSize{1.f, 1.f}, .wstrDrawText{m_wstrFPS}, .wstrFontName{Font_Default} });
    m_pGameInstance->Add_Text_Stack({ .eSetting{TEXTDRAW_SETTING::DIR8}, .vSize{1.f, 1.f}, .wstrDrawText{m_wstrFPS}, .wstrFontName{L"Font_Default"} });
}

MainApp* VFXTool::MainApp::Create()
{
    MainApp* pInstance = new MainApp;

    MSG_FAIL(pInstance->Initialize(), L"MainApp 생성에 실패했습니다!", L"경고!!!", nullptr);

    return pInstance;
}

void MainApp::Free()
{    
    __super::Free();
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pLoader);

    m_pImGuiManager->DestroyInstance();
    Mouse::DestroyInstance();
    m_pGameInstance->Release_Engine();
    Safe_Release(m_pGameInstance);
}
