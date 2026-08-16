#pragma once

#include <Windows.h>
#include <process.h>
#include <cstdint>
//#include <string_view>

#include "Base.h"
#include "Client_Macro.h"
#include <queue>

NS_BEGIN(Engine)
class GameObject;
class ParticleSystem;
NS_END

namespace Client
{
    extern _float WINCX;
    extern _float WINCY;
    extern _bool bIsDuelMonitor;
    extern _bool g_bIMGUI_UIOn;
    extern _uint g_fFPSRate;
    extern _bool g_bShowCursor;
    extern _uint g_Flag;
    extern bool g_bFreeCam;

    // 크기만 1 / 100 해주는 변수
    inline const _matrix PreMatrix_Identity =
        XMMatrixIdentity() * XMMatrixScaling(0.01f, 0.01f, 0.01f);

    // 크기 줄이고 X축 내리기
    inline const _matrix PreMatrix_X_PLUS =
        XMMatrixIdentity() * XMMatrixScaling(0.01f, 0.01f, 0.01f) *
        XMMatrixRotationX(XMConvertToRadians(90.f));

    // 크기 줄이고 X축 올려주기
    inline const _matrix PreMatrix_X =
        XMMatrixIdentity() * XMMatrixScaling(0.01f, 0.01f, 0.01f) *
        XMMatrixRotationX(XMConvertToRadians(-90.f));

    // 크기 줄이고 Y축 돌려주기
    inline const _matrix PreMatrix_Y =
        XMMatrixIdentity() * XMMatrixScaling(0.01f, 0.01f, 0.01f) *
        XMMatrixRotationY(XMConvertToRadians(180.f));

    // 
    inline const _matrix PreMatrix_XY =
        XMMatrixIdentity() * XMMatrixScaling(0.01f, 0.01f, 0.01f) *
        XMMatrixRotationX(XMConvertToRadians(-90.f)) *
        XMMatrixRotationY(XMConvertToRadians(180.f));

    inline const _matrix PreMatrix_XY_PLUS =
        XMMatrixIdentity() * XMMatrixScaling(0.01f, 0.01f, 0.01f) *
        XMMatrixRotationX(XMConvertToRadians(90.f)) *
        XMMatrixRotationY(XMConvertToRadians(180.f));

    enum class LEVEL { STATIC = 0, LOADING, LOGO, MAIN, SAMPLE, UIDEV, BASE, CHURCH, PLAYER, CUSTOMIZE, END };
    enum class CharacterState {
        CHARACTER_IDLE = 0, CHARACTER_FAST, CHARACTER_WAIT, CHARACTER_NORMAL, CHARACTER_END
    };

    //이전 카메라 위치 설정
    enum class CAMERA {
        FREE = 0,
        PLAYER_FOCUS_00, PLAYER_FOCUS_01, PLAYER_FOCUS_02,
        CUSTOMIZE, 
        NPC_SHOP,
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

    struct ESCLOCKEVENT
    {
        bool m_bLock = false;
    };
    // 샘플 저장 이벤트
    struct SAVEEVENT {
        _bool bSaveJson = true;
        _bool bSaveBinary = true;
        TOOLTYPE eToolType = TOOLTYPE::TOOL_END;
    };

    // 샘플 로드 이벤트
    struct LOADEVENT {
        _bool bLoadJson = true;
        _bool bLoadBinary = true;
        string m_Path = "";
        bool    m_bPersistent = false;
        LEVEL   ProtoLevel = LEVEL::STATIC;

        TOOLTYPE eToolType = TOOLTYPE::TOOL_END;
    };

    enum UI_POOLING_TYPE {MONSTERSTATUSBAR,END};
    //풀링할 객체 로드이벤트
    struct LOADEVENT_UIPOOLING
    {
        _bool bLoadJson = true;
        string m_Path = "";
        int     m_iSize = 10;

        UI_POOLING_TYPE ePoolingType = UI_POOLING_TYPE::END;

        TOOLTYPE eToolType = TOOLTYPE::TOOL_END;
    };


 
    enum class IMGUI_EVENT_TYPE { CHANGE_OBJ, UPDATE_PROTOTYPE, END };

    // IMGUI용 이벤트
    struct IMGUI_EVENT {
        IMGUI_EVENT_TYPE eType = IMGUI_EVENT_TYPE::END;
        GameObject* pGameObject = nullptr;
        _bool bDeleteObject = false;
    };


