#pragma once

#include <Windows.h>
#include <process.h>
#include <cstdint>

#include "Base.h"
#include "UITool_Macro.h"


namespace UITool
{
    extern _float WINCX;
    extern _float WINCY;
    extern _bool bIsDuelMonitor;
    extern _bool g_bIMGUI_UIOn;
    extern _uint g_fFPSRate;
    extern _bool bUseCopyObject;
   enum class LEVEL { STATIC,LOADING,MAIN, PARSING=9,END };
    enum class CAMERA {
        FREE = 0,
        PLAYER_FOCUS_00, PLAYER_FOCUS_01, PLAYER_FOCUS_02,
        END
    };


    enum FPS_RATE {
        FPS_FREE = 0, FPS_60, FPS_144
    
    };


    enum class FILETYPE{ALL,PREFAB};

    enum class UI_PREFAB {HPBAR,END};

    // 샘플 저장 이벤트
    struct SAVEEVENT {
        _bool bSaveJson = true;
        FILETYPE eType=FILETYPE::ALL;
        _bool bSaveBinary = true;
        string Path = "";

        void* pData = nullptr;

        TOOLTYPE eToolType = TOOLTYPE::TOOL_END;
    };

    // 샘플 로드 이벤트
    struct LOADEVENT {
        _bool bLoadJson = true;
        FILETYPE eType = FILETYPE::ALL;
        _bool bLoadBinary = true;
        string Path = "";


        TOOLTYPE eToolType = TOOLTYPE::TOOL_END;
    };


    //구체화할 클래스타입모음
    //PROGRESSBAR-HPBAR/STEMINARBAR/MONSTERHP..등
    //SLOT - QUICKSLOT,INVENTORYSLOT..

    //enum class UI_Base { NONE, PROGRESSBAR, SLOT, PLAYERHUD, SLOTPANEL, BITMAPTEXT, PLAYERMENUWINDOW, INVENTORYWINDOW,SUBWINDOW,TEXT,ITEMINFODESC,SHORTCUTMENU,POPUP_ITEMGET,POPUP_INTERACTION,BOSSNAME,BLURUI,MINIMAP,MONSTER_STATUSBAR,FADESCREEN,LOADING_BACK,LOADING_CUBE,TITLEWINDOW,SELECTOR, GLOWBUTTON,LOCKON,FOCUSMENU,
    //    SCENESELECTMENU,SCENESLOT,HAZE,SAVEPOINT_SLOT,MANAINFO,NPC_DIALOGUE,NPC_CHOICEMENU,WINDOW_SHOP,SHOP_CATEGORY, SHOP_SLOTGRID,SHOP_SLOT,
    //    SHOP_NEED_INFO,SHOP_DESC_WINDOW,SHOP_DESC_SUBINFO,
    //    
    //    //커마관련
    //    CUSTOMIZINGMENU,CUSTOMIZINGMENU_BUTTON,CUSTOMMENU_INFO,

    //    //Color-pallete
    //    PALEETE_WINDOW,PALLETE_AREA,PALEETE_CURSOR,PALLETE_DISPLAY,SLIDER,SLIDER_CURSOR,
    //    
    //    END};
    //enum class UI_CATEGORY { DEFAULT,HPBAR,STEMINARBAR, GUARDBAR, QUICKSLOT, PLAYERHUD_LEFT, PLAYERHUD_RIGHT, EQUIP,EQUIP_WEAPON,EQUIP_ARMOR,PASSIVE,SKILL,SHORTCUT,BLOODCODE, SKILLWINDOW,TOTAL,
    //   SKILL_INFO,WEAPON_INFO,PASSIVE_INFO,ARMOR_INFO,ITEM_INFO ,SHORTCUT_QUICK,FULL,CENTER,
    //    

    //    //씬슬롯의 점유하는레벨설정
    //    LEVEL_BASE,LEVEL_MAIN,LEVEL_CHURCH,LEVEL_SAMPLE,

    //    //WORLDUI
    //    FOCUSMENU_NPC,ITEM,


    //    //CUSTOMIZING TYPE
    //    SKIN,HAIR,EYE,EYEBROW,MAKEUP,CLOTHES,
    //    END };

