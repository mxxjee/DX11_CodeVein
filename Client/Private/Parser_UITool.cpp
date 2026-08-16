#include "Client_Define.h"
#include "Parser_UITool.h"

#include "GameInstance.h"
#include "Layer.h"
#include "GameObject.h"
#include "UIObject.h"


LEVEL Parser_UITool::m_iLoadLevel = LEVEL::UIDEV;

//////////////////////////////////////////////////////// 생성자, 소멸자 ////////////////////////////////////////////////////////
Client::Parser_UITool::Parser_UITool()
{
}

Client::Parser_UITool::~Parser_UITool()
{
}

/******************************************************* 생성자, 소멸자 *******************************************************/
//////////////////////////////////////////////////////// 초기화 함수 ////////////////////////////////////////////////////////
HRESULT Client::Parser_UITool::Initialize()
{
    m_pGameInstance = GameInstance::GetInstance();
   

    m_pGameInstance->Register_Pooling_Func([this](UIObjectInfo& pInfo)->UIObject*
        {
            UIObject* pRoot= Load_UI_Recursive_Pooling(pInfo, nullptr);
        
            return pRoot;
        });
    SAVEEVENT event;
    event.bSaveBinary = true;
    event.bSaveJson = true;
    event.eToolType = TOOLTYPE::UI_TOOL;

    _uint iEventHandle= m_pGameInstance->Subscribe<LOADEVENT>([this](const LOADEVENT& e)
        {
            if (e.eToolType != TOOLTYPE::UI_TOOL)
                return;

            Start_Load(e);

        });

    m_EventHandles.push_back(iEventHandle);


    //풀링객체 로드이벤트
    iEventHandle=m_pGameInstance->Subscribe<LOADEVENT_UIPOOLING>([this](const LOADEVENT_UIPOOLING& e)
        {
            if (e.eToolType != TOOLTYPE::UI_TOOL)
                return;

            Start_Load_Pooling(e);

        });
    m_EventHandles.push_back(iEventHandle);

    //해시키 만들기
    ProtoTypeBundle Bundle;
    {
        Bundle.PrototypeName = Proto_UIObject;
        Bundle.iType = _UINT(UI_Base::NONE);
        Bundle.TypeHash = _UINT(hash<string>{}("NONE"));
        
        m_BaseKeyHashs.emplace("NONE",Bundle );
    }

    {
        Bundle.PrototypeName = Proto_UIObject_PROGRESSBAR;
        Bundle.iType = _UINT(UI_Base::PROGRESSBAR);
        Bundle.TypeHash = _UINT(hash<string>{}("PROGRESSBAR"));
        Bundle.m_ChildProtos.push_back("HPBAR");
        Bundle.m_ChildProtos.push_back("STAMINARBAR");
        Bundle.m_ChildProtos.push_back("GUARDBAR");

        m_BaseKeyHashs.emplace("PROGRESSBAR", Bundle);
    }

    {
        Bundle.m_ChildProtos.clear();
        Bundle.PrototypeName = Proto_UIObject_SLOT;
        Bundle.iType = _UINT(UI_Base::SLOT);
        Bundle.TypeHash = _UINT(hash<string>{}("SLOT"));

        Bundle.m_ChildProtos.push_back("QUICKSLOT");
        Bundle.m_ChildProtos.push_back("SHORTCUT_QUICK");

        m_BaseKeyHashs.emplace("SLOT", Bundle);
    }

    {
        Bundle.m_ChildProtos.clear();
        Bundle.PrototypeName = Proto_UIObject_PLAYERHUD;
        Bundle.iType = _UINT(UI_Base::PLAYERHUD);
        Bundle.TypeHash = _UINT(hash<string>{}("PLAYERHUD"));

        Bundle.m_ChildProtos.push_back("PLAYERHUD_LEFT");

        m_BaseKeyHashs.emplace("PLAYERHUD", Bundle);
    }

    {
        Bundle.m_ChildProtos.clear();
        Bundle.PrototypeName = Proto_UIObject_SLOTGRID;
        Bundle.iType = _UINT(UI_Base::SLOTPANEL);
        Bundle.TypeHash = _UINT(hash<string>{}("SLOTPANEL"));

        m_BaseKeyHashs.emplace("SLOTPANEL", Bundle);
    }


    {
        Bundle.m_ChildProtos.clear();
        Bundle.PrototypeName = Proto_UIObject_PLAYERHUD;
        Bundle.iType = _UINT(UI_Base::NONE);
        Bundle.TypeHash = _UINT(hash<string>{}("NONE"));

        m_BaseKeyHashs.emplace("NONE", Bundle);
    }


    {
        Bundle.m_ChildProtos.clear();
        Bundle.PrototypeName = Proto_UIObject_BITMAPTEXT;
        Bundle.iType = _UINT(UI_Base::BITMAPTEXT);
        Bundle.TypeHash = _UINT(hash<string>{}("BITMAPTEXT"));

        m_BaseKeyHashs.emplace("BITMAPTEXT", Bundle);
    }

    {
        Bundle.m_ChildProtos.clear();
        Bundle.PrototypeName = Proto_UIObject_PLAYERMENUWINDOW;
        Bundle.iType = _UINT(UI_Base::PLAYERMENUWINDOW);
        Bundle.TypeHash = _UINT(hash<string>{}("PLAYERMENUWINDOW"));

        m_BaseKeyHashs.emplace("PLAYERMENUWINDOW", Bundle);
    }

    {
        Bundle.m_ChildProtos.clear();
        Bundle.PrototypeName = Proto_UIObject_INVENTORYWINDOW;
        Bundle.iType = _UINT(UI_Base::INVENTORYWINDOW);
        Bundle.TypeHash = _UINT(hash<string>{}("INVENTORYWINDOW"));

        m_BaseKeyHashs.emplace("INVENTORYWINDOW", Bundle);
    }

    {
        Bundle.m_ChildProtos.clear();
        Bundle.PrototypeName = Proto_UIObject_SUBWINDOW;
        Bundle.iType = _UINT(UI_Base::SUBWINDOW);
        Bundle.TypeHash = _UINT(hash<string>{}("SUBWINDOW"));

        Bundle.m_ChildProtos.push_back("BLOODCODE");

        m_BaseKeyHashs.emplace("SUBWINDOW", Bundle);
    }

    {
        Bundle.m_ChildProtos.clear();
        Bundle.PrototypeName = Proto_UIObject_TEXT;
        Bundle.iType = _UINT(UI_Base::TEXT);
        Bundle.TypeHash = _UINT(hash<string>{}("TEXT"));

        m_BaseKeyHashs.emplace("TEXT", Bundle);
    }

    {
        Bundle.m_ChildProtos.clear();
        Bundle.PrototypeName = Proto_UIObject_SUBDESC;
        Bundle.iType = _UINT(UI_Base::ITEMINFODESC);
        Bundle.TypeHash = _UINT(hash<string>{}("ITEMINFODESC"));

        m_BaseKeyHashs.emplace("ITEMINFODESC", Bundle);
    }

    {
        Bundle.m_ChildProtos.clear();
        Bundle.PrototypeName = Proto_UIObject_SHORTCUTMENU;
        Bundle.iType = _UINT(UI_Base::SHORTCUTMENU);
        Bundle.TypeHash = _UINT(hash<string>{}("SHORTCUTMENU"));

        m_BaseKeyHashs.emplace("SHORTCUTMENU", Bundle);

    }

    {
        Bundle.m_ChildProtos.clear();
        Bundle.PrototypeName = Proto_UIObject_PopUp_ItemGet;
        Bundle.iType = _UINT(UI_Base::POPUP_ITEMGET);
        Bundle.TypeHash = _UINT(hash<string>{}("POPUP_ITEMGET"));

        m_BaseKeyHashs.emplace("POPUP_ITEMGET", Bundle);

    }

    {
        Bundle.m_ChildProtos.clear();
        Bundle.PrototypeName = Proto_UIObject_PopUp_Interaction;
        Bundle.iType = _UINT(UI_Base::POPUP_INTERACTION);
        Bundle.TypeHash = _UINT(hash<string>{}("POPUP_INTERACTION"));

        Bundle.m_ChildProtos.push_back("MAPTITLE");


        m_BaseKeyHashs.emplace("POPUP_INTERACTION", Bundle);

    }


    {
        Bundle.m_ChildProtos.clear();
        Bundle.PrototypeName = Proto_UIObject_BossName;
        Bundle.iType = _UINT(UI_Base::BOSSNAME);
        Bundle.TypeHash = _UINT(hash<string>{}("BOSSNAME"));

        m_BaseKeyHashs.emplace("BOSSNAME", Bundle);

    }


    {
        Bundle.m_ChildProtos.clear();
        Bundle.PrototypeName = Proto_UIObject_BlurUI;
        Bundle.iType = _UINT(UI_Base::BLURUI);
        Bundle.TypeHash = _UINT(hash<string>{}("BLURUI"));

        m_BaseKeyHashs.emplace("BLURUI", Bundle);

    }


    {
        Bundle.m_ChildProtos.clear();
        Bundle.PrototypeName = Proto_UIObject_MINIMAP;
        Bundle.iType = _UINT(UI_Base::MINIMAP);
        Bundle.TypeHash = _UINT(hash<string>{}("MINIMAP"));

        m_BaseKeyHashs.emplace("MINIMAP", Bundle);

    }


    {
        Bundle.m_ChildProtos.clear();
        Bundle.PrototypeName = Proto_UIObject_MonsterStatus;
        Bundle.iType = _UINT(UI_Base::MONSTERSTATUS);
        Bundle.TypeHash = _UINT(hash<string>{}("MONSTERSTATUSBAR"));

        m_BaseKeyHashs.emplace("MONSTERSTATUSBAR", Bundle);

    }

    {
        Bundle.m_ChildProtos.clear();
        Bundle.PrototypeName = Proto_UIObject_LoadingCube;
        Bundle.iType = _UINT(UI_Base::LOADING_CUBE);
        Bundle.TypeHash = _UINT(hash<string>{}("LOADING_CUBE"));

        m_BaseKeyHashs.emplace("LOADING_CUBE", Bundle);

    }

    {
        Bundle.m_ChildProtos.clear();
        Bundle.PrototypeName = Proto_UIObject_FadeScreen;
        Bundle.iType = _UINT(UI_Base::FADESCREEN);
        Bundle.TypeHash = _UINT(hash<string>{}("FADESCREEN"));

        m_BaseKeyHashs.emplace("FADESCREEN", Bundle);

    }

    {
        Bundle.m_ChildProtos.clear();
        Bundle.PrototypeName = Proto_UIObject_LoadingScreen;
        Bundle.iType = _UINT(UI_Base::LOADING_BACK);
        Bundle.TypeHash = _UINT(hash<string>{}("LOADING_BACK"));

        m_BaseKeyHashs.emplace("LOADING_BACK", Bundle);

    }

    {
        Bundle.m_ChildProtos.clear();
        Bundle.PrototypeName = Proto_UIObject_SceneSelectMenu;
        Bundle.iType = _UINT(UI_Base::SCENESELECTMENU);
        Bundle.TypeHash = _UINT(hash<string>{}("SCENESELECTMENU"));

        m_BaseKeyHashs.emplace("SCENESELECTMENU", Bundle);

    }


    {
        Bundle.m_ChildProtos.clear();
        Bundle.PrototypeName = Proto_UIObject_SceneSlot;
        Bundle.iType = _UINT(UI_Base::SCENESLOT);
        Bundle.TypeHash = _UINT(hash<string>{}("SCENESLOT"));

        m_BaseKeyHashs.emplace("SCENESLOT", Bundle);

    }

    {
        Bundle.m_ChildProtos.clear();
        Bundle.PrototypeName = Proto_UIObject_Haze;
        Bundle.iType = _UINT(UI_Base::HAZE);
        Bundle.TypeHash = _UINT(hash<string>{}("HAZE"));

        m_BaseKeyHashs.emplace("HAZE", Bundle);

    }
    {
        Bundle.m_ChildProtos.clear();
        Bundle.PrototypeName = Proto_UIObject_NpcDialogue;
        Bundle.iType = _UINT(UI_Base::NPC_DIALOGUE);
        Bundle.TypeHash = _UINT(hash<string>{}("NPC_DIALOGUE"));

        m_BaseKeyHashs.emplace("NPC_DIALOGUE", Bundle);

    }

    {
        Bundle.m_ChildProtos.clear();
        Bundle.PrototypeName = Proto_UIObject_NpcDialogue;
        Bundle.iType = _UINT(UI_Base::NPC_DIALOGUE);
        Bundle.TypeHash = _UINT(hash<string>{}("NPC_DIALOGUE"));

        m_BaseKeyHashs.emplace("NPC_DIALOGUE", Bundle);

    }





    ////////////////////////////////////////////////////////////////////
    {
        Bundle.m_ChildProtos.clear();
        Bundle.PrototypeName = Proto_UIObject_TitleWindow;
        Bundle.iType = _UINT(UI_Base::LOADING_BACK);
        Bundle.TypeHash = _UINT(hash<string>{}("TITLEWINDOW"));

        m_BaseKeyHashs.emplace("TITLEWINDOW", Bundle);

    }

    {
        Bundle.m_ChildProtos.clear();
        Bundle.PrototypeName = Proto_UIObject_Selector;
        Bundle.iType = _UINT(UI_Base::SELECTOR);
        Bundle.TypeHash = _UINT(hash<string>{}("SELECTOR"));

        m_BaseKeyHashs.emplace("SELECTOR", Bundle);

    }

    {
        Bundle.m_ChildProtos.clear();
        Bundle.PrototypeName = Proto_UIObject_GlowButton;
        Bundle.iType = _UINT(UI_Base::GLOWBUTTON);
        Bundle.TypeHash = _UINT(hash<string>{}("GLOWBUTTON"));

        m_BaseKeyHashs.emplace("GLOWBUTTON", Bundle);

    }


    {
        Bundle.m_ChildProtos.clear();
        Bundle.PrototypeName = Proto_UIObject_LockOn;
        Bundle.iType = _UINT(UI_Base::LOCKON);
        Bundle.TypeHash = _UINT(hash<string>{}("LOCKON"));

        m_BaseKeyHashs.emplace("LOCKON", Bundle);

    }

    {
        Bundle.m_ChildProtos.clear();
        Bundle.PrototypeName = Proto_UIObject_FocusMenu;
        Bundle.iType = _UINT(UI_Base::FOCUSMENU);
        Bundle.TypeHash = _UINT(hash<string>{}("FOCUSMENU"));


        Bundle.m_ChildProtos.push_back("FOCUSMENU_NPC");
        m_BaseKeyHashs.emplace("FOCUSMENU", Bundle);

    }

    {
        Bundle.m_ChildProtos.clear();
        Bundle.PrototypeName = Proto_UIObject_SavePoint_Slot;
        Bundle.iType = _UINT(UI_Base::SAVEPOINT_SLOT);
        Bundle.TypeHash = _UINT(hash<string>{}("SAVEPOINT_SLOT"));

        m_BaseKeyHashs.emplace("SAVEPOINT_SLOT", Bundle);

    }

    {
        Bundle.m_ChildProtos.clear();
        Bundle.PrototypeName = Proto_UIObject_ManaInfo;
        Bundle.iType = _UINT(UI_Base::MANAINFO);
        Bundle.TypeHash = _UINT(hash<string>{}("MANAINFO"));

        m_BaseKeyHashs.emplace("MANAINFO", Bundle);

    }

    {
        Bundle.m_ChildProtos.clear();
        Bundle.PrototypeName = Proto_UIObject_NPCMenu;
        Bundle.iType = _UINT(UI_Base::NPC_CHOICEMENU);
        Bundle.TypeHash = _UINT(hash<string>{}("NPC_CHOICEMENU"));

        m_BaseKeyHashs.emplace("NPC_CHOICEMENU", Bundle);

    }

    {
        Bundle.m_ChildProtos.clear();
        Bundle.PrototypeName = Proto_UIObject_LevelUpWindow;
        Bundle.iType = _UINT(UI_Base::WINDOW_LEVELUP);
        Bundle.TypeHash = _UINT(hash<string>{}("WINDOW_LEVELUP"));

        m_BaseKeyHashs.emplace("WINDOW_LEVELUP", Bundle);

    }

    ///////////////Shop//////////////////
    {
        Bundle.m_ChildProtos.clear();
        Bundle.PrototypeName = Proto_UIObject_WINDOW_SHOP;
        Bundle.iType = _UINT(UI_Base::WINDOW_SHOP);
        Bundle.TypeHash = _UINT(hash<string>{}("WINDOW_SHOP"));

        m_BaseKeyHashs.emplace("WINDOW_SHOP", Bundle);

    }

    {
        Bundle.m_ChildProtos.clear();
        Bundle.PrototypeName = Proto_UIObject_SHOP_CATEGORY;
        Bundle.iType = _UINT(UI_Base::SHOP_CATEGORY);
        Bundle.TypeHash = _UINT(hash<string>{}("SHOP_CATEGORY"));

        m_BaseKeyHashs.emplace("SHOP_CATEGORY", Bundle);

    }

    {
        Bundle.m_ChildProtos.clear();
        Bundle.PrototypeName = Proto_UIObject_SHOP_SLOTGRID;
        Bundle.iType = _UINT(UI_Base::SHOP_SLOTGRID);
        Bundle.TypeHash = _UINT(hash<string>{}("SHOP_SLOTGRID"));

        m_BaseKeyHashs.emplace("SHOP_SLOTGRID", Bundle);

    }

    {
        Bundle.m_ChildProtos.clear();
        Bundle.PrototypeName = Proto_UIObject_SHOP_SLOT;
        Bundle.iType = _UINT(UI_Base::SHOP_SLOT);
        Bundle.TypeHash = _UINT(hash<string>{}("SHOP_SLOT"));

        m_BaseKeyHashs.emplace("SHOP_SLOT", Bundle);

    }

    ///subwindow
    {
        Bundle.m_ChildProtos.clear();
        Bundle.PrototypeName = Proto_UIObject_SHOP_NEEDINFO;
        Bundle.iType = _UINT(UI_Base::SHOP_NEED_INFO);
        Bundle.TypeHash = _UINT(hash<string>{}("SHOP_NEED_INFO"));

        m_BaseKeyHashs.emplace("SHOP_NEED_INFO", Bundle);

    }


    {
        Bundle.m_ChildProtos.clear();
        Bundle.PrototypeName = Proto_UIObject_SHOP_DESC_WINDOW;
        Bundle.iType = _UINT(UI_Base::SHOP_DESC_WINDOW);
        Bundle.TypeHash = _UINT(hash<string>{}("SHOP_DESC_WINDOW"));

        m_BaseKeyHashs.emplace("SHOP_DESC_WINDOW", Bundle);

    }


    {
        Bundle.m_ChildProtos.clear();
        Bundle.PrototypeName = Proto_UIObject_SHOP_DESC_SUBINFO;
        Bundle.iType = _UINT(UI_Base::SHOP_DESC_SUBINFO);
        Bundle.TypeHash = _UINT(hash<string>{}("SHOP_DESC_SUBINFO"));

        m_BaseKeyHashs.emplace("SHOP_DESC_SUBINFO", Bundle);

    }

    {
        Bundle.m_ChildProtos.clear();
        Bundle.PrototypeName = Proto_UIObject_BOSSDEAD;
        Bundle.iType = _UINT(UI_Base::BOSSDEAD);
        Bundle.TypeHash = _UINT(hash<string>{}("BOSSDEAD"));

        m_BaseKeyHashs.emplace("BOSSDEAD", Bundle);

    }

    {
        Bundle.m_ChildProtos.clear();
        Bundle.PrototypeName = Proto_UIObject_Ending;
        Bundle.iType = _UINT(UI_Base::ENDING);
        Bundle.m_iLevel= LEVEL::CHURCH;
        Bundle.TypeHash = _UINT(hash<string>{}("ENDING"));

        m_BaseKeyHashs.emplace("ENDING", Bundle);

    }
    ///////////////////////////////////////////////////////////////////

 {
     Bundle.m_ChildProtos.clear();
     Bundle.PrototypeName = Proto_UIObject_VALUESELECTOR;
     Bundle.iType = _UINT(UI_Base::VALUESELECTOR);
     Bundle.TypeHash = _UINT(hash<string>{}("VALUESELECTOR"));
     Bundle.m_iLevel = LEVEL::CUSTOMIZE;
     m_BaseKeyHashs.emplace("VALUESELECTOR", Bundle);

 }
 ////////////customizing//////////////////
 {
     Bundle.m_ChildProtos.clear();
     Bundle.PrototypeName = Proto_UIObject_CUSTOMIZINGMENU;
     Bundle.iType = _UINT(UI_Base::CUSTOMIZINGMENU);
     Bundle.m_iLevel = LEVEL::CUSTOMIZE;
     Bundle.TypeHash = _UINT(hash<string>{}("CUSTOMIZINGMENU"));

     m_BaseKeyHashs.emplace("CUSTOMIZINGMENU", Bundle);

 }

 {
     Bundle.m_ChildProtos.clear();
     Bundle.PrototypeName = Proto_UIObject_CUSTOMIZINGMENU_BUTTON;
     Bundle.iType = _UINT(UI_Base::CUSTOMIZINGMENU_BUTTON);
     Bundle.m_iLevel = LEVEL::CUSTOMIZE;
     Bundle.TypeHash = _UINT(hash<string>{}("CUSTOMIZINGMENU_BUTTON"));

     m_BaseKeyHashs.emplace("CUSTOMIZINGMENU_BUTTON", Bundle);

 }

 {
     Bundle.m_ChildProtos.clear();
     Bundle.PrototypeName = Proto_UIObject_CUSTOMMENU_INFO;
     Bundle.iType = _UINT(UI_Base::CUSTOMMENU_INFO);
     Bundle.TypeHash = _UINT(hash<string>{}("CUSTOMMENU_INFO"));
     Bundle.m_iLevel = LEVEL::CUSTOMIZE;
     m_BaseKeyHashs.emplace("CUSTOMMENU_INFO", Bundle);

 }


 {
     Bundle.m_ChildProtos.clear();
     Bundle.PrototypeName = Proto_UIObject_PALEETE_WINDOW;
     Bundle.iType = _UINT(UI_Base::PALEETE_WINDOW);
     Bundle.TypeHash = _UINT(hash<string>{}("PALEETE_WINDOW"));
     Bundle.m_iLevel = LEVEL::CUSTOMIZE;
     m_BaseKeyHashs.emplace("PALEETE_WINDOW", Bundle);

 }

 {
     Bundle.m_ChildProtos.clear();
     Bundle.PrototypeName = Proto_UIObject_PALLETE_AREA;
     Bundle.iType = _UINT(UI_Base::PALLETE_AREA);
     Bundle.TypeHash = _UINT(hash<string>{}("PALLETE_AREA"));
     Bundle.m_iLevel = LEVEL::CUSTOMIZE;
     m_BaseKeyHashs.emplace("PALLETE_AREA", Bundle);

 }

 {
     Bundle.m_ChildProtos.clear();
     Bundle.PrototypeName = Proto_UIObject_PALLETE_DISPLAY;
     Bundle.iType = _UINT(UI_Base::PALLETE_DISPLAY);
     Bundle.TypeHash = _UINT(hash<string>{}("PALLETE_DISPLAY"));
     Bundle.m_iLevel = LEVEL::CUSTOMIZE;
     m_BaseKeyHashs.emplace("PALLETE_DISPLAY", Bundle);

 }

 {
     Bundle.m_ChildProtos.clear();
     Bundle.PrototypeName = Proto_UIObject_VERTICAL_SLIDER;
     Bundle.iType = _UINT(UI_Base::SLIDER);
     Bundle.TypeHash = _UINT(hash<string>{}("SLIDER"));
     Bundle.m_iLevel = LEVEL::CUSTOMIZE;
     m_BaseKeyHashs.emplace("SLIDER", Bundle);

 }

 {
     Bundle.m_ChildProtos.clear();
     Bundle.PrototypeName = Proto_UIObject_CUSTOMIZE_GRID;
     Bundle.iType = _UINT(UI_Base::CUSTOMIZE_GRID);
     Bundle.TypeHash = _UINT(hash<string>{}("CUSTOMIZE_GRID"));
     Bundle.m_iLevel = LEVEL::CUSTOMIZE;
     m_BaseKeyHashs.emplace("CUSTOMIZE_GRID", Bundle);

 }

 {
     Bundle.m_ChildProtos.clear();
     Bundle.PrototypeName = Proto_UIObject_COLOR_SELECTOR;
     Bundle.iType = _UINT(UI_Base::COLOR_SELECTOR);
     Bundle.TypeHash = _UINT(hash<string>{}("COLOR_SELECTOR"));
     Bundle.m_iLevel = LEVEL::CUSTOMIZE;
     m_BaseKeyHashs.emplace("COLOR_SELECTOR", Bundle);

 }

 {
     Bundle.m_ChildProtos.clear();
     Bundle.PrototypeName = Proto_UIObject_TEXTURE_SELECTOR;
     Bundle.iType = _UINT(UI_Base::TEXTURE_SELECTOR);
     Bundle.TypeHash = _UINT(hash<string>{}("TEXTURE_SELECTOR"));
     Bundle.m_iLevel = LEVEL::CUSTOMIZE;
     m_BaseKeyHashs.emplace("TEXTURE_SELECTOR", Bundle);

 }

 {
     Bundle.m_ChildProtos.clear();
     Bundle.PrototypeName = Proto_UIObject_MESH_SELECTOR;
     Bundle.iType = _UINT(UI_Base::COLOR_SELECTOR);
     Bundle.TypeHash = _UINT(hash<string>{}("MESH_SELECTOR"));
     Bundle.m_iLevel = LEVEL::CUSTOMIZE;
     m_BaseKeyHashs.emplace("MESH_SELECTOR", Bundle);

 }

 {
     Bundle.m_ChildProtos.clear();
     Bundle.PrototypeName = Proto_UIObject_ITEMGRID;
     Bundle.iType = _UINT(UI_Base::ITEMGRID);
     Bundle.TypeHash = _UINT(hash<string>{}("ITEMGRID"));
     Bundle.m_iLevel = LEVEL::CUSTOMIZE;
     m_BaseKeyHashs.emplace("ITEMGRID", Bundle);

 }
    //////////////////////////TypeHashKey///////////////////////////
   
     Bundle = {};
    {
        Bundle.m_ChildProtos.clear();
        Bundle.PrototypeName = Proto_UIObject_HPBAR;
        Bundle.iType = _UINT(UI_CATEGORY::HPBAR);
        m_UITypeKeyHashs.emplace("HPBAR", Bundle);
    }


    {
        Bundle.m_ChildProtos.clear();
        Bundle.PrototypeName = Proto_UIObject_STAMINABAR;
        Bundle.iType = _UINT(UI_CATEGORY::STAMINARBAR);
        m_UITypeKeyHashs.emplace("STAMINARBAR", Bundle);

    }

    {
        Bundle.m_ChildProtos.clear();
        Bundle.PrototypeName = Proto_UIObject_GUARDBAR;
        Bundle.iType = _UINT(UI_CATEGORY::GUARDBAR);
        m_UITypeKeyHashs.emplace("GUARDBAR", Bundle);

    }




    {
        Bundle.m_ChildProtos.clear();
        Bundle.PrototypeName = Proto_UIObject_PLAYERHUD_LEFT;
        Bundle.iType = _UINT(UI_CATEGORY::GUARDBAR);
        m_UITypeKeyHashs.emplace("PLAYERHUD_LEFT", Bundle);

    }


    {
        Bundle.m_ChildProtos.clear();
        Bundle.PrototypeName = Proto_UIObject_QUICKSLOT;
        Bundle.iType = _UINT(UI_CATEGORY::QUICKSLOT);
        m_UITypeKeyHashs.emplace("QUICKSLOT", Bundle);

    }

    {
        Bundle.m_ChildProtos.clear();
        Bundle.PrototypeName = Proto_UIObject_SHORTCUTQUICKSLOT;
        Bundle.iType = _UINT(UI_CATEGORY::SHORTCUT_QUICK);
        m_UITypeKeyHashs.emplace("SHORTCUT_QUICK", Bundle);

    }


    {
        Bundle.m_ChildProtos.clear();
        Bundle.PrototypeName = Proto_UIObject_SUBWINDOW_BLOODCODE;
        Bundle.iType = _UINT(UI_CATEGORY::BLOODCODE);
        m_UITypeKeyHashs.emplace("BLOODCODE", Bundle);

    }


    {
        Bundle.m_ChildProtos.clear();
        Bundle.PrototypeName = Proto_UIObject_PopUp_MapTitle;
        Bundle.iType = _UINT(UI_CATEGORY::MAPTITLE);
        m_UITypeKeyHashs.emplace("MAPTITLE", Bundle);

    }

    {
        Bundle.m_ChildProtos.clear();
        Bundle.PrototypeName = Proto_UIObject_FocusMenu_NPC;
        Bundle.iType = _UINT(UI_CATEGORY::FOCUSMENU_NPC);
        m_UITypeKeyHashs.emplace("FOCUSMENU_NPC", Bundle);
    }

    return S_OK;
}
/******************************************************* 초기화 함수 *******************************************************/


