#pragma once

#pragma push_macro("new")
#undef new

//#define IMGUI_IMPLEMENTATION
//#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_internal.h"
//#include "imgui_draw.cpp"
//#include "imgui_widgets.cpp"
//#include "imgui_tables.cpp"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#pragma pop_macro("new")



namespace Client {
    enum Panel_Num { Panel_Hierarchy = 0, Panel_Inspector, Panel_Animation, Panel_Track, Panel_Light, Panel_END };
}