    //로딩끝났음 알릴려고만든이벤트 / 
    enum class SYSTEM_EVENT_TYPE{END_LOAD,END};
    struct SYSTEM_EVENT
    {
        SYSTEM_EVENT_TYPE eType;

    };



#pragma region UI
    enum class UI_Base { NONE, PROGRESSBAR, SLOT, PLAYERHUD, SLOTPANEL, BITMAPTEXT, PLAYERMENUWINDOW, INVENTORYWINDOW, SUBWINDOW, ITEMINFODESC, SHORTCUTMENU,TEXT,POPUP_ITEMGET, POPUP_INTERACTION,BOSSNAME,BLURUI,MINIMAP,MONSTERSTATUS, FADESCREEN, LOADING_BACK, LOADING_CUBE, TITLEWINDOW, SELECTOR, GLOWBUTTON,LOCKON,FOCUSMENU, SCENESELECTMENU, SCENESLOT,HAZE, SAVEPOINT_SLOT,MANAINFO, NPC_DIALOGUE,NPC_CHOICEMENU, SHOP_CATEGORY, SHOP_SLOTGRID,WINDOW_SHOP,SHOP_SLOT,
        SHOP_NEED_INFO, SHOP_DESC_WINDOW, SHOP_DESC_SUBINFO,
       
        //customizing 관련
        CUSTOMIZINGMENU, CUSTOMIZINGMENU_BUTTON, CUSTOMMENU_INFO, CUSTOMIZE_GRID,

        //Colorpalette관련
        PALEETE_WINDOW, PALLETE_AREA, PALLETE_DISPLAY, SLIDER, SLIDER_CURSOR, 
        
        
        COLOR_SELECTOR, TEXTURE_SELECTOR, MESH_SELECTOR, ITEMGRID,VALUESELECTOR,
        BOSSDEAD, WINDOW_LEVELUP,ENDING,
        END
    };
    enum class UI_CATEGORY {
        DEFAULT, HPBAR, STAMINARBAR, GUARDBAR,
        QUICKSLOT, PLAYERHUD_LEFT, PLAYERHUD_RIGHT,

        //슬롯타입들..(읽어올때필요함)
        EQUIP, EQUIP_WEAPON, EQUIP_CLOTHES,
        PASSIVE, SKILL, SHORTCUT, BLOODCODE, SKILLWINDOW, SHORTCUT_QUICK,

        //Desc타입들
        SKILL_INFO, WEAPON_INFO, PASSIVE_INFO, ARMOR_INFO, ITEM_INFO,
        FULL,CENTER,

        //POPUPTYPE
        ITEMPICKUP,SAVEPOINT,LADDER_UP,MAPTITLE,

        SAVEPOINT_SLOT, FOCUSMENU_NPC,

        //UITYPE-CUSTOMIZING TYPE
        SKIN, HAIR, EYE, EYEBROW, MAKEUP, CLOTHES, FACEPAINT,


        PLAYER_INFORMATION, WEAPON_INFORAMTION, SHIELD_INFORMATION,
        END
    };



    //만약 이벤트함수호출대상을 객체로설정했을때, 문자열이아닌 해쉬값비교로 map저장된 이 구조체의 함수호출
    struct UIOwnerEventInfo
    {
        size_t Hash;
        function<void(UI_MasterEvent& e)>        m_Fuc = nullptr;
    };

    struct LevelChangeEvent
    {
        LEVEL eNextLevel;

    };

#pragma endregion
    enum class SLOT_TYPE {
        EQUIP_WEAPON,
        EQUIP_ARMOR,
        SKILL,
        PASSIVE,

        SHORTCUT,
        BLOODCODE,

        QUICKSLOT,
        PRIVIEW,
        TOTAL,
        SHORTCUT_QUICK,
        END
    };

    

    enum class MAINUISLOT { EQUIP_WEAPON, EQUIP_ARMOR, SKILL_1, SKILL_2, PASSIVE, ITEM, BLOODCODE, END };
    enum class EQUIP_TYPE { WEAPON, ARMOR = 2 };

#define MAX_SLOT        8;

#pragma region 아이템관련
    enum class ITEM_CATEGORY { EQUIP_WEAPON, EQUIP_ARMOR, PASSIVE, SKILL, ITEM, BLOODCODE, END };
    enum class ITEM_VALUE {
        //WEAPON - ARMORS
        ATTACK_POWER,//공격력(값X,단지표시용)

        PHYSICS_DMG,//물리 공격력/속성

        //SKILLS
        ICHOR_COST,//소비 명혈
        COOLDOWN,//재사용ㅅ간
        SKILL_TYPE,//연혈타입
        LINEAGE_TYPE,//계통
        ATTACK_TYPE, //공격속성
        END
        //
    };