//////////////////////////////////////////////////////// 로드 함수 ////////////////////////////////////////////////////////
void Client::Parser_UITool::Start_Load(const LOADEVENT& _event)
{
    COUT("[Sample_Pars_UItool] 로드 시작");

    vecData.clear();
    // JSON 파일에서 데이터 로드
    if (m_pGameInstance->LoadFromJson(_event.m_Path, *this))
    {
        COUT("[Sample_Pars_UItool] JSON 로드 성공 - " + to_string(vecData.size()) + "개 오브젝트");

        // 오브젝트 생성
        Spawn_Objects(_event);
    }
    else
    {
        COUT("[Sample_Pars_UItool] JSON 로드 실패!");
    }

    COUT("[Sample_Pars_UItool] 로드 완료");
}

void Client::Parser_UITool::Spawn_Objects(const LOADEVENT& _event)
{
    COUT("[Sample_Pars_UItool] UI 계층 구조 생성 시작");

    if (!_event.m_bPersistent)
    {
        //최상위 Root 의 데이터만들어있음
        for (auto& info : vecData)
        {
            //root이므로 parent=nullptr
            UIObject* pRoot = Load_UI_Recursive(_event,info, nullptr);
            COUT("[성공] 오브젝트 생성: " + info.strObjectKey);
        }
    }
   
    else
    {
        //최상위 Root 의 데이터만들어있음
        for (auto& info : vecData)
        {
            //root이므로 parent=nullptr
            UIObject* pRoot = Load_UI_Recursive_Persistent(info, nullptr);
            if (pRoot)
                m_pGameInstance->Register_PersistentUI(pRoot);

            COUT("[성공] 오브젝트 생성: " + info.strObjectKey);
        }
    }

    COUT("[Sample_Pars_UItool] 오브젝트 생성 완료 - " + to_string(vecData.size()) + "개");
}

