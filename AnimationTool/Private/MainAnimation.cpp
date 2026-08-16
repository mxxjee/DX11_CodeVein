#include "AnimationTool_Define.h"
#include "GameInstance.h"
#include "ImguiManager.h"
#include "MainAnimation.h"
#include "Window_AnimEditor.h"
#include "Window_AnimList.h"
#include "Window_AnimTimeline.h"

#include "Loader.h"
#include "Level_Load.h"
#include "DebugDraw.h"

USING(AnimationTool)

MainAnimation::MainAnimation()
{
}

HRESULT MainAnimation::MainAnimation::Initialize()
{
    g_vHDRColor = _float3(0.f,0.f, 1.f);
    
    // 26.02.20에 승우가 추가함
    g_bDrawDebugCollider = true;
    g_toolType = TOOLTYPE::ANIMATION_TOOL;

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

    g_fFPSRate = FPS_144;

    Create_Windows();

    CHECK_FAILED(Ready_Static(), E_FAIL);
    CHECK_FAILED(Start_Level(), E_FAIL);

    return S_OK;
}

HRESULT MainAnimation::Ready_Static()
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

    return S_OK;
}

HRESULT MainAnimation::Start_Level()
{
    m_pGameInstance->Add_Level(CAST(_uint)(LEVEL::LOADING), Level_Load::Create(m_pDevice, m_pContext, LEVEL::LOGO));
    m_pGameInstance->Change_Level(CAST(_uint)(LEVEL::LOADING));


    return S_OK;
}

HRESULT MainAnimation::Create_Windows()
{
    ImguiWindow::IMGUIWINDOW_DESC AnimToolListDesc;
    AnimToolListDesc.m_WindowTitle = "Animation_List";

    Window_AnimList* pAnimToolListWindow = Window_AnimList::Create(m_pDevice, m_pContext, &AnimToolListDesc);
    if (!pAnimToolListWindow)
        return E_FAIL;

    ImguiWindow::IMGUIWINDOW_DESC AnimToolEditDesc;
    AnimToolEditDesc.m_WindowTitle = "Animation_Edit";

    Window_AnimEditor* pAnimToolEditWindow = Window_AnimEditor::Create(m_pDevice, m_pContext, &AnimToolEditDesc);
    if (!pAnimToolEditWindow)
        return E_FAIL;

    ImguiWindow::IMGUIWINDOW_DESC AnimToolTimelineDesc;
    AnimToolTimelineDesc.m_WindowTitle = "Animation_Timeline";

    Window_AnimTimeline* pAnimToolTimelineWindow = Window_AnimTimeline::Create(m_pDevice, m_pContext, &AnimToolTimelineDesc);
    if (!pAnimToolTimelineWindow)
        return E_FAIL;


    //ImguiManager에게 추가한다.
    m_pImGuiManager->RegisterWindow(pAnimToolListWindow);
    m_pImGuiManager->RegisterWindow(pAnimToolEditWindow);
    m_pImGuiManager->RegisterWindow(pAnimToolTimelineWindow);

    return S_OK;
}

void MainAnimation::Update(const _float fTimeDelta)
{
    Show_FPS(fTimeDelta);

    if (m_pLoader != nullptr && m_pLoader->Is_LoadComplete())
        Safe_Release(m_pLoader);

    m_pGameInstance->Update_Level(fTimeDelta);
    m_pImGuiManager->Update_Priority(fTimeDelta);
    m_pImGuiManager->Update(fTimeDelta);
}

void MainAnimation::Render(const _float fTimeDelta)
{
    static _float4 clearcolor = _float4(0.f, 1.f, 1.f, 1.f);

    m_pGameInstance->Draw_Begin(&clearcolor);

    m_pGameInstance->Render_Grid();
    m_pGameInstance->Draw(fTimeDelta);

    m_pImGuiManager->Render();

    m_pGameInstance->Draw_Text_Begin();
    m_pGameInstance->Render_Texts_Stacked();
    m_pGameInstance->Draw_Text_End();


    m_pGameInstance->Draw_End();
}



//////////////////////////////////////////////////////// FPS출력 함수 ////////////////////////////////////////////////////////
void MainAnimation::Show_FPS(const _float fTimeDelta)
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
}
/******************************************************* FPS출력 함수 *******************************************************/

MainAnimation* MainAnimation::Create()
{
    MainAnimation* pInstance = new MainAnimation;

    MSG_FAIL(pInstance->Initialize(), L"MainAnimation 생성에 실패했습니다!", L"경고!!!", nullptr);

    return pInstance;
}

void MainAnimation::Free()
{
    __super::Free();
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
    Safe_Release(m_pLoader);

    m_pImGuiManager->DestroyInstance();
    m_pGameInstance->Release_Engine();
    Safe_Release(m_pGameInstance);


}