#pragma once


namespace ProtoName
{
#define Proto_GameObject(name) L"Prototype_GameObject_" name


#pragma region Camera
    inline const _wstring Proto_GameObject_Camera_Free = L"Prototype_GameObject_Camera_Free";
    inline const _wstring Proto_GameObject_Camera_Focus = L"Prototype_GameObject_Camera_Player_Focus";
    inline const _wstring Proto_GameObject_Camera_Player = L"Prototype_GameObject_Camera_Player";
    inline const _wstring Proto_GameObject_Camera_Customize = L"Prototype_GameObject_Camera_Customize";
    inline const _wstring Proto_GameObject_Camera_NPC = L"Prototype_GameObject_Camera_NPC";
#pragma endregion Camera



#pragma region Character
    inline const _wstring Proto_GameObject_Wonder_Acute = L"Prototype_GameObject_Wonder_Acute";
    inline const _wstring Proto_GameObject_SkySphere = L"Prototype_GameObject_SkySphere";
    inline const _wstring Proto_GameObject_SphereSun = L"Prototype_GameObject_SphereSun";
    
#pragma endregion Character


#pragma region UI

    inline const _wstring Proto_UIObject = L"Prototype_UIObject";
    inline const _wstring Proto_UIObject_LOGO = L"Prototype_UIObject_LOGO";
    inline const _wstring Proto_UIObject_PROGRESSBAR = L"Prototype_UIObject_ProgressBar";
    inline const _wstring Proto_UIObject_PLAYERHUD = L"Prototype_UIObject_PLAYERHUD";
    inline const _wstring Proto_UIObject_BITMAPTEXT = L"Prototype_UIObject_BitmapText";
    inline const _wstring Proto_UIObject_SLOTGRID = L"Prototype_UIObject_SlotGrid";
    inline const _wstring Proto_UIObject_SLOT = L"Prototype_UIObject_Slot";
    inline const _wstring Proto_UIObject_TEXT = L"Prototype_UIObject_Text";


    inline const _wstring Proto_UIObject_PLAYERMENUWINDOW = L"Prototype_UIObject_Window_PlayerMenu";
    inline const _wstring Proto_UIObject_INVENTORYWINDOW = L"Prototype_UIObject_Window_InventoryWindow";
    inline const _wstring Proto_UIObject_SUBWINDOW = L"Prototype_UIObject_Window_SubWindow";
    inline const _wstring Proto_UIObject_SUBDESC = L"Prototype_UIObject_SubDesc";

    inline const _wstring Proto_UIObject_SHORTCUTMENU = L"Prototype_UIObject_ShortCutMenu";
    inline const _wstring Proto_UIObject_PopUp_ItemGet = L"Prototype_UIObject_PopUp_ItemGet";
    inline const _wstring Proto_UIObject_PopUp_Interaction = L"Prototype_UIObject_PopUp_Interaction";
    inline const _wstring Proto_UIObject_PopUp_MapTitle = L"Prototype_UIObject_MapTitle";
    inline const _wstring Proto_UIObject_BossName = L"Prototype_UIObject_BossName";
    inline const _wstring Proto_UIObject_BlurUI = L"Prototype_UIObject_BlurUI";

    inline const _wstring Proto_UIObject_LockOn = L"Prototype_UIObject_Lockon";

    //수정필요
    inline const _wstring Proto_UIObject_HPBAR = L"Prototype_UIObject_HpBar";
    inline const _wstring Proto_UIObject_STAMINABAR = L"Prototype_UIObject_Stamina";
    inline const _wstring Proto_UIObject_GUARDBAR = L"Prototype_UIObject_GuardBar";
    

    inline const _wstring Proto_UIObject_QUICKSLOT = L"Prototype_UIObject_QuickSlot";
    inline const _wstring Proto_UIObject_SHORTCUTQUICKSLOT = L"Prototype_UIObject_ShortCutQuickSlot";

    inline const _wstring Proto_UIObject_PLAYERHUD_LEFT = L"Prototype_UIObject_PLAYERHUD";
    inline const _wstring Proto_UIObject_SUBWINDOW_BLOODCODE = L"Prototype_UIObject_Window_Bloodcode";
    inline const _wstring Proto_UIObject_MINIMAP = L"Prototype_UIObject_Window_Minimap";
    inline const _wstring Proto_UIObject_MonsterStatus = L"Prototype_UIObject_MonsterStatus";

    inline const _wstring Proto_UIObject_FadeScreen = L"Prototype_UIObject_FadeScreen";
    inline const _wstring Proto_UIObject_LoadingScreen = L"Prototype_UIObject_LoadingScreen";
    inline const _wstring Proto_UIObject_LoadingCube = L"Prototype_UIObject_LoadingCube";


    inline const _wstring Proto_UIObject_TitleWindow = L"Prototype_UIObject_TitleWindow";
    inline const _wstring Proto_UIObject_Selector = L"Prototype_UIObject_Selector";
    inline const _wstring Proto_UIObject_GlowButton= L"Prototype_UIObject_GlowButton";
    inline const _wstring Proto_UIObject_FocusMenu = L"Prototype_UIObject_FocusMenu";

