#pragma once
// Client_Macro.h
#include <string_view>

#include "Client_Macro_Component.h"
#include "Client_Macro_GameObject.h"

using StringID = uint32_t;


namespace ProtoName
{
#pragma region Texture
	inline const _wstring Proto_Texture_Logo    = L"Prototype_Component_Texture_Logo";


#define Proto_Texture(name) L"Prototype_Component_Texture_" name
#pragma endregion Texture



#pragma region UI_Texture


#define Proto_UITexture(name) L"Prototype_Component_UITexture_" name
#pragma endregion UI_Texture


#pragma region Video
    inline const _wstring Proto_Video_Title = L"Prototype_Component_Video_Title";


#define Proto_Video(name) L"Prototype_Component_Video_" name
#pragma endregion Video



#pragma region LayerName
    inline const _wstring Layer_Camera	        = L"Layer_Camera";
    inline const _wstring Layer_Enviroment	    = L"Layer_Enviroment";
    inline const _wstring Layer_GameObject	    = L"Layer_GameObject";
    inline const _wstring Layer_Monsters        = L"Layer_Monster";
    inline const _wstring Layer_UIs             = L"Layer_UIs";
    inline const _wstring Layer_Sky	            = L"Layer_Sky";
    inline const _wstring Layer_Projectile      = L"Layer_Projectile";
    inline const _wstring Layer_Effect          = L"Layer_Effect";
    inline const _wstring Layer_Items           = L"Layer_Items";
#pragma endregion LayerName


#pragma region Fonts
    inline const _wstring Font_Default = L"Terraria";
    inline const _wstring Font_Maple = L"Maple";
    inline const _wstring Font_Uma = L"Gyungi_M13";
    inline const _wstring Font_M10 = L"Gyungi_M10";

    //ÇÑ±Û
    inline const _wstring Font_Yoon310 = L"Yoon310";
    inline const _wstring Font_Yoon320 = L"Yoon320";
    inline const _wstring Font_Yoon330 = L"Yoon330";

    inline const _wstring Font_A2Z_5_Medium = L"A2Z_5_Medium";

    //Number
    inline const _wstring Font_BankGothic = L"Bank Gothic Medium";

    inline const _wstring Font_SquadaOne = L"SquadaOne-Regular";
    inline const _wstring Font_Rajdhani_Bold = L"Rajdhani Bold";
    inline const _wstring Font_Rajdhani_Light = L"Rajdhani-Light";
    inline const _wstring Font_Rajdhani_Medium = L"Rajdhani-Medium";
    inline const _wstring Font_Rajdhani_SemiBold = L"Rajdhani-SemiBold";


    //BitmapFont
    inline const _wstring BitmapFont_Rajdhani_Medium = L"Bitmap_Rajdhani-Medium";
    inline const _wstring BitmapFont_Rajdhani_LightGray = L"Bitmap_Rajdhani-LightGray";
    inline const _wstring BitmapFont_Squada_One = L"Bitmap__Squada_One";


    inline const _wstring Font_Bookk_Bold = L"Font_Bookk_Bold";
    inline const _wstring Font_Bookk_Bold_Eng = L"Font_Bookk_Bold_Eng";



    inline const _wstring Font_Shine_Bold = L"Font_Shine_Bold";

#define Font(name) L"Font_" name
#pragma endregion Fonts


}

using namespace ProtoName;