void Parser_UITool::Start_Load_Prefab(const LOADEVENT& _event)
{
    COUT("[Sample_Pars_UItool] 로드 시작");

    // JSON 파일에서 데이터 로드
    if (m_pGameInstance->LoadFromJson(_event.m_Path, *this))
    {
        COUT("[Sample_Pars_UItool] JSON 로드 성공 - " + to_string(vecData.size()) + "개 오브젝트");

        // 오브젝트 생성
        Spawn_Objects(_event);
    }
    else
    {
        COUT("[Sample_Pars_UItool] JSON 로드 실패!");
    }

    COUT("[Sample_Pars_UItool] 로드 완료");
}
void Client::Parser_UITool::Start_Load_Pooling(const LOADEVENT_UIPOOLING& e)
{
    COUT("[Sample_Pars_UItool] 로드 시작");

    vecData.clear();
    // JSON 파일에서 데이터 로드
    if (m_pGameInstance->LoadFromJson(e.m_Path, *this))
    {
        COUT("[Sample_Pars_UItool] JSON 로드 성공 - " + to_string(vecData.size()) + "개 오브젝트");

        // 오브젝트 생성 (생성시 어떻게 풀링에추가할걱ㄴ지 넘겨줌)
        Spawn_PoolingObjects(e);
    }
    else
    {
        COUT("[Sample_Pars_UItool] JSON 로드 실패!");
    }

    COUT("[Sample_Pars_UItool] 로드 완료");
}
void Client::Parser_UITool::Spawn_PoolingObjects(const LOADEVENT_UIPOOLING& e)
{
    COUT("[Sample_Pars_UItool] UI 계층 구조 생성 시작");

    if (e.m_iSize == 0)
        return;

    //최상위 Root 의 데이터만들어있음
    for (auto& info : vecData)
    {
       //원본등록
        m_pGameInstance->Register_Factory(_UINT(e.ePoolingType), info);

      
       m_pGameInstance->Add_PoolObject(_UINT(e.ePoolingType), e.m_iSize);

        

        COUT("[성공/pooling] 오브젝트 생성: " + info.strObjectKey);
    }

    COUT("[Sample_Pars_UItool]풀링 오브젝트 생성 완료 - " + to_string(vecData.size()) + "개");
}
void Client::Parser_UITool::UnSubcribe_Evnets()
{
    for (auto idx : m_EventHandles)
        m_pGameInstance->UnsubScribe(idx);
}
UIObject* Client::Parser_UITool::Load_UI_Recursive_Pooling(UIObjectInfo& info, UIObject* pParent)
{
    //프로토타입으로 객체생성
    UIObject* pSelf = nullptr;

    UIObject::UIOBJECT_DESC Desc;
    Desc.wstrName = stringToWstring(info.strObjectKey);

    Desc.fX = info.m_Local.m_fX;
    Desc.fY = info.m_Local.m_fY;
    Desc.fCX = info.m_Local.m_fSizeX;
    Desc.fCY = info.m_Local.m_fSizeY;
    Desc.iZOrder = info.iZOrder;
    Desc.fAlpha = info.m_Local.m_fAlpha;
    if (info.strParentName != "")
        Desc.pParent = pParent;


    Desc.UIBaseType = info.UIBaseType;
    Desc.UIType = info.UIType;

    Desc.bIsActive = info.bActive;
    Desc.bIsVisible = info.bVisible;

    Desc.iShaderNumber = info.iShaderPass;
    Desc.wstrShaderName = stringToWstring(info.ShaderComName);

    Desc.bAddUIManager = false;

    //나머지는 이후에세팅
    GameObject* pOut = nullptr;
    LEVEL ProtoLevel;
    wstring strProtoName = Get_PrototypeName(info.UIBaseType, info.UIType,&ProtoLevel);
    
	Base* pBase = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, _UINT(LEVEL::STATIC), strProtoName, &Desc);
	if (pBase)
		pOut = dynamic_cast<GameObject*>(pBase);

   
    if (pOut)
    {
        pSelf = dynamic_cast<UIObject*>(pOut);
        if (pSelf)
            pSelf->Apply_Data_From_Info(info);


    }

    if (!pSelf)
        return nullptr;


    for (auto& childInfo : info.m_Children)
    {
        UIObject* pChild = Load_UI_Recursive_Pooling(childInfo, pSelf);

    }

    //각 클래스 자식 캐싱/ 등 추후처리작업
    pSelf->After_ApplyData();
    //부모세팅
    return pSelf;
}
UIObject* Client::Parser_UITool::Load_UI_Recursive_Persistent(UIObjectInfo& info, UIObject* pParent)
{
    //프로토타입으로 객체생성
    UIObject* pSelf = nullptr;

    UIObject::UIOBJECT_DESC Desc;
    Desc.wstrName = stringToWstring(info.strObjectKey);

    Desc.fX = info.m_Local.m_fX;
    Desc.fY = info.m_Local.m_fY;
    Desc.fCX = info.m_Local.m_fSizeX;
    Desc.fCY = info.m_Local.m_fSizeY;
    Desc.iZOrder = info.iZOrder;
    Desc.fAlpha = info.m_Local.m_fAlpha;
    if (info.strParentName != "")
        Desc.pParent = pParent;


    Desc.UIBaseType = info.UIBaseType;
    Desc.UIType = info.UIType;

    Desc.bIsActive = info.bActive;
    Desc.bIsVisible = info.bVisible;

    Desc.iShaderNumber = info.iShaderPass;
    Desc.wstrShaderName = stringToWstring(info.ShaderComName);

    Desc.bAddUIManager = false;

    //나머지는 이후에세팅
    GameObject* pOut = nullptr;
    LEVEL ProtoLevel;
    wstring strProtoName = Get_PrototypeName(info.UIBaseType, info.UIType,&ProtoLevel);

    Base* pBase = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, _UINT(LEVEL::STATIC), strProtoName, &Desc);
    if (pBase)
        pOut = dynamic_cast<GameObject*>(pBase);


    if (pOut)
    {
        pSelf = dynamic_cast<UIObject*>(pOut);
        if (pSelf)
            pSelf->Apply_Data_From_Info(info);


    }

    if (!pSelf)
        return nullptr;


    for (auto& childInfo : info.m_Children)
    {
        UIObject* pChild = Load_UI_Recursive_Persistent(childInfo, pSelf);

    }

    //각 클래스 자식 캐싱/ 등 추후처리작업
    pSelf->After_ApplyData();
    //부모세팅
    return pSelf;
}
UIObject* Parser_UITool::Load_UI_Recursive(const LOADEVENT& _event,UIObjectInfo& info, UIObject* pParent)
{
    //프로토타입으로 객체생성
    UIObject* pSelf = nullptr;

    UIObject::UIOBJECT_DESC Desc;
    Desc.wstrName = stringToWstring(info.strObjectKey);

    Desc.fX = info.m_Local.m_fX;
    Desc.fY = info.m_Local.m_fY;
    Desc.fCX = info.m_Local.m_fSizeX;
    Desc.fCY = info.m_Local.m_fSizeY;
    Desc.iZOrder = info.iZOrder;
    Desc.fAlpha = info.m_Local.m_fAlpha;
    if (info.strParentName != "")
        Desc.pParent = pParent;


    Desc.UIBaseType = info.UIBaseType;
    Desc.UIType = info.UIType;

    Desc.bIsActive = info.bActive;
    Desc.bIsVisible = info.bVisible;

    Desc.iShaderNumber = info.iShaderPass;
    Desc.wstrShaderName = stringToWstring(info.ShaderComName);

    //나머지는 이후에세팅
    GameObject* pOut = nullptr;
    LEVEL ProtoLevel;
    wstring strProtoName = Get_PrototypeName(info.UIBaseType,info.UIType,&ProtoLevel);
    if (Desc.pParent)//레이어에등록X
    {
        Base* pBase = m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, _UINT(ProtoLevel), strProtoName, &Desc);
        if (pBase)
            pOut = dynamic_cast<GameObject*>(pBase);
    }
    //레이어에등록
    else
        m_pGameInstance->Add_GameObject_To_Layer(_UINT(ProtoLevel), strProtoName, _UINT(m_iLoadLevel), Layer_UIs, &pOut, &Desc);

    if (pOut)
    {
        pSelf = dynamic_cast<UIObject*>(pOut);
        if (pSelf)
            pSelf->Apply_Data_From_Info(info);


    }

    if (!pSelf)
        return nullptr;


    for (auto& childInfo : info.m_Children)
    {
        UIObject* pChild = Load_UI_Recursive(_event,childInfo, pSelf);

        //    pSelf->Add_Child_OnLoad(pChild);

    }

    //각 클래스 자식 캐싱/ 등 추후처리작업
    pSelf->After_ApplyData();
    //부모세팅
    return pSelf;
}
/******************************************************* 로드 함수 *******************************************************/




