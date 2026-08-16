#pragma once

#include <Windows.h>
#include <process.h>
#include <cstdint>

#include "Base.h"

#include "Client_Macro.h"

namespace AnimationTool
{
    extern _float WINCX;
    extern _float WINCY;
    extern _bool bIsDuelMonitor;
    extern _bool g_IMGUI_UIOn;
    extern _uint g_fFPSRate;

    enum class LEVEL { STATIC,LOADING, LOGO, MAIN, END };
    enum class CAMERA {
        FREE = 0,
        PLAYER_FOCUS_00, PLAYER_FOCUS_01, PLAYER_FOCUS_02,
        END
    };

    // 크기만 1 / 100 해주는 변수
    inline const _matrix PreMatrix_Identity =
        XMMatrixIdentity() * XMMatrixScaling(0.01f, 0.01f, 0.01f);

    // 
    inline const _matrix PreMatrix_XY =
        XMMatrixIdentity() * XMMatrixScaling(0.01f, 0.01f, 0.01f) *
        XMMatrixRotationX(XMConvertToRadians(-90.f)) *
        XMMatrixRotationY(XMConvertToRadians(180.f));

    inline const _matrix PreMatrix_XY_PLUS =
        XMMatrixIdentity() * XMMatrixScaling(0.01f, 0.01f, 0.01f) *
        XMMatrixRotationX(XMConvertToRadians(90.f)) *
        XMMatrixRotationY(XMConvertToRadians(180.f));

    struct SphericalCoord
    {
        _float fRadius{};
        _float fTheta{};
        _float fPhi{};
    };


    enum FPS_RATE {
        FPS_FREE = 0, FPS_60, FPS_144
    };


    // 저장 이벤트
    struct SAVEEVENT {
        _bool bSaveJson = true;
        _bool bSaveBinary = true;
        _string strPath = "";
        TOOLTYPE eToolType = TOOLTYPE::TOOL_END;
    };

    // 로드 이벤트
    struct LOADEVENT {
        _bool bLoadJson = true;
        _bool bLoadBinary = true;
        string m_Path = "";
        TOOLTYPE eToolType = TOOLTYPE::TOOL_END;
    };

    //로딩끝났음 알릴려고만든이벤트 / 
    enum class SYSTEM_EVENT_TYPE { END_LOAD, END };
    struct SYSTEM_EVENT
    {
        SYSTEM_EVENT_TYPE eType;

    };

}

extern HWND g_hWnd;
extern HINSTANCE g_hInstance;

using namespace std;
using namespace AnimationTool;