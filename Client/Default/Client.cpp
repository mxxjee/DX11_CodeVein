// Client.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "Client_Define.h"
#include "framework.h"
#include "Client.h"
#include "MainApp.h"
#include "GameInstance.h"
#include "GameClock.h"

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE g_hInstance;                          // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
HWND g_hWnd;
_uint g_Flag = 0;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace Client
{
    _float WINCX = 1600/*1600*/;
    _float WINCY = 900/*900*/;
    _bool bIsDuelMonitor = false;
    _bool bIsSeungWooDuel = false;
    _uint g_fFPSRate = FPS_60;
    _bool g_bIMGUI_UIOn = false;
    _bool g_bShowCursor = false;
    _bool g_bFreeCam = false;
}
//레퍼런스 카운트 체크용
_bool bIsDebugging = false;

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void Enable_Console()
{
    AllocConsole();
    g_bPrintRefCnt = false;//bIsDebugging;

    FILE* fpIn{};
    FILE* fpOut{};

    freopen_s(&fpIn, "CONIN$", "r", stdin);
    freopen_s(&fpOut, "CONOUT$", "w", stdout);

    cout.clear();
    cin.clear();

    // 콘솔창 생성 위치 고정
    HWND consolewindow = GetConsoleWindow();

    if (consolewindow)
    {
        int screenCY = GetSystemMetrics(SM_CYSCREEN); // 전체 화면 Y크기
        int consoleCY = 500;    // 콘솔창 크기
        int consoleStartY = int((screenCY - 500) * 0.5); // 콘솔창 Y포인트 시작점
        MoveWindow(consolewindow, 30, consoleStartY, 800, consoleCY, TRUE); // 콘솔창 위치 이동
    }

}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    g_bClient = true;
#ifdef _DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    Enable_Console();
#endif

    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CLIENT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CLIENT));
    MSG msg{};
    msg.message = NULL;

    MainApp* MainApp = MainApp::Create();
    MSG_NULL(MainApp, L"MainApp이 nullptr입니다!", L"경고!!!", 0);

    GameInstance* GameInstance = GameInstance::GetInstance();
    Safe_AddRef(GameInstance);


    _wstring Timer_Normal = L"Timer_Normal";
    _wstring Timer_TargetFPS = L"Timer_TargetFPS";

    ShowCursor(false);

    _float TargetFPS{};

    switch (g_fFPSRate)
    {
    case FPS_FREE:
        TargetFPS = 10000.f;
        break;
    case FPS_60:
        TargetFPS = 60.f;
        break;
    case FPS_144:
        TargetFPS = 144.f;
        break;
    }

    TargetFPS = 90.f;

    GameClock* clock = GameInstance->Add_Clock(L"Clock_Default", 1.f / TargetFPS);

    while (true)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
                break;
            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            auto frameInfo = clock->Tick();

            // 물리/로직 : 고정 간격으로 N번 실행
            //for (_uint i = 0; i < frameInfo.iFixedStepCount; ++i)
            //{
            //    MainApp->FixedUpdate(frameInfo.fFixedDelta);
            //}

            if (frameInfo.bCanUpdate == false)
                continue;

            // 일반 업데이트 : 실제 경과 시간 기반
            MainApp->Update(frameInfo.fDeltaTime);

            // 렌더링 : alpha로 이전/현재 상태 보간
            MainApp->Render(frameInfo.fDeltaTime);
        }
    }


#pragma region 구 파이프라인 Timer
    /*GameInstance->Add_Timer(Timer_Normal);
    GameInstance->Compute_Timer(Timer_Normal);

    GameInstance->Add_Timer(Timer_TargetFPS);
    GameInstance->Compute_Timer(Timer_TargetFPS);

    _float TimeDelta{};
    _float Timer{};
    _float TimeDeltaTargetFPS{};*/