    /*
     ,,, , , , , ,,,,,,, ,,
       ,,,, ,,,,
        

        //씬슬롯의 점유하는레벨설정
        ,,,,

        //WORLDUI
        ,,


        //CUSTOMIZING TYPE
        ,,,,,,
        END
    */
#define UI_BASE_LIST \
   X(NONE) X(PROGRESSBAR) X(SLOT) X(PLAYERHUD) X(SLOTPANEL) \
    X(BITMAPTEXT) X(PLAYERMENUWINDOW) X(INVENTORYWINDOW) X(SUBWINDOW) \
    X(TEXT) X(ITEMINFODESC) X(SHORTCUTMENU) X(POPUP_ITEMGET) \
    X(WINDOW_SHOP) X(SHOP_CATEGORY) X(SHOP_SLOT) \
    X(CUSTOMIZINGMENU) X(CUSTOMIZINGMENU_BUTTON) X(CUSTOMMENU_INFO) \
    X(PALEETE_WINDOW) X(PALLETE_AREA) X(PALLETE_DISPLAY) X(SLIDER) X(SLIDER_CURSOR) X(CUSTOMIZE_GRID) X(COLOR_SELECTOR) X(TEXTURE_SELECTOR) X(MESH_SELECTOR) X(ITEMGRID) \
    X(VALUESELECTOR)   X(WINDOW_LEVELUP) X(POPUP_LEVELUP)


    enum class UI_Base {
#define X(name) name,
        UI_BASE_LIST
#undef X
        END
    };

    // 3. String Vector 자동 생성
    inline vector<const char*> Get_UIBaseStr() {
        return {
    #define X(name) #name,
            UI_BASE_LIST
    #undef X
        };
    }

   

#define UI_CATEGORY_LIST \
        X(DEFAULT) X(HPBAR) X(GUARDBAR) X(QUICKSLOT) X(PLAYERHUD_LEFT) \
        X(PLAYERHUD_RIGHT) X(EQUIP) X(EQUIP_WEAPON) X(EQUIP_ARMOR) \
        X(PAlSSIVE) X(SKILL) X(SHORTCUT) X(BLOODCODE) \
        X(SKILLWINDOW) X(TOTAL) X(SKILL_INFO) \
        X(WEAPON_INFO) X(PASSIVE_INFO) X(ARMOR_INFO) X(ITEM_INFO) \
        X(SHORTCUT_QUICK) X(FULL) X(CENTER) X(LEVEL_BASE) X(LEVEL_MAIN) X(LEVEL_CHURCH) X(LEVEL_SAMPLE) \
        X(FOCUSMENU_NPC) X(ITEM) X(SKIN) X(HAIR) X(EYE) X(EYEBROW) X(MAKEUP) X(CLOTHES) X(FACEPAINT) \
        X(PLAYER_INFORMATION) X(WEAPON_INFORAMTION) X(SHIELD_INFORMATION) 

    enum class UI_CATEGORY {
#define X(name) name,
        UI_CATEGORY_LIST
#undef X
        END
    };

    inline vector<const char*> Get_CategoryStr()
    {
        return {
  #define X(name) #name,
          UI_CATEGORY_LIST
  #undef X

        };
    }

   // 2. 문자열을 넣으면 인덱스(순번)를 찾아주는 함수
  inline int Get_BaseUITypeIdx(string str)
  {
      // 정적 배열로 이름을 쫙 깔아두기 (컴파일 시점에 생성됨)
      static const char* UI_Names[] = {
  #define X(name) #name,
          UI_BASE_LIST
  #undef X
      };

      // 배열을 순회하면서 문자열이 일치하는 인덱스를 반환
      for (int i = 0; i < (int)std::size(UI_Names); ++i)
      {
          if (str == UI_Names[i])
              return i;
      }

      return 0; // 못 찾으면 NONE(0) 반환
  }

  //
  inline int Get_UITypeIdx(string str)
  {
      // 정적 배열로 이름을 쫙 깔아두기 (컴파일 시점에 생성됨)
      static const char* UI_Names[] = {
  #define X(name) #name,
          UI_CATEGORY_LIST
  #undef X
      };

      // 배열을 순회하면서 문자열이 일치하는 인덱스를 반환
      for (int i = 0; i < (int)std::size(UI_Names); ++i)
      {
          if (str == UI_Names[i])
              return i;
      }

      return 0; // 못 찾으면 NONE(0) 반환
  }


}

extern HWND g_hWnd;
extern HINSTANCE g_hInstance;
extern  HDC  g_HDC;


using namespace std;
using namespace UITool;