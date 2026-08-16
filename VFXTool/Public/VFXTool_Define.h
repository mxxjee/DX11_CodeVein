#pragma once

#include <Windows.h>
#include <process.h>
#include <cstdint>

#include "Base.h"

namespace VFXTool
{
    extern _float WINCX;
    extern _float WINCY;
    extern _bool bIsDuelMonitor;
    extern _bool g_IMGUI_UIOn;
    extern _uint g_fFPSRate;

    enum class LEVEL { STATIC, LOADING, MAIN, END };

    enum class CAMERA {
        FREE = 0,
        PLAYER_FOCUS_00, PLAYER_FOCUS_01, PLAYER_FOCUS_02,
        END
    };

    enum FPS_RATE {
        FPS_FREE = 0, FPS_60, FPS_144
    };

    // 샘플 저장 이벤트
    struct SAVEEVENT {
        _bool bSaveJson = true;
        _bool bSaveBinary = true;
        _string strPath = "";
        TOOLTYPE eToolType = TOOLTYPE::TOOL_END;
    };

    // 샘플 로드 이벤트
    struct LOADEVENT {
        _bool bLoadJson = true;
        _bool bLoadBinary = true;
        string m_Path = "";
        TOOLTYPE eToolType = TOOLTYPE::TOOL_END;
    };
}

extern HWND g_hWnd;
extern HINSTANCE g_hInstance;

using namespace std;
using namespace VFXTool;