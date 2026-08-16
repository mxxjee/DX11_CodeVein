#pragma once

#include <Windows.h>
#include <process.h>
#include <cstdint>
//#include <string_view>

#include "Base.h"
#include "ShaderTool_Macro.h"


namespace ShaderTool
{
	extern _float WINCX;
	extern _float WINCY;
    extern _bool bIsDuelMonitor;
    extern _bool g_IMGUI_UIOn;
    extern _uint g_fFPSRate;

	enum class LEVEL { STATIC, LOADING, LOGO, MAIN, END};
    enum class CharacterState {
        CHARACTER_IDLE = 0, CHARACTER_FAST, CHARACTER_WAIT, CHARACTER_NORMAL, CHARACTER_END
    };
    
    //이전 카메라 위치 설정
    enum class CAMERA {
        FREE = 0,
        PLAYER_FOCUS_00, PLAYER_FOCUS_01, PLAYER_FOCUS_02,
        END
    };

    //트랙찍을때 쓰던거 지금은 안씀
    struct SphericalCoord
    {
        _float fRadius{};
        _float fTheta{};
        _float fPhi{};
    };

    //프레임 레이트
    enum FPS_RATE {
        FPS_FREE = 0, FPS_60, FPS_144
    };

    // 저장 이벤트
    struct SAVEEVENT {
        _bool bSaveJson = true;
        _bool bSaveBinary = true;
        TOOLTYPE eToolType = TOOLTYPE::TOOL_END;
    };

    // 로드 이벤트
    struct LOADEVENT {
        _bool bLoadJson = true;
        _bool bLoadBinary = true;
        TOOLTYPE eToolType = TOOLTYPE::TOOL_END;
    };

#pragma region MapObject

    enum class MAP_TYPE {
        NONE,
        ST00_BASE,
        ST01_UNDER,
        ST02_CHURCH,
        ST03_,
        ST04_,
        END,
    };

    typedef struct tagPlayerBodyShaderDesc
    {
        _float4 vInnerColor[6];
        _float3 PaddingBody;
        _float  fColorStrength;
    }PLAYER_BODY_SHADER_DESC;


    typedef struct tagPlayerHairShaderDesc
    {
        //Hair_json에서 값은 참조
        _float4 vPlayerHairColor; //플레이어 머리색

        _float fPlayerHighlight1Shift; //첫번째 하이라이트의 위치를 이동 , 머리색과 비슷한 색의 광택
        _float fPlayerHighlight1Strength; //첫번째 하이라이트의 밝기(강도) 높을수록 머리카락이 반짝 , 기름짐
        _float fPlayerHighlight2Shift; //두번째 하이라이트의 위치를 이동 / 1번과 살짝 어긋나야 자연스러운 값
        _float fPlayerHighlight2Strength; //두번째 하이라이트의 밝기(강도) 높을수록 머리카락이 반짝 , 기름짐
        
        _float fPlayerHighlightPower; //광택의 날카로움(모임 정도)
        _float fPlayerMetallic; //낮게 설정 / 높이면 인조가발 느낌
        _float fPlayerRoughnessMin; //최소 거칠기 / 빛이 너무 완벽하게 반사되는것을 막는 값
        _float HairPadding;
    }PLAYER_HAIR_SHADER_DESC;

    typedef struct tagPlayerEyeShaderDesc
    {
        _float4 vPlayerEyeBaseColor; //홍채의 기본(바깥쪽) 추측 (원작 커스터마이징 처럼 좌우 눈으로 하려면 Eye_R , Eye_L과 같이 구분해야함)
        _float4 vPlayerEyeAppendColor; //홍채의 중심부 추측  

        _float fPlayerEyeScale; //홍채크기
        _float fPlayerEyeIndividualScaleOffset; //텍스처마다 홍채 크기/정렬 보정같음
        _float fPlayerEyeOffsetU;  //눈동자 시선 제어
        _float fPlayerEyeOffsetV; //눈동자 시선 제어

        _float fPlayerEyeHighlightStrength; //하이라이트(반사광)강도

        _float fPlayerTatooOnOff; //타투 ON/OFF
        _float fPlayerTattoAlpha;// 타투 알파값
        _float fPlayerTattoScale;  //타투 크기
        _float fPlayerTattoOffsetU; //타투 X위치
        _float fPlayerTattoOffsetV; //타투 y위치
        _float2 PaddingEye;
        _float4 vPlayerTattoColor; //타투색깔

    }PLAYER_EYE_SHADER_DESC;

    typedef struct tagPlayerBrowShaderDesc
    {
        _float4		vPlayerBrowColor; //눈썹 색깔
        _float4		vPlayerEyeLineColor; //아이라인 색깔
        _float4		vPlayerLipColor; //립색깔

        _float2		vPlayerBrowUVOffset; //json값 읽어오기
        _float2		fPlayerBrowUVScale; //json값 읽어오기
        _int		iPlayerUseSecondUV; //2번째 UV사용할것인지 여부
        _float		fPlayerBrowUVRotation; //회전
        _float		fPlayerBrowXGap;//눈썹 x 간격
        _float      fEyeLineAlpha; //아이라인알파
        _float3     padding33;
        _float      fLipAlpha; //립알파

    }PLAYER_BROW_SHADER_DESC;

     typedef struct tagPlayerShaderDesc
     {
         PLAYER_BODY_SHADER_DESC m_tBodyDesc;
         PLAYER_HAIR_SHADER_DESC m_tHairDesc;
         PLAYER_EYE_SHADER_DESC m_tEyeDesc;
         PLAYER_BROW_SHADER_DESC m_tBrowDesc;
     }PLAYER_SHADER_DESC;

     typedef struct tagPlayerHeadTextureChange
     {
         _int iBrowNum = 0;
         _int iEyelashNum = 0;
         _int iEyeWhiteNum = 0;
         _int iEyeDetailNum = 0;
         _int iEyeHighlightNum = 0;
         _int iTattoPaintNum = 0;

     }HEAD_TEXTURE_CHANGE;

     typedef struct tagPlayerMeshTextureNum
     {
         _int* pPlayerHairNum;
         _int* pPlayerBodyNum;
         HEAD_TEXTURE_CHANGE* tHeadTexture;

     }PLAYER_MESHTEX_NUM;

#pragma endregion
}

extern HWND g_hWnd;
extern HINSTANCE g_hInstance;

using namespace std;
using namespace ShaderTool;