    inline static string  ItemValue_To_String(ITEM_VALUE eValue)
    {
        switch (eValue)
        {
        case Client::ITEM_VALUE::ATTACK_POWER:
            return "ATTACK_POWER";
            break;
        case Client::ITEM_VALUE::PHYSICS_DMG:
            return "PHYSICS_DMG";
            break;
        case Client::ITEM_VALUE::ICHOR_COST:
            return "ICHOR_COST";
            break;
        case Client::ITEM_VALUE::COOLDOWN:
            return "COOLDOWN";
            break;
        case Client::ITEM_VALUE::SKILL_TYPE:
            return "SKILL_TYPE";
            break;
        case Client::ITEM_VALUE::LINEAGE_TYPE:
            return "LINEAGE_TYPE";
            break;

        case Client::ITEM_VALUE::ATTACK_TYPE:
            return "ATTACK_TYPE";
            break;

        case Client::ITEM_VALUE::END:
            break;
        default:
            break;
        }

        return "";
    }

    inline static ITEM_VALUE  String_To_ItemValue(string str)
    {
        if (str == "ATTACK_POWER")
            return ITEM_VALUE::ATTACK_POWER;

        else if (str == "PHYSICS_DMG")
            return ITEM_VALUE::PHYSICS_DMG;

        else if (str == "ICHOR_COST")
            return ITEM_VALUE::ICHOR_COST;

        else if (str == "COOLDOWN")
            return ITEM_VALUE::COOLDOWN;


        else if (str == "SKILL_TYPE")
            return ITEM_VALUE::SKILL_TYPE;


        else if (str == "SKILL_TYPE")
            return ITEM_VALUE::SKILL_TYPE;

        else if (str == "LINEAGE_TYPE")
            return ITEM_VALUE::LINEAGE_TYPE;

        else if (str == "ATTACK_TYPE")
            return ITEM_VALUE::ATTACK_TYPE;


        else
            return ITEM_VALUE::END;
    }

    typedef struct tagItemInfo
    {
        wstring ItemName = L"";       //아이템이름
        wstring ItemDescription = L"";  //아이템정보


        string ItemTexKey = "";     //아이템 아이콘 표시할 텍스처키
        int     itemCount = 0;      //additem시 한번 획들할 양, 인벤토리에있을 경우 현재소지하는양

        ITEM_CATEGORY       m_eCategory;    //아이템카테고리(대분류)
        //   _uint               m_iItemType;        //아이템 서브타입(소분류)

        _uint                   maxItemSize = 0;
        _uint                   ItemID; //각 이름을 기준으로 아이템이름세팅

        bool                m_bMain = false;    //메인에등록되었는지.

        _uint               m_iHaze = 0;        //이 아이템의 가격(헤이즈)

        ///ItemData들
        wstring     m_wstrLineage_Type = L"";
        wstring     m_SkillType = L"";
        wstring     m_AttackType = L""; //공격 속성(item전용)

        _float   m_Values[_UINT(ITEM_VALUE::END)] = { -1.f, };

        _float      Get_Value(ITEM_VALUE eValueType) { return m_Values[_UINT(eValueType)]; }

        inline static string      Get_Category_To_String(ITEM_CATEGORY eType)
        {
            switch (eType)
            {
            case Client::ITEM_CATEGORY::EQUIP_WEAPON:
                return "EQUIP_WEAPON";
                break;
            case Client::ITEM_CATEGORY::EQUIP_ARMOR:
                return "EQUIP_ARMOR";
                break;
            case Client::ITEM_CATEGORY::PASSIVE:
                return "PASSIVE";
                break;
            case Client::ITEM_CATEGORY::SKILL:
                return "SKILL";
                break;
            case Client::ITEM_CATEGORY::ITEM:
                return "ITEM";
                break;
            case Client::ITEM_CATEGORY::BLOODCODE:
                return "BLOODCODE";
                break;
            case Client::ITEM_CATEGORY::END:
                break;
            }

            return "";
        }
        inline static ITEM_CATEGORY               Get_Category_To_Enum(string str)
        {
            if (str == "EQUIP_WEAPON")
                return ITEM_CATEGORY::EQUIP_WEAPON;


            else if (str == "EQUIP_ARMOR")
                return ITEM_CATEGORY::EQUIP_ARMOR;

            else if (str == "PASSIVE")
                return ITEM_CATEGORY::PASSIVE;

            else if (str == "SKILL")
                return ITEM_CATEGORY::SKILL;

            else if (str == "ITEM")
                return ITEM_CATEGORY::ITEM;

            else if (str == "BLOODCODE")
                return ITEM_CATEGORY::BLOODCODE;

            return ITEM_CATEGORY::END;
        }
    }ItemInfo;

}


typedef struct tagPlayerSkillInfo
{
    _uint idx = 0;          //퀵슬롯중에 어느인덱스인지, 근데 내꺼는 skill_1,skill_2구역이 나눠져있어서 skill_2구역은 +4해서해줌

                    //  1
                    //0   2
                    //  3    
            //5
        //4        6
            //7


    Client::ItemInfo* m_ItemInfo = nullptr;

}PlayerSkillInfo;

#pragma endregion

#pragma region Minimap
enum class MINIMAP_TYPE { CENTER, FULL, END };

#pragma endregion

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

struct SAVE_POINT_INFO
{
    _uint iIndex;              // 같은 맵 내 저장점 인덱스
    Client::LEVEL targetLevel;          // 텔레포트할 목표 레벨
    MAP_TYPE mapType;           // 맵 타입
    _float4 spawnPosition;      // 플레이어 스폰 위치
    _float4 spawnRotation;      // 플레이어 스폰 회전
    wstring wsName;             // 저장점 이름
};


#pragma region NPC관련

//////////////NPC관련/////////////////