//    // 기본 메시지 루프입니다:
//// 기본 메시지 루프입니다:
//    while (true)
//    {
//        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
//        {
//            if (msg.message == WM_QUIT)
//                break;
//
//            if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
//            {
//                TranslateMessage(&msg);
//                DispatchMessage(&msg);
//            }
//        }
//
//        // 앱 활성화 상태에서만 Update Render 실행
//        //if (g_bAppActive)
//        //{
//            TimeDelta = GameInstance->Compute_Timer(Timer_Normal);
//
//            if (TargetFPS <= 0.f) // FPS_FREE 인 경우
//            {
//                MainApp->Update(TimeDelta);
//                MainApp->Render(TimeDelta);
//            }
//            else
//            {
//                Timer += TimeDelta;
//                _float fFrameTime = 1.f / TargetFPS;
//
//                // 누적된 시간이 프레임 타임을 넘었을 때만 업데이트/렌더링 수행
//                if (Timer >= fFrameTime)
//                {
//                    // Timer = 0.f; 대신 남은 오차를 이월시켜 프레임 정확도를 유지함
//                    TimeDeltaTargetFPS = GameInstance->Compute_Timer(Timer_TargetFPS);
//
//                    MainApp->Update(TimeDeltaTargetFPS);
//                    MainApp->Render(TimeDeltaTargetFPS);
//
//                    Timer = 0.f;
//                }
//            }
//        //}
//        //else
//        //{
//        //    // 비활성화 상태일 때는 CPU 점유율을 낮추기 위해 짧게 휴식
//        //    Sleep(10);
//        //}
//    }
#pragma endregion

    Safe_Release(GameInstance);
    Safe_Release(MainApp);



    if (bIsDebugging)
    {
#ifdef _DEBUG
        system("pause");
#endif // _DEBUG
    }

    return (int) msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex{};

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CLIENT));
    wcex.hCursor        = nullptr; //LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = NULL; //MAKEINTRESOURCEW(IDC_CLIENT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    g_hInstance = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

    lstrcpy(szTitle, L"Code Vein : 씹덕과 일반의 경계");

    RECT window = { 0, 0, (LONG)WINCX, (LONG)WINCY };

    AdjustWindowRect(&window, WS_OVERLAPPEDWINDOW, FALSE);

    _int screenCX = GetSystemMetrics(SM_CXSCREEN);
    _int screenCY = GetSystemMetrics(SM_CYSCREEN);

    _int windowStartX = int((screenCX - WINCX) * 0.5f);
    _int windowStartY = int((screenCY * 0.47) - (WINCY * 0.5f));

    if (bIsDuelMonitor)
    {
        windowStartX -= 1900;
        windowStartY -= 100;
    }
    else if (bIsSeungWooDuel)
    {
        windowStartX;
        windowStartY -= 1100;
    }


    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        windowStartX, windowStartY,
        window.right - window.left, window.bottom - window.top,
        nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    g_hWnd = hWnd;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if(g_bIMGUI_UIOn)
    {
        if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
            return true;
    }

    switch (message)
    {
       

        // Alt + Enter (전체화면 전환) 등을 직접 처리하고 싶다면 여기서 추가 제어
    case WM_SYSKEYDOWN:
        if (wParam == VK_F4)
            DestroyWindow(g_hWnd);

        if (wParam == VK_MENU) // Alt 키 자체 입력 무시
            return 0;

     
        break;



    case WM_SETCURSOR:
        // 커서가 내 게임창 안에 있다면 시스템 커서를 출력하지 않음
        if (LOWORD(lParam) == HTCLIENT)
        {
            SetCursor(NULL);
            return TRUE;
        }
        break;

    case WM_KEYDOWN:
    {
     /*   if (wParam == VK_ESCAPE)
        {
            PostQuitMessage(0);
            break;
        }*/

        //if (wParam == VK_F12)
        //    DestroyWindow(g_hWnd);

        break;
    }

    case WM_ACTIVATEAPP:
    {
        g_bAppActive = (wParam == TRUE);
        break;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// 정보 대화 상자의 메시지 처리기입니다.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
