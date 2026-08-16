#include "ShaderTool_Define.h"
#include "MainApp.h"
#include "GameInstance.h"

#include "Loader.h"
#include "Level_Load.h"
#include "Mouse.h"
#include "ImguiManager.h"
#include "ShaderImgui.h"
#include "ImguiRTV.h"
#include "GameObject.h"
#include "Player.h"
#include "ContainerObject.h"
#include "PartObject.h"

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
MainApp::MainApp()
{
}

MainApp::~MainApp()     
{
}
//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////



//////////////////////////////////////////////////////// 객체 준비 함수 ////////////////////////////////////////////////////////
HRESULT ShaderTool::MainApp::Initialize()
{
    srand((unsigned)time(NULL));

    g_fFPSRate = FPS_144;
    g_toolType = TOOLTYPE::SHADER_TOOL;

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

    CHECK_FAILED(Ready_Static(), E_FAIL);

    CHECK_FAILED(Start_Level(), E_FAIL);

    g_bDrawDebugCollider = true;

    //Imgui창 만들기
    Create_Windows();
    Create_RTVWindows();

    return S_OK;
}

HRESULT ShaderTool::MainApp::Create_Windows()
{
    ImguiWindow::IMGUIWINDOW_DESC ShaderImguiDesc;
    ShaderImguiDesc.m_WindowTitle = "ShaderImguiWindow";

    ShaderImgui* pShaderImguiWindow = ShaderImgui::Create(m_pDevice, m_pContext, &ShaderImguiDesc);
    if (!pShaderImguiWindow)
        return E_FAIL;

    //ImguiManager에게 추가한다.
    m_pImGuiManager->RegisterWindow(pShaderImguiWindow);

    return S_OK;
}

HRESULT ShaderTool::MainApp::Create_RTVWindows()
{
    ImguiWindow::IMGUIWINDOW_DESC ShaderImguiDesc;
    ShaderImguiDesc.m_WindowTitle = "RTVImguiWindow";

    ImguiRTV* pShaderImguiWindow = ImguiRTV::Create(m_pDevice, m_pContext, &ShaderImguiDesc);
    if (!pShaderImguiWindow)
        return E_FAIL;

    //ImguiManager에게 추가한다.
    m_pImGuiManager->RegisterWindow(pShaderImguiWindow);

     return S_OK;
}

HRESULT ShaderTool::MainApp::Ready_Static()
{
    m_pLoader = Loader::Create(m_pDevice, m_pContext, LEVEL::STATIC);

    CHECK_FAILED(m_pGameInstance->Load_Font(Font_Default, L"../../Resources/Font/Terraria20.spritefont"), E_FAIL);

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
    CHECK_FAILED(m_pGameInstance->Add_Prototype(_UINT(LEVEL::STATIC), Proto_Com_Shader_VTXPosTex,
        Shader::Create(m_pDevice, m_pContext, L"../../Shader/Shader_VtxPosTex.hlsl", VTXPOSTEX::Elements, VTXPOSTEX::iNumElements, entries)), E_FAIL);

    m_pMouse = Mouse::GetInstance();
    m_pMouse->Initialize(m_pDevice, m_pContext);

    return S_OK;
}