            //NPC이름
enum class NPCTYPE{
    MURASAME,//상점 NPC
    END};

enum class DIALOGUE_STATE{TEXT,CHOICE,END};

struct Choice
{
    wstring     text;       //선택지버튼에 출력할 text(도망간다/가만히있는다)
    int     nextStep;   //다음에 이동할 대사 idx, -1이면 종료
    string  action;     //선택지 끝나고 나올 액션 (if문으로 람다정의)
};
struct DialogueLine
{
    int                 step = 0;   //현재 대사 번호
    wstring wstrText;           //대화 내용
    vector<Choice>     vecChoices; //선택지 리스트,단지 말만나오게하는거면 이거비워두기.
};


struct NPCData
{
    wstring npcName;//npc이름..
    _float3 OffSet= _float3(-0.3f, 1.5f, 0.f);      //UI메뉴 오프셋값
    map<int, DialogueLine>       dialogueMap;   //Key:Step번호 /value:대화 구조체
};

#pragma endregion



#pragma region Interaction
enum class INTERACTION_TYPE{ SAVEPOINT,LADDER,DOOR,NPC,ITEM,ITEM_BOX,END};
struct PlayerIntercation
{
    _bool               bEKeyDown = { false }; //E키 눌렀는지
    INTERACTION_TYPE    eInteractionType; //해당 상호작용 오브젝트 타입(상태 분리)
    _bool               bClimbUp = { false }; //올라가는중인지,내려가는중인지
    _float4             vSnapPos = {}; //스냅될 위치 
    _float3             vLook = {}; //바라볼 방향 
    _float3             vLadderTopPos = {}; //사다리 꼭대기 위치
    _float3             vLadderBottomPos = {}; //사다리 바닥 위치
};
#pragma endregion

typedef struct tagProjectileArrivalEvent
{
    _uint iOwnerID = {}; // 투사체를 발사한 오브젝트
    _uint iTargetID = {}; // 타겟오브젝트 
    _bool bSuccess = { false }; // 성공여부 , 플레이어에게 도달했는지

} Projectile_ARRIVAL_EVENT;

typedef struct tagYakumoActive
{
    _bool bSuccess = { false }; // 성공여부

} YAKUMO_ACTIVE_EVENT;

typedef struct tagYakumoTeleport
{
    _float fDelay = { 0.f };

} YAKUMO_TELEPORT_EVENT;

#pragma region Player
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


#pragma region 커마 타입
enum CUSTOMIZING_TYPE
{
    SKIN,HAIR,EYE,EYEBROW,MAKEUP,CLOTHES,FACEPAINT ,END
};

enum class CUSTOMIZING_VALUE_TYPE
{
    COLOR,TEXTURE,MESH,VALUE,END
};
inline CUSTOMIZING_TYPE        Get_CustomType(string m_UIType)
{
    if (m_UIType == "SKIN")
        return CUSTOMIZING_TYPE::SKIN;

    else if (m_UIType == "HAIR")
        return CUSTOMIZING_TYPE::HAIR;


    else if (m_UIType == "EYE")
        return CUSTOMIZING_TYPE::EYE;


    else if (m_UIType == "EYEBROW")
        return CUSTOMIZING_TYPE::EYEBROW;


    else if (m_UIType == "MAKEUP")
        return CUSTOMIZING_TYPE::MAKEUP;


    else if (m_UIType == "CLOTHES")
        return CUSTOMIZING_TYPE::CLOTHES;


    else if (m_UIType == "FACEPAINT")
        return CUSTOMIZING_TYPE::FACEPAINT;


    return CUSTOMIZING_TYPE();
}

inline string       Get_CustomType_Str(CUSTOMIZING_TYPE Type)
{
    switch (Type)
    {
    case SKIN:
        return "SKIN";
        break;

    case HAIR:
        return "SKIN";
        break;

    case EYE:
        return "EYE";
        break;

    case EYEBROW:
        return "EYEBROW";
        break;

    case MAKEUP:
        return "MAKEUP";
        break;

    case CLOTHES:
        return "CLOTHES";
        break;

    case FACEPAINT:
        return "FACEPAINT";
        break;
    }

    return "END";
}

//편집기에서 편집하고 보낼값들
struct Export_ColorData
{
    _float4 vColor;
    _float fSaturate = 0.f;
};


///커스터마이징 매니저에 저장할값들(단지 ui세팅용)
struct CustomColorInfo
{
    _float2 vCursorPos; //Palette Area안에서의 커서로컬위치를 저장. 읽어서 다시 위치시키기 위해서 저장한다.
    _float SliderCursorPosY;     //Slider안에서의 cursor위치(Y값) .다시 세팅 시 필요.

