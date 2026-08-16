#include "Engine_Define.h"

namespace Engine
{
	bool g_bPrintRefCnt{};
    bool g_bDrawRTV{};
	bool g_bAppActive{};
	bool g_bDrawDebugCollider{};
	bool g_bOmniPVDRecording{ false };
	bool g_bPhysXDebug{true};
	bool g_bClient{ false };
	TOOLTYPE g_toolType = TOOLTYPE::TOOL_END;
	_float3 g_vHDRColor = {0.f, 0.f, 0.f};
	_float Engine_WINCX = 1600.f;
	_float Engine_WINCY = 900.f;
}