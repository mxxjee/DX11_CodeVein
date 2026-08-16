#pragma once
// Client_Macro.h
#include <string_view>

#include "ShaderTool_Macro_Component.h"
#include "ShaderTool_Macro_GameObject.h"

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
    inline const _wstring Layer_UIs             = L"Layer_UIs";
    inline const _wstring Layer_Horses	        = L"Layer_Horses";
#pragma endregion LayerName


#pragma region Fonts
    inline const _wstring Font_Default	    = L"Gyungi_M13";
    inline const _wstring Font_Terraria	    = L"Terraria";
    inline const _wstring Font_Maple        = L"Maple";
    inline const _wstring Font_Uma          = L"Gyungi_M13";
    inline const _wstring Font_M10          = L"Gyungi_M10";

#define Font(name) L"Font_" name
#pragma endregion Fonts


}

using namespace ProtoName;