    _float  fSaturation;           //슬라이더의 value값
    _float4 vColor;         //실제로 의미ㅏㅎ는 RGB색상 저장 (Hue값 포함)


    function<void(void*)>    m_UpdateFunc = nullptr;     //플레이어랑 연동하기위해 호출하는함수
};

struct CustomItemGridInfo
{
    _int TexIdx = 0;
    function<void(void*)>        m_UpdateFunc = nullptr;

    CustomItemGridInfo()
    {
    }

    CustomItemGridInfo(_int Idx, function<void(void*)> Func)
        :TexIdx(Idx), m_UpdateFunc(Func)
    {
    }
};

struct CustomAlphaValueInfo
{
    _float fRatio = 0;
    function<void(void*)>        m_UpdateFunc = nullptr;
    
    CustomAlphaValueInfo()
    {
    }

    CustomAlphaValueInfo(_float Ratio, function<void(void*)> Func)
        :fRatio(Ratio), m_UpdateFunc(Func)
    {
    }
};


struct CustomKey
{
    string Name;
    _uint idx = 0;

    bool operator==(const CustomKey& other) const {
        return Name == other.Name && idx == other.idx;
    }
};
template <>
struct hash<CustomKey>
{
    size_t operator()(const CustomKey& key) const
    {
        size_t h1 = hash<string>()(key.Name);
        size_t h2 = hash<unsigned int>()(key.idx);

        // 두 해시값을 섞어줌 (0x9e3779b9는 마법의 숫자로 불리는 황금비율 상수야)
        return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
    }
};


//실제 셰이더에서 쓸용
struct CustomData
{
    unordered_map<CustomKey, CustomColorInfo> m_ColorInfos;//색상정보

    unordered_map<CustomKey, CustomItemGridInfo>    m_Texs;//텍스처번호(눈, 페이스페인팅 등에사용)
    unordered_map<CustomKey, CustomItemGridInfo>    m_Meshs; //메시번호  (머리카락,옷,등에 사용)
    unordered_map<CustomKey, CustomAlphaValueInfo>    m_Values;//하이라이트강도 및 어떠한 값들을 넘겨주는용도
    //vector<_int*>   m_iTexIdxs;      
    //vector<_int*>   m_iMeshIdxs;    
    //vector<_float>      m_Values;   

};
#pragma endregion


#pragma region 보스타입
enum class BOSS_TYPE{ OLIVER,WOLFGOST,WHITE_DEVIL,END };
#pragma endregion

#pragma region 아이템이벤트
enum class ITEM_TRIGGER { NONE = 0, CHURCH_RAID, CHURCH_ENDING, END};

struct ITEM_TRIGGER_EVENT
{
    ITEM_TRIGGER eTrigger = ITEM_TRIGGER::NONE;
    _float3 fPos;//위치도전송
};
#pragma endregion

#include "Client_Function.h"

extern HWND g_hWnd;
extern HINSTANCE g_hInstance;

using namespace std;
using namespace Client;