    inline const _wstring Proto_UIObject_SceneSelectMenu = L"Prototype_UIObject_SceneSelectMenu";
    inline const _wstring Proto_UIObject_SceneSlot = L"Prototype_UIObject_SceneSlot";
    inline const _wstring Proto_UIObject_Haze = L"Prototype_UIObject_Haze";
    inline const _wstring Proto_UIObject_SavePoint_Slot = L"Prototype_UIObject_SavePoint_Slot";
    inline const _wstring Proto_UIObject_ManaInfo = L"Prototype_UIObject_ManaInfo";
    
    inline const _wstring Proto_UIObject_NpcDialogue = L"Prototype_UIObject_NPCDialogue";
    inline const _wstring Proto_UIObject_FocusMenu_NPC = L"Prototype_UIObject_Focus_NPCMenu";
    inline const _wstring Proto_UIObject_NPCMenu = L"Prototype_UIObject_NPC_ChoiceMenu";


    inline const _wstring Proto_UIObject_WINDOW_SHOP = L"Prototype_UIObject_WINDOW_SHOP";
    inline const _wstring Proto_UIObject_SHOP_CATEGORY = L"Prototype_UIObject_SHOP_CATEGORY";
    inline const _wstring Proto_UIObject_SHOP_SLOTGRID = L"Prototype_UIObject_SHOP_SLOTGRID";
    inline const _wstring Proto_UIObject_SHOP_SLOT = L"Prototype_UIObject_SHOP_SLOT";
    inline const _wstring Proto_UIObject_SHOP_DESC_WINDOW = L"Prototype_UIObject_SHOP_DESC_WINDOW";
    inline const _wstring Proto_UIObject_SHOP_DESC_SUBINFO = L"Prototype_UIObject_SHOP_DESC_SUBINFO";
    inline const _wstring Proto_UIObject_SHOP_NEEDINFO = L"Prototype_UIObject_SHOP_NEEDINFO";


    inline const _wstring Proto_UIObject_CUSTOMIZINGMENU = L"Prototype_UIObject_CUSTOMIZINGMENU";
    inline const _wstring Proto_UIObject_CUSTOMIZINGMENU_BUTTON = L"Prototype_UIObject_CUSTOMIZINGMENU_BUTTON";
    inline const _wstring Proto_UIObject_CUSTOMMENU_INFO = L"Prototype_UIObject_CUSTOMMENU_INFO";



    inline const _wstring Proto_UIObject_PALEETE_WINDOW = L"Prototype_UIObject_PALEETE_WINDOW";
    inline const _wstring Proto_UIObject_PALLETE_AREA = L"Prototype_UIObject_PALLETE_AREA";
    inline const _wstring Proto_UIObject_PALLETE_DISPLAY = L"Prototype_UIObject_PALLETE_DISPLAY";

    inline const _wstring Proto_UIObject_VERTICAL_SLIDER = L"Prototype_UIObject_VERTICAL_SLIDER";

    inline const _wstring Proto_UIObject_CUSTOMIZE_GRID = L"Prototype_UIObject_CUSTOMIZE_GRID";
    
    inline const _wstring Proto_UIObject_COLOR_SELECTOR = L"Prototype_UIObject_COLOR_SELECTOR";
    inline const _wstring Proto_UIObject_TEXTURE_SELECTOR = L"Prototype_UIObject_TEXTURE_SELECTOR";
    inline const _wstring Proto_UIObject_MESH_SELECTOR = L"Prototype_UIObject_MESH_SELECTOR";
    inline const _wstring Proto_UIObject_VALUESELECTOR = L"Prototype_UIObject_VALUE_SELECTOR";

    inline const _wstring Proto_UIObject_ITEMGRID = L"Prototype_UIObject_ITEMGRID";
    inline const _wstring Proto_UIObject_BOSSDEAD = L"Prototype_UIObject_BOSSDEAD";

    inline const _wstring Proto_UIObject_LevelUpWindow = L"Prototype_UIObject_LevelUpWindow";


    inline const _wstring Proto_UIObject_Ending = L"Prototype_UIObject_Ending";

#define Proto_UIObject(name) L"Prototype_UIObject_" name

#pragma endregion



#pragma region Effect
    inline const _wstring Proto_GameObject_Effect_Cursor = L"Prototype_GameObject_Effect_PlayerCursor";

#define Proto_EffectObject(name) L"Prototype_EffectObject_" name


#pragma endregion Effect


#pragma region EventShape
    inline const _wstring Proto_GameObject_Monster_EventShape = L"Prototype_GameObject_EventShape_Monster";
  
#define Proto_EventShape(name) L"Prototype_GameObject_EventShape_" name


#pragma endregion EventShape

    #define  POOLING PoolingManager::Get_Instance()

}
