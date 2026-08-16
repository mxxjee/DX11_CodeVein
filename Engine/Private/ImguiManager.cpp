#include "Engine_Define.h"
#include "ImguiManager.h"
#include "GameInstance.h"
#include "ImguiWindow.h"
#include "ImGuizmo.h"


IMPLEMENT_SINGLETON(ImguiManager)

Engine::ImguiManager::ImguiManager()
{
}

Engine::ImguiManager::~ImguiManager()
{
}

void Engine::ImguiManager::Init(HWND hWnd, ID3D11Device* device, ID3D11DeviceContext* device_context)
{
    m_pContext = device_context;

    //모니터 세팅에 맞는 Imgui 세팅
    ImGui_ImplWin32_EnableDpiAwareness();
    float main_scale = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));


    //Imgui context세팅
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

    // Setup Dear ImGui style
    ImGui::StyleColorsMint();
    //ImGui::StyleColorsLight();
   // ImGui::StyleColorsSpectrum();
    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = main_scale;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)
    io.ConfigDpiScaleFonts = true;          // [Experimental] Automatically overwrite style.FontScaleDpi in Begin() when Monitor DPI changes. This will scale fonts but _NOT_ scale sizes/padding for now.
    io.ConfigDpiScaleViewports = true;      // [Experimental] Scale Dear ImGui and Platform Windows when Monitor DPI changes.

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX11_Init(device, device_context);

}

_int Engine::ImguiManager::Update_Priority(const _float fTimeDelta)
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    //ImGuizmo::BeginFrame(); // Guizmo 프레임 시작
    //ImGuizmo::SetRect(0, 0, Engine_WINCX, Engine_WINCY);


    BeginDockSpace();//도킹기능

   
    return 0;
}

_int Engine::ImguiManager::Update(const _float fTimeDelta)
{
   // Test();//Imgui 창 테스트
    for (auto& pair : m_Windows)
        pair.second->Update(fTimeDelta);


    return 0;
}

_int Engine::ImguiManager::Render()
{
    ImGui::Render();
    //  const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
     // g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
     // g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    // 뷰포트 옵션 켠 경우 추가
    ImGuiIO& io = ImGui::GetIO();

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        // DX11 상태 백업
        ID3D11RenderTargetView* backup_rt = nullptr;
        ID3D11DepthStencilView* backup_ds = nullptr;
        m_pContext->OMGetRenderTargets(1, &backup_rt, &backup_ds);

        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();

        //// DX11 상태 복원
        m_pContext->OMSetRenderTargets(1, &backup_rt, backup_ds);
        if (backup_rt) backup_rt->Release();
        if (backup_ds) backup_ds->Release();
    }

    return 0;

}

void Engine::ImguiManager::Test()
{
    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);
    ImGuiIO& io = ImGui::GetIO();

    ImGui::Text("DockingEnable: %s", (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) ? "ON" : "OFF");
    ImGui::Text("ViewportsEnable: %s", (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) ? "ON" : "OFF");

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &show_another_window);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", (float*)&clearColor); // Edit 3 floats representing a color

        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        //     ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::End();
    }

    // 3. Show another simple window.
    if (show_another_window)
    {
        ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            show_another_window = false;
        ImGui::End();
    }

}

void Engine::ImguiManager::BeginDockSpace()
{
    const ImGuiViewport* viewport = ImGui::GetMainViewport();

    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0, 0, 0, 0));

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoBackground;

    ImGui::Begin("DockSpace Demo", nullptr, window_flags);

    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

    ImGui::End();

    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(2);
}
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT Engine::ImguiManager::WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam);
}

/////////////////////////////////////////////////////
HRESULT Engine::ImguiManager::RegisterWindow(ImguiWindow* pInstance)
{
    CHECK_NULLPTR(pInstance);

    /*중복체크*/
    string WindowTitle = pInstance->Get_WindowTitle();

    if (Find_Window(WindowTitle) == nullptr)
        m_Windows[WindowTitle] = pInstance;
    
    return S_OK;
}

ImguiWindow* Engine::ImguiManager::Find_Window(string _WindowTitle)
{
    auto pTargetIter = m_Windows.find(_WindowTitle);
    ImguiWindow* pTarget = nullptr;

    if (pTargetIter != m_Windows.end())
        pTarget = pTargetIter->second;

    return pTarget;
}

void Engine::ImguiManager::Reset_Window(string _WindowName)
{
    ImguiWindow* pWindow = Find_Window(_WindowName);
    if (pWindow)
        pWindow->Reset();
}

void Engine::ImguiManager::Reset_All_Window()
{
    for (auto& window : m_Windows)
    {
        window.second->Reset();
    }
}


void Engine::ImguiManager::Set_Open_All_Window(_bool _open)
{
    for (auto& window : m_Windows)
    {
        window.second->Set_Open(_open);
    }
}
//////////////////////////////////////////////////


void Engine::ImguiManager::Free()
{
    for (auto& pair : m_Windows)
    {
        Safe_Release(pair.second);

    }

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    __super::Free();

}