HRESULT ShaderTool::MainApp::Start_Level()
{
    m_pGameInstance->Add_Level(CAST(_uint)(LEVEL::LOADING), Level_Load::Create(m_pDevice, m_pContext, LEVEL::LOGO));
    m_pGameInstance->Change_Level(CAST(_uint)(LEVEL::LOADING));


    return S_OK;
}
/******************************************************* 객체 준비 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
void ShaderTool::MainApp::Update(const _float fTimeDelta)
{
    Show_FPS(fTimeDelta);
    if (m_pGameInstance->KeyDown(DIK_1))
    {
        g_bDrawRTV = !g_bDrawRTV;
    }
    if (m_pGameInstance->KeyPress(DIK_LCONTROL) && m_pGameInstance->KeyDown(DIK_F10))
    {
        g_bDrawDebugCollider = !g_bDrawDebugCollider;
    }
    if (m_pLoader != nullptr && m_pLoader->Is_LoadComplete())
        Safe_Release(m_pLoader);

    m_pMouse->Update_Priority(fTimeDelta);
    m_pGameInstance->Update_Level(fTimeDelta);
    m_pMouse->Update(fTimeDelta);
    m_pMouse->Update_Late(fTimeDelta);
    m_pGameInstance->Update_Cameras(fTimeDelta);
    m_pImGuiManager->Update_Priority(fTimeDelta);
    m_pImGuiManager->Update(fTimeDelta);

}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// 업데이트 함수 ////////////////////////////////////////////////////////
void ShaderTool::MainApp::Render(const _float fTimeDelta)
{
    //여기서 플레이어 렌더할때 한번에 던져주기

        

    static _float4 clearcolor = Color(0.3f, 0.592f, 0.467f, 1.0f); //Color(0.698f, 0.592f, 0.467f, 1.0f);

    m_pGameInstance->Draw_Begin(&clearcolor);

    GameObject* tempPlayer = m_pGameInstance->Get_Player();
    if (tempPlayer)
    {
        PartObject* PartObj = static_cast<ContainerObject*>(tempPlayer)->Find_PartObject(TEXT("Part_Body"));
        if (PartObj)
        {
            PLAYER_SHADER_DESC m_vPlayerShaderDesc = static_cast<Player*>(tempPlayer)->Get_PlayerShaderDesc();
            m_pShaderCom = PartObj->Get_Shader();
            if (m_pShaderCom)
                m_pShaderCom->Bind_EntireBuffer_BySlot(3, &m_vPlayerShaderDesc, sizeof(PLAYER_SHADER_DESC));
        }
    }

    m_pGameInstance->Draw(fTimeDelta);
    
    m_pImGuiManager->Render();

    m_pGameInstance->Draw_Text_Begin();
    m_pGameInstance->Render_Texts_Stacked();
    m_pGameInstance->Draw_Text_End();

    m_pGameInstance->Draw_End();

}
/******************************************************* 업데이트 함수 *******************************************************/



//////////////////////////////////////////////////////// FPS출력 함수 ////////////////////////////////////////////////////////
void ShaderTool::MainApp::Show_FPS(const _float fTimeDelta)
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

    m_pGameInstance->Add_Text_Stack({ .eSetting{TEXTDRAW_SETTING::NORMAL}, .vPosition{100.f, 0.f}, .vSize{1.f, 1.f}, .wstrDrawText{m_wstrFPS}, .wstrFontName{Font_Default} });
    m_pGameInstance->Add_Text_Stack({ .eSetting{TEXTDRAW_SETTING::DIR4}, .vPosition{100.f, 50.f}, .vSize{1.f, 1.f}, .wstrDrawText{m_wstrFPS}, .wstrFontName{Font_Default} });
    m_pGameInstance->Add_Text_Stack({ .eSetting{TEXTDRAW_SETTING::DIR8}, .vPosition{100.f, 100.f}, .vSize{1.f, 1.f}, .wstrDrawText{m_wstrFPS}, .wstrFontName{Font_Default} });

    FONT_DESC desc;
    desc.wstrDrawText = m_wstrFPS;
    desc.wstrFontName = Font_Default;
    desc.vOutlineColor = { 1.f, 0.f, 1.f, 1.f };
    m_pGameInstance->Add_Text_Stack(desc);

    desc.vPosition = _float2(0.f, 50.f);
    desc.eSetting = TEXTDRAW_SETTING::DIR4;
    m_pGameInstance->Add_Text_Stack(desc);

    desc.vPosition = _float2(0.f, 100.f);
    desc.eSetting = TEXTDRAW_SETTING::DIR8;
    m_pGameInstance->Add_Text_Stack(desc);

}
/******************************************************* FPS출력 함수 *******************************************************/



//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
MainApp* ShaderTool::MainApp::Create()
{
    MainApp* pInstance = new MainApp;

    MSG_FAIL(pInstance->Initialize(), L"MainApp 생성에 실패했습니다!", L"경고!!!", nullptr);

    return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void ShaderTool::MainApp::Free()
{
    __super::Free();

    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pLoader);
    Safe_Release(m_pMouse);

    m_pImGuiManager->DestroyInstance();
    m_pGameInstance->Release_Engine();
    Safe_Release(m_pGameInstance);
}
/******************************************************* 객체 반환 함수 *******************************************************/

