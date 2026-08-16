#pragma once

#include <windows.h>
#include <process.h>
#include <nlohmannjson/json.hpp>

#include "Engine_Define.h"
#include "GameObject.h"
#include "MT_Macro_Component.h"
#include "MT_Macro_GameObject.h"

extern HWND g_hWnd;
extern HINSTANCE g_hInst;
extern unsigned int g_iGlobalObjCount;
inline GameObject* g_pSelectedObject = nullptr;

static const unsigned int g_iWinSizeX = 1600;
static const unsigned int g_iWinSizeY = 900;

enum class LEVEL { TOOL, END };
enum class CAMERA {
    FREE = 0,
    PLAYER_FOCUS_00, PLAYER_FOCUS_01, PLAYER_FOCUS_02,
    END
};

struct SAVEEVENT {
    _bool bSaveJson = true;
    _bool bSaveBinary = true;
    _string strSaveFilePath = "../../DataFiles/Map_Data/Level_Main/MapData.json";
    TOOLTYPE eToolType = TOOLTYPE::TOOL_END;
};

enum class TOOL_MODE {
    NONE,			// 아무것도 안함
    MAP_PLAYER,		// 플레이어 배치
    MAP_MONSTER,	// 몬스터 배치
    SPLINE_EDIT,		// 스플라인
    MAP_FUNCTION
};

enum class MAP_TYPE {
    NONE,
    ST00_BASE,
    ST01_UNDER,
    ST02_CHURCH,
    ST03_,
    ST04_,
    END,
};

enum class POINT_TYPE { PATH, ZONE };

struct SplineNode {
    Vector3 vPos;
    POINT_TYPE eType = POINT_TYPE::PATH;
    float fRadius = 0.0f;
};

extern TOOL_MODE g_eToolMode;

#include "MT_Utils.h"

using namespace std; 