wstring Client::Parser_UITool::Get_PrototypeName(string BaseType, string UIType,LEVEL* pOut)
{
    if (BaseType == "PALLETE_CURSOR")
        int A = 10;

    *pOut = LEVEL::STATIC;
    
 	auto BaseIter = m_BaseKeyHashs.find(BaseType);
    if (BaseIter != m_BaseKeyHashs.end())
    {
        if (UIType == "DEFAULT")
        {
            *pOut = BaseIter->second.m_iLevel;
            return BaseIter->second.PrototypeName;
        }

        else
        {
            vector<string> m_vecChildProto = BaseIter->second.m_ChildProtos;
            for (auto& proto : m_vecChildProto)
            {
                if (proto == UIType)
                {
                    *pOut = m_UITypeKeyHashs[proto].m_iLevel;
                    return m_UITypeKeyHashs[proto].PrototypeName;
                }
            }
        }
    }

    else
    {
        *pOut = LEVEL::STATIC;
        return Proto_UIObject;
    }
       

    *pOut = BaseIter->second.m_iLevel;
    return BaseIter->second.PrototypeName;
}

//////////////////////////////////////////////////////// 생성자 호출 함수 ////////////////////////////////////////////////////////
Parser_UITool* Client::Parser_UITool::Create()
{
    Parser_UITool* pInstance = new Parser_UITool();

    pInstance->Initialize();

    return pInstance;
}
/******************************************************* 생성자 호출 함수 *******************************************************/



//////////////////////////////////////////////////////// 객체 반환 함수 ////////////////////////////////////////////////////////
void Client::Parser_UITool::Free()
{
 
    UnSubcribe_Evnets();

    __super::Free();
}
/******************************************************* 객체 반환 함수 *******************************************************/


