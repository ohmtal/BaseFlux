//-----------------------------------------------------------------------------
// Copyright (c) 2026 Thomas Hühn (XXTH)
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
// ElfScript ImGui binding
//-----------------------------------------------------------------------------
//FIXME THIS MUST BE PORTED TO OhmFlux :D
// or better add this project to ElfScript that is what BaseFlux was
// made for: ProtoTyping so it should be there
// OhmFlux can simple add this file and is ready to go

//NOTE  dont forget to call InitBindings_ImGui when porting;)

#include "console/engineAPI.h"
#include "console/consoleExtras.h"
#include "ConsoleTypes.h"
#include "core/strings/stringUnit.h"


#include <string>
#include <format>

#include <imgui.h>
#include <imgui_internal.h>

// -----------------------------------------------------------------------------
inline ImVec4 ToImVec4(const Color4F& color) {
    return ImVec4(color.r, color.g, color.b, color.a);
}
inline ImVec2 ToImVec2(const Point2F& p) {
    return ImVec2(p.x,p.y);
}


static ImU32 ParseColor(F32 r, F32 g, F32 b, F32 a = 1.0f) {
    return ImGui::ColorConvertFloat4ToU32(ImVec4(r, g, b, a));
}

// -----------------------------------------------------------------------------
void RegisterImGuiWindowConstants() {
    #define REGISTER_IMGUI_CONST(constName) Con::setScriptConstant(#constName, (S32)constName)

    // Window Flags (ImGuiWindowFlags_)
    REGISTER_IMGUI_CONST(ImGuiWindowFlags_None);
    REGISTER_IMGUI_CONST(ImGuiWindowFlags_NoTitleBar);                // Disable title-bar
    REGISTER_IMGUI_CONST(ImGuiWindowFlags_NoResize);                  // Disable user resizing with the lower-right corner
    REGISTER_IMGUI_CONST(ImGuiWindowFlags_NoMove);                    // Disable user moving the window
    REGISTER_IMGUI_CONST(ImGuiWindowFlags_NoScrollbar);               // Disable scrollbars (window can still scroll with mouse wheel)
    REGISTER_IMGUI_CONST(ImGuiWindowFlags_NoScrollWithMouse);         // Disable user vertically scrolling with mouse wheel
    REGISTER_IMGUI_CONST(ImGuiWindowFlags_NoCollapse);                // Disable user collapsing window by double-clicking it
    REGISTER_IMGUI_CONST(ImGuiWindowFlags_AlwaysAutoResize);          // Resize every frame to its content
    REGISTER_IMGUI_CONST(ImGuiWindowFlags_NoBackground);              // Disable drawing background color and margins
    REGISTER_IMGUI_CONST(ImGuiWindowFlags_NoSavedSettings);           // Never load/save settings in .ini file
    REGISTER_IMGUI_CONST(ImGuiWindowFlags_NoMouseInputs);             // Disable catching mouse inputs
    REGISTER_IMGUI_CONST(ImGuiWindowFlags_MenuBar);                  // Has a menu-bar (required for ImBeginMenuBar)
    REGISTER_IMGUI_CONST(ImGuiWindowFlags_HorizontalScrollbar);       // Allow horizontal scrollbar to appear
    REGISTER_IMGUI_CONST(ImGuiWindowFlags_NoFocusOnAppearing);        // Disable taking focus when transitioning from hidden to visible state
    REGISTER_IMGUI_CONST(ImGuiWindowFlags_NoBringToFrontOnFocus);     // Disable bringing window to front when clicking on it
    REGISTER_IMGUI_CONST(ImGuiWindowFlags_AlwaysVerticalScrollbar);   // Always show vertical scrollbar
    REGISTER_IMGUI_CONST(ImGuiWindowFlags_AlwaysHorizontalScrollbar); // Always show horizontal scrollbar
    REGISTER_IMGUI_CONST(ImGuiWindowFlags_NoNavFocus);                // No focusing navigation inside window
    REGISTER_IMGUI_CONST(ImGuiWindowFlags_UnsavedDocument);           // Display a dot next to the title (indicates unsaved changes)
    REGISTER_IMGUI_CONST(ImGuiWindowFlags_NoNav);                     // Combination of NoNavInputs and NoNavFocus
    REGISTER_IMGUI_CONST(ImGuiWindowFlags_NoDecoration);              // Combination of NoTitleBar, NoResize, NoScrollbar, NoCollapse
    REGISTER_IMGUI_CONST(ImGuiWindowFlags_NoInputs);                  // Combination of NoMouseInputs, NoNavInputs, NoNavFocus

    #undef REGISTER_IMGUI_CONST
}

// -----------------------------------------------------------------------------
void RegisterImGuiKeyConstants() {
    #define REGISTER_IMGUI_CONST(constName) Con::setScriptConstant(#constName, (S32)constName)

    // Modifiers / Chords
    REGISTER_IMGUI_CONST(ImGuiMod_None);
    REGISTER_IMGUI_CONST(ImGuiMod_Ctrl);
    REGISTER_IMGUI_CONST(ImGuiMod_Shift);
    REGISTER_IMGUI_CONST(ImGuiMod_Alt);
    REGISTER_IMGUI_CONST(ImGuiMod_Super);

    // Alphabet
    REGISTER_IMGUI_CONST(ImGuiKey_A); REGISTER_IMGUI_CONST(ImGuiKey_B);
    REGISTER_IMGUI_CONST(ImGuiKey_C); REGISTER_IMGUI_CONST(ImGuiKey_D);
    REGISTER_IMGUI_CONST(ImGuiKey_E); REGISTER_IMGUI_CONST(ImGuiKey_F);
    REGISTER_IMGUI_CONST(ImGuiKey_G); REGISTER_IMGUI_CONST(ImGuiKey_H);
    REGISTER_IMGUI_CONST(ImGuiKey_I); REGISTER_IMGUI_CONST(ImGuiKey_J);
    REGISTER_IMGUI_CONST(ImGuiKey_K); REGISTER_IMGUI_CONST(ImGuiKey_L);
    REGISTER_IMGUI_CONST(ImGuiKey_M); REGISTER_IMGUI_CONST(ImGuiKey_N);
    REGISTER_IMGUI_CONST(ImGuiKey_O); REGISTER_IMGUI_CONST(ImGuiKey_P);
    REGISTER_IMGUI_CONST(ImGuiKey_Q); REGISTER_IMGUI_CONST(ImGuiKey_R);
    REGISTER_IMGUI_CONST(ImGuiKey_S); REGISTER_IMGUI_CONST(ImGuiKey_T);
    REGISTER_IMGUI_CONST(ImGuiKey_U); REGISTER_IMGUI_CONST(ImGuiKey_V);
    REGISTER_IMGUI_CONST(ImGuiKey_W); REGISTER_IMGUI_CONST(ImGuiKey_X);
    REGISTER_IMGUI_CONST(ImGuiKey_Y); REGISTER_IMGUI_CONST(ImGuiKey_Z);

    // Numbers
    REGISTER_IMGUI_CONST(ImGuiKey_0); REGISTER_IMGUI_CONST(ImGuiKey_1);
    REGISTER_IMGUI_CONST(ImGuiKey_2); REGISTER_IMGUI_CONST(ImGuiKey_3);
    REGISTER_IMGUI_CONST(ImGuiKey_4); REGISTER_IMGUI_CONST(ImGuiKey_5);
    REGISTER_IMGUI_CONST(ImGuiKey_6); REGISTER_IMGUI_CONST(ImGuiKey_7);
    REGISTER_IMGUI_CONST(ImGuiKey_8); REGISTER_IMGUI_CONST(ImGuiKey_9);

    // Function Keys
    REGISTER_IMGUI_CONST(ImGuiKey_F1);  REGISTER_IMGUI_CONST(ImGuiKey_F2);
    REGISTER_IMGUI_CONST(ImGuiKey_F3);  REGISTER_IMGUI_CONST(ImGuiKey_F4);
    REGISTER_IMGUI_CONST(ImGuiKey_F5);  REGISTER_IMGUI_CONST(ImGuiKey_F6);
    REGISTER_IMGUI_CONST(ImGuiKey_F7);  REGISTER_IMGUI_CONST(ImGuiKey_F8);
    REGISTER_IMGUI_CONST(ImGuiKey_F9);  REGISTER_IMGUI_CONST(ImGuiKey_F10);
    REGISTER_IMGUI_CONST(ImGuiKey_F11); REGISTER_IMGUI_CONST(ImGuiKey_F12);

    // Navigation & Editing
    REGISTER_IMGUI_CONST(ImGuiKey_Tab);
    REGISTER_IMGUI_CONST(ImGuiKey_LeftArrow);
    REGISTER_IMGUI_CONST(ImGuiKey_RightArrow);
    REGISTER_IMGUI_CONST(ImGuiKey_UpArrow);
    REGISTER_IMGUI_CONST(ImGuiKey_DownArrow);
    REGISTER_IMGUI_CONST(ImGuiKey_PageUp);
    REGISTER_IMGUI_CONST(ImGuiKey_PageDown);
    REGISTER_IMGUI_CONST(ImGuiKey_Home);
    REGISTER_IMGUI_CONST(ImGuiKey_End);
    REGISTER_IMGUI_CONST(ImGuiKey_Insert);
    REGISTER_IMGUI_CONST(ImGuiKey_Delete);
    REGISTER_IMGUI_CONST(ImGuiKey_Backspace);
    REGISTER_IMGUI_CONST(ImGuiKey_Space);
    REGISTER_IMGUI_CONST(ImGuiKey_Enter);
    REGISTER_IMGUI_CONST(ImGuiKey_Escape);

    // Mouse Button Enums
    REGISTER_IMGUI_CONST(ImGuiMouseButton_Left);
    REGISTER_IMGUI_CONST(ImGuiMouseButton_Right);
    REGISTER_IMGUI_CONST(ImGuiMouseButton_Middle);

    #undef REGISTER_IMGUI_CONST
}
// -----------------------------------------------------------------------------
void RegisterImGuiTableConstants() {
    #define REGISTER_IMGUI_CONST(constName) Con::setScriptConstant(#constName, (S32)constName)

    // Table Flags (ImGuiTableFlags_)
    REGISTER_IMGUI_CONST(ImGuiTableFlags_None);
    REGISTER_IMGUI_CONST(ImGuiTableFlags_Resizable);        // Make columns resizable
    REGISTER_IMGUI_CONST(ImGuiTableFlags_Reorderable);      // Allow reordering columns via drag-and-drop
    REGISTER_IMGUI_CONST(ImGuiTableFlags_Hideable);         // Allow hiding columns via right-click
    REGISTER_IMGUI_CONST(ImGuiTableFlags_Sortable);         // Enable sorting features
    REGISTER_IMGUI_CONST(ImGuiTableFlags_NoSavedSettings);  // Don't save style modifications to .ini
    REGISTER_IMGUI_CONST(ImGuiTableFlags_ContextMenuInBody);// Right-click inside table body opens options

    // Table Decorations / Styling
    REGISTER_IMGUI_CONST(ImGuiTableFlags_RowBg);            // Alternating row background colors (Zebra stripes)
    REGISTER_IMGUI_CONST(ImGuiTableFlags_BordersInnerH);    // Draw horizontal borders inside
    REGISTER_IMGUI_CONST(ImGuiTableFlags_BordersOuterH);    // Draw horizontal borders outside
    REGISTER_IMGUI_CONST(ImGuiTableFlags_BordersInnerV);    // Draw vertical borders inside
    REGISTER_IMGUI_CONST(ImGuiTableFlags_BordersOuterV);    // Draw vertical borders outside
    REGISTER_IMGUI_CONST(ImGuiTableFlags_Borders);          // Full border grid combination
    REGISTER_IMGUI_CONST(ImGuiTableFlags_ScrollX);          // Enable horizontal scrolling inside table
    REGISTER_IMGUI_CONST(ImGuiTableFlags_ScrollY);          // Enable vertical scrolling inside table

    // Column Configuration Flags (ImGuiTableColumnFlags_)
    REGISTER_IMGUI_CONST(ImGuiTableColumnFlags_None);
    REGISTER_IMGUI_CONST(ImGuiTableColumnFlags_Disabled);    // Completely hide/disable column
    REGISTER_IMGUI_CONST(ImGuiTableColumnFlags_DefaultHide); // Hidden by default, user can turn it on
    REGISTER_IMGUI_CONST(ImGuiTableColumnFlags_WidthFixed);
    REGISTER_IMGUI_CONST(ImGuiTableColumnFlags_WidthStretch);

    #undef REGISTER_IMGUI_CONST
}

// -----------------------------------------------------------------------------
// Borrowed from OhmFlux
void RegisterColorConstants() {

    #define REGISTER_COLOR_CONST(name, r, g, b, a) \
    { \
        char colBuf[128]; \
        std::snprintf(colBuf, sizeof(colBuf), "\"%f %f %f %f\"", r, g, b, a); \
        Con::setScriptConstant(#name, colBuf); \
    }

    // Standard Colors
    REGISTER_COLOR_CONST(cl_NONE,        -1.0f, -1.0f, -1.0f, -1.0f);
    REGISTER_COLOR_CONST(cl_White,        1.0f,  1.0f,  1.0f,  1.0f);
    REGISTER_COLOR_CONST(cl_Black,        0.0f,  0.0f,  0.0f,  1.0f);
    REGISTER_COLOR_CONST(cl_Red,          1.0f,  0.0f,  0.0f,  1.0f);
    REGISTER_COLOR_CONST(cl_Green,        0.0f,  1.0f,  0.0f,  1.0f);
    REGISTER_COLOR_CONST(cl_Blue,         0.0f,  0.0f,  1.0f,  1.0f);
    REGISTER_COLOR_CONST(cl_Yellow,       1.0f,  1.0f,  0.0f,  1.0f);
    REGISTER_COLOR_CONST(cl_Cyan,         0.0f,  1.0f,  1.0f,  1.0f);
    REGISTER_COLOR_CONST(cl_Magenta,      1.0f,  0.0f,  1.0f,  1.0f);
    REGISTER_COLOR_CONST(cl_Gray,         0.5f,  0.5f,  0.5f,  1.0f);
    REGISTER_COLOR_CONST(cl_LightGray,    0.75f, 0.75f, 0.75f, 1.0f);
    REGISTER_COLOR_CONST(cl_DarkGray,     0.25f, 0.25f, 0.25f, 1.0f);
    REGISTER_COLOR_CONST(cl_Orange,       1.0f,  0.5f,  0.0f,  1.0f);
    REGISTER_COLOR_CONST(cl_Purple,       0.5f,  0.0f,  0.5f,  1.0f);
    REGISTER_COLOR_CONST(cl_Brown,        0.6f,  0.3f,  0.0f,  1.0f);
    REGISTER_COLOR_CONST(cl_Lime,         0.75f, 1.0f,  0.0f,  1.0f);
    REGISTER_COLOR_CONST(cl_Pink,         1.0f,  0.4f,  0.7f,  1.0f);

    // Modern / UI UI Accents
    REGISTER_COLOR_CONST(cl_Crimson,      0.86f, 0.08f, 0.24f, 1.0f);
    REGISTER_COLOR_CONST(cl_Emerald,      0.16f, 0.71f, 0.44f, 1.0f);
    REGISTER_COLOR_CONST(cl_SkyBlue,      0.53f, 0.81f, 0.98f, 1.0f);
    REGISTER_COLOR_CONST(cl_Slate,        0.18f, 0.24f, 0.31f, 1.0f);
    REGISTER_COLOR_CONST(cl_Gold,         1.00f, 0.84f, 0.00f, 1.0f);
    REGISTER_COLOR_CONST(cl_Transparent,  0.00f, 0.00f, 0.00f, 0.00f);

    // Environment Colors
    REGISTER_COLOR_CONST(cl_Aquamarine,   0.50f, 1.00f, 0.83f, 1.0f);
    REGISTER_COLOR_CONST(cl_Coral,        1.00f, 0.50f, 0.31f, 1.0f);
    REGISTER_COLOR_CONST(cl_DeepSea,      0.00f, 0.08f, 0.20f, 1.0f);
    REGISTER_COLOR_CONST(cl_Seafoam,      0.60f, 0.85f, 0.75f, 1.0f);
    REGISTER_COLOR_CONST(cl_Sand,         0.76f, 0.70f, 0.50f, 1.0f);
    REGISTER_COLOR_CONST(cl_Kelp,         0.13f, 0.29f, 0.13f, 1.0f);

    // Debug / Visualizer Colors
    REGISTER_COLOR_CONST(cl_NeonPink,     1.00f, 0.00f, 0.50f, 1.0f);
    REGISTER_COLOR_CONST(cl_ElectricBlue, 0.00f, 1.00f, 1.00f, 1.0f);
    REGISTER_COLOR_CONST(cl_AcidGreen,    0.50f, 1.00f, 0.00f, 1.0f);

    // Special FX Colors
    REGISTER_COLOR_CONST(cl_Glass,        1.00f, 1.00f, 1.00f, 0.25f);
    REGISTER_COLOR_CONST(cl_Shadow,       0.00f, 0.00f, 0.00f, 0.40f);
    REGISTER_COLOR_CONST(cl_Ghost,        0.70f, 0.70f, 1.00f, 0.50f);

    #undef REGISTER_COLOR_CONST
}

// -----------------------------------------------------------------------------
void InitBindings_ImGui() {
    //real constant using preprocessor*
     Con::setScriptConstant("FLT_MIN", (F64)FLT_MIN);
     Con::setScriptConstant("FLT_MAX", (F64)FLT_MAX);

     RegisterImGuiWindowConstants();
     RegisterImGuiTableConstants();
     RegisterImGuiKeyConstants();
     RegisterColorConstants();

     // Con::registerEnumS32<ImGuiKey>("", true);
     //enum ImGuiKey : int
}
// -----------------------------------------------------------------------------
ConsoleFunctionGroupBegin( ImGui, "ImGui/BaseFlux functions");
// -----------------------------------------------------------------------------
DefineEngineFunction(ImSetNextWindowSize, void, (Point2F size, S32 condition ),(0),"")
{
    ImGui::SetNextWindowSize(ToImVec2(size), condition);
}

// -----------------------------------------------------------------------------
DefineEngineFunction(ImBegin, bool, (String name, String openVarName, S32 windowFlags),("", 0 ),"")
{
    // IMGUI_API bool          Begin(const char* name, bool* p_open = NULL, ImGuiWindowFlags flags = 0);
    if (!openVarName.isEmpty()) {
        bool value = Con::getBoolVariable(openVarName.c_str(), true);
        if (ImGui::Begin(name.c_str(), &value, windowFlags)) {
            Con::setBoolVariable(openVarName, value);
            return true;
        }
        return false;
    }
    return ImGui::Begin(name.c_str());
}

DefineEngineFunction(ImEnd, void, (),,"") { ImGui::End(); }

DefineEngineFunction(ImBeginGroup, void, (),,"") { ImGui::BeginGroup(); }
DefineEngineFunction(ImEndGroup, void, (),,"") { ImGui::EndGroup(); }

// -----------------------------------------------------------------------------
DefineEngineFunction(ImSameLine, void, (F32 offsetX, F32 spacing),(0.0f, -1.0f),"") {
    ImGui::SameLine(offsetX, spacing);
}
DefineEngineFunction(ImNewLine, void, (),,"") { ImGui::NewLine(); }
DefineEngineFunction(ImSpacing, void, (),,"") { ImGui::Spacing(); }
DefineEngineFunction(ImDummy, void, (Point2F size ),,"") { ImGui::Dummy(ToImVec2(size)); }
// -----------------------------------------------------------------------------
DefineEngineFunction(ImPushID, void, (const char* str_id),,"") { ImGui::PushID(str_id); }
DefineEngineFunction(ImPopID, void, (),,"") { ImGui::PopID(); }
// -----------------------------------------------------------------------------
DefineEngineFunction(ImText, void, (String text),,"") { ImGui::Text("%s", text.c_str()); }
DefineEngineFunction(ImTextColored, void, (Color4F color, String text),,"") { ImGui::TextColored(ToImVec4(color), "%s", text.c_str()); }
DefineEngineFunction(ImTextLink, bool, (const char* label),,"") { return ImGui::TextLink(label); }
// -----------------------------------------------------------------------------
DefineEngineFunction(ImBullet, void, (),,"draw a small circle + keep the cursor on the same line") { ImGui::Bullet(); }
// -----------------------------------------------------------------------------
// ProgressBar(float fraction, const ImVec2& size_arg = ImVec2(-FLT_MIN, 0), const char* overlay = NULL);
DefineEngineFunction(ImProgressBar, void, (F32 fraction, Point2F size, String overlay),(Point2F(-FLT_MIN, 0.f), "")  ,"a ProgressBar") {
    if (!overlay.isEmpty()) ImGui::ProgressBar(fraction, ToImVec2(size), overlay.c_str());
    else ImGui::ProgressBar(fraction, ToImVec2(size));
}
// -----------------------------------------------------------------------------
DefineEngineFunction(ImSeparator, void, (),,"") { ImGui::Separator(); }
DefineEngineFunction(ImSeparatorText, void, (const char* label),,"") { ImGui::SeparatorText(label); }
// -----------------------------------------------------------------------------
// IMGUI_API bool          Button(const char* label, const ImVec2& size = ImVec2(0, 0));   // button
DefineEngineFunction(ImButton, bool, (const char* label, Point2F size ),(Point2F(0.f,0.f)),"") {
    return ImGui::Button(label, ToImVec2(size));
}
// -----------------------------------------------------------------------------
DefineEngineFunction(ImCheckbox, bool, (const char* text, const char* valueVarName),,
    "return true if changed add a varname as reference. Can be a global variable or a Object field.\n"
    "Example: Imcheckbox(\"test\", \"$myvar\"); "
    "Example: Imcheckbox(\"test\", \"MyObject.myvar\"); "
    "Example: Imcheckbox(\"test\", %obj.getId() @ \".myvar\"); "
) {
    bool value = Con::getBoolVariable(valueVarName, false);
    bool result = ImGui::Checkbox(text, &value);
    if (result) {
        Con::setBoolVariable(valueVarName, value);
    }
    return result;
}
// -----------------------------------------------------------------------------
DefineEngineFunction(ImSliderFloat, bool, (const char* text, const char* valueVarName, F32 min, F32 max, const char* format, S32 flags),
    (0.f, 1.f,"%.3f", 0),
    "return true if changed add a varname as reference. Can be a global variable or a Object field.\n"
) {
    F32 value = Con::getFloatVariable(valueVarName, 0.f);
    bool result = ImGui::SliderFloat(text, &value, min, max, format, flags);
    if (result) {
        Con::setFloatVariable(valueVarName, value);
    }
    return result;
}
// -----------------------------------------------------------------------------
DefineEngineFunction(ImSliderInt, bool, (const char* text, const char* valueVarName
    , S32 min, S32 max, const char* format, S32 flags),
    (0, 100,"%d", 0),
    "return true if changed add a varname as reference. Can be a global variable or a Object field.\n"
) {
    S32 value = Con::getIntVariable(valueVarName, 0);
    bool result = ImGui::SliderInt(text, &value, min, max, format, flags);
    if (result) {
        Con::setIntVariable(valueVarName, value);
    }
    return result;
}
// -----------------------------------------------------------------------------
// InputText(const char* label, char* buf, size_t buf_size, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
DefineEngineFunction(ImInputText, bool, (const char* text, const char* valueVarName,  S32 flags),
    (0),
    "return true if changed add a varname as reference. Can be a global variable or a Object field.\n"
) {
    char buf[1024]{};
    dStrcpy(buf, Con::getVariable(valueVarName, ""), 1024);
    bool result = ImGui::InputText(text, buf,1024, flags);
    if (result) {
        Con::setVariable(valueVarName, buf);
    }
    return result;
}

// -----------------------------------------------------------------------------
DefineEngineFunction(ImCombo, bool, (
    const char* label
    , const char* currentItemIndexVarName
    , const char* ItemsTabSeparated),,
    "return true if changed add a varname as reference. Can be a global variable or a Object field.\n"
    "@var currentItemIndexVarName is the name of variable where the current item index lives\n"
    "@var ItemsTabSeparated tab separated list of items \n"
) {
    S32 index = Con::getIntVariable(currentItemIndexVarName, 0);
    bool valueChanged = false;
    const char* previewText = "";
    if (ItemsTabSeparated && ItemsTabSeparated[0] != '\0' && index >= 0) {
        const char* currentItem = StringUnit::getUnit(ItemsTabSeparated, index, "\t");
        if (currentItem) {
            previewText = currentItem;
        }
    }
    if (ImGui::BeginCombo(label, previewText, 0)) {
        if (ItemsTabSeparated && ItemsTabSeparated[0] != '\0') {
            U32 count = StringUnit::getUnitCount(ItemsTabSeparated, "\t");

            for (U32 i = 0; i < count; ++i) {
                const char* item = StringUnit::getUnit(ItemsTabSeparated, i, "\t");
                if (!item || item[0] == '\0') {
                    continue;
                }
                const bool isSelected = (index == static_cast<S32>(i));
                if (ImGui::Selectable(item, isSelected)) {
                    index = i;
                    valueChanged = true;
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
        }
        ImGui::EndCombo();
    }
    if (valueChanged) {
        Con::setIntVariable(currentItemIndexVarName, index);
    }
    return valueChanged;
}

//  version with \0 separated
// DefineEngineFunction(ImCombo, bool, (
//     const char* label
//     , const char* currentItemIndexVarName
//     , const char* ItemsTabSeparated
//     ,  S32 popup_max_height_in_items),
//     (-1),
//         "return true if changed add a varname as reference. Can be a global variable or a Object field.\n"
//         "@var currentItemIndexVarName is the name of variable where the current item index lives\n"
//         "@var ItemsTabSeparated tab separated list of items \n"
// ) {
//     char buf[1024]{};
//     if (!ConvertTabSeparatedToNullSeparated(ItemsTabSeparated, buf, sizeof(buf))) {
//         Con::errorf("ImCombo Invalid ItemsTabSeparated list!!");
//         return false;
//     }
//     S32 index = Con::getIntVariable(currentItemIndexVarName, 0);
//
//     bool result = ImGui::Combo(label, &index, buf, popup_max_height_in_items);
//     if (result) {
//         Con::setIntVariable(currentItemIndexVarName, index);
//     }
//     return result;
// }

// -----------------------------------------------------------------------------
DefineEngineFunction(ImListBox, bool, (
        const char* label
        , const char* currentItemIndexVarName
        , const char* ItemsTabSeparated
        , S32 height_in_items),
        (-1),
        "return true if changed add a varname as reference. Can be a global variable or a Object field.\n"
        "@var currentItemIndexVarName is the name of variable where the current item index lives\n"
        "@var ItemsTabSeparated tab separated list of items \n"
) {

    S32 index = Con::getIntVariable(currentItemIndexVarName, 0);
    bool valueChanged = false;

    if (!ItemsTabSeparated || ItemsTabSeparated[0] == '\0') {
        return false;
    }
    U32 count = StringUnit::getUnitCount(ItemsTabSeparated, "\t");
    if (ImGui::BeginListBox(label,
            height_in_items > 0
                ? ImVec2(0, height_in_items * ImGui::GetTextLineHeightWithSpacing())
                : ImVec2(0,0)
        )) {
        for (U32 i = 0; i < count; ++i) {
            const char* item = StringUnit::getUnit(ItemsTabSeparated, i, "\t");
            if (!item || item[0] == '\0') continue;// ignore empty
            const bool isSelected = (index == static_cast<S32>(i));
            if (ImGui::Selectable(item, isSelected)) {
                index = i;
                valueChanged = true;
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndListBox();
    }
    if (valueChanged) {
        Con::setIntVariable(currentItemIndexVarName, index);
    }
    return valueChanged;
}
// -----------------------------------------------------------------------------
// =============================================================================
// Popups
// =============================================================================
DefineEngineFunction(ImBeginPopupContextItem, bool, (String contextId, S32 popupFlags),("",0),"") {
    return ImGui::BeginPopupContextItem(!contextId.isEmpty() ? contextId.c_str() : nullptr, popupFlags);
}
DefineEngineFunction(ImBeginPopupContextWindow, bool, (String contextId, S32 popupFlags),("",0),"") {
    return  ImGui::BeginPopupContextWindow(!contextId.isEmpty() ? contextId.c_str() : nullptr, popupFlags);
}

DefineEngineFunction(ImBeginPopup, bool, (String contextId, S32 popupFlags),("",0),"") {
    return ImGui::BeginPopup(!contextId.isEmpty() ? contextId.c_str() : nullptr, popupFlags);
}

DefineEngineFunction(ImBeginPopupModal, bool, (String name, String openVarName, S32 windowFlags),("", 0 ),"")
{
    if (!openVarName.isEmpty()) {
        bool value = Con::getBoolVariable(openVarName.c_str(), true);
        if (ImGui::BeginPopupModal(name.c_str(), &value, windowFlags)) {
            Con::setBoolVariable(openVarName, value);
            return true;
        }
        return false;
    }
    return ImGui::BeginPopupModal(name.c_str());
}


DefineEngineFunction(ImIsPopupOpen, bool, (String contextId, S32 popupFlags),(0),"") {
    return ImGui::IsPopupOpen(contextId.c_str(), popupFlags);
}
DefineEngineFunction(ImOpenPopup, void, (String contextId, S32 popupFlags),("",0),"call to mark popup as open (don't call every frame!).") {
    ImGui::OpenPopup(!contextId.isEmpty() ? contextId.c_str() : nullptr, popupFlags);
}

DefineEngineFunction(ImEndPopup, void, (),,"") { ImGui::EndPopup();}
// -----------------------------------------------------------------------------
// =============================================================================
// Window Menu Bars
// =============================================================================
DefineEngineFunction(ImBeginMenuBar, bool, (), ,
                     "Append to menu-bar of current window (requires ImGuiWindowFlags_MenuBar flag set on parent window).\n"
                     "Only call ImEndMenuBar() if this returns true.\n") {
    return ImGui::BeginMenuBar();
}
DefineEngineFunction(ImEndMenuBar, void, (), ,
                     "Only call if ImBeginMenuBar() returned true.\n") {
    ImGui::EndMenuBar();
}
// =============================================================================
// Main Menu Bars (Full Screen)
// =============================================================================
DefineEngineFunction(ImBeginMainMenuBar, bool, (), ,
                     "Create and append to a full screen menu-bar.\n"
                     "Only call ImEndMainMenuBar() if this returns true.\n") {
    return ImGui::BeginMainMenuBar();
}
DefineEngineFunction(ImEndMainMenuBar, void, (), ,
                     "Only call if ImBeginMainMenuBar() returned true.\n") {
    ImGui::EndMainMenuBar();
}
// =============================================================================
// Sub-Menus
// =============================================================================
DefineEngineFunction(ImBeginMenu, bool, (const char* label, bool enabled), (true),
                     "Create a sub-menu entry. Only call ImEndMenu() if this returns true.\n") {
    return ImGui::BeginMenu(label, enabled);
}
DefineEngineFunction(ImEndMenu, void, (), ,
                     "Only call if ImBeginMenu() returned true.\n") {
    ImGui::EndMenu();
}
// =============================================================================
// Menu Items
// =============================================================================
// Standard variant (returns true when clicked/activated)
DefineEngineFunction(ImMenuItem, bool, (const char* label, const char* shortcut, bool selected, bool enabled), ("", false, true),
                     "Renders a standard menu item. Returns true when activated.\n") {
    const char* shortcutPtr = (shortcut && shortcut[0] != '\0') ? shortcut : nullptr;
    return ImGui::MenuItem(label, shortcutPtr, selected, enabled);
}
// Toggle variant (binds directly to an engine/script variable name)
DefineEngineFunction(ImMenuItemToggle, bool, (const char* label, const char* shortcut, const char* boolVarName, bool enabled), ("", "", true),
                     "Renders a menu item with a checkbox toggle tied to a script variable.\n"
                     "Returns true if the state changed.\n") {
    const char* shortcutPtr = (shortcut && shortcut[0] != '\0') ? shortcut : nullptr;

    bool value = Con::getBoolVariable(boolVarName, false);
    bool result = ImGui::MenuItem(label, shortcutPtr, &value, enabled);

    if (result) {
        Con::setBoolVariable(boolVarName, value);
    }
    return result;
}
// -----------------------------------------------------------------------------
// wrapped Context menu adding menuitems and do a callback
DefineEngineFunction(ImContextMenu, void, (
    const char* contextId
    , const char* ItemsTabSeparated
    , const char* callbackFunctionName),
    ("", ""), // Default to empty string if not provided
                     "Opens a context menu on right-click for the previous widget.\n"
                     "@param contextId Unique ID for the popup (optional)\n"
                     "@param ItemsTabSeparated TAB-separated list of menu items\n"
                     "@param callbackFunctionName Script function called on click (receives index and item name)\n"
) {
    if (!ItemsTabSeparated || ItemsTabSeparated[0] == '\0') {
        return;
    }
    const char* popupId = (contextId && contextId[0] != '\0') ? contextId : nullptr;
    if (ImGui::BeginPopupContextItem(popupId)) {
        U32 count = StringUnit::getUnitCount(ItemsTabSeparated, "\t");

        for (U32 i = 0; i < count; ++i) {
            const char* item = StringUnit::getUnit(ItemsTabSeparated, i, "\t");
            if (!item || item[0] == '\0') {
                continue;
            }
            if (dStrcmp(item, "-") == 0) {
                ImGui::Separator();
                continue;
            }
            if (ImGui::MenuItem(item)) {
                if (callbackFunctionName && callbackFunctionName[0] != '\0') {
                    char indexStr[16];
                    std::snprintf(indexStr, sizeof(indexStr), "%u", i);
                    Con::executef(callbackFunctionName, indexStr, item);
                }
            }
        }
        ImGui::EndPopup();
    }
}
// -----------------------------------------------------------------------------
// another wrapper for menuitems
DefineEngineFunction(ImMenuRows, S32, (const char* ItemsTabSeparated), ,
                     "Renders a TAB-separated list of items as MenuItems.\n"
                     "Returns the index of the clicked item, or -1 if nothing was clicked.\n"
){
    if (!ItemsTabSeparated || ItemsTabSeparated[0] == '\0') {
        return 0;
    }

    U32 count = StringUnit::getUnitCount(ItemsTabSeparated, "\t");
    S32 clickedIndex = -1;

    for (U32 i = 0; i < count; ++i) {
        const char* item = StringUnit::getUnit(ItemsTabSeparated, i, "\t");
        if (!item || item[0] == '\0') {
            continue;
        }
        if (dStrcmp(item, "-") == 0) {
            ImGui::Separator();
            continue;
        }
        if (ImGui::MenuItem(item)) {
            clickedIndex = static_cast<S32>(i);
        }
    }
    return clickedIndex;
}
// =============================================================================
// Mouse Status (Global & Last Item)
// =============================================================================

DefineEngineFunction(ImIsItemHovered, bool, (S32 flags), (0),
                     "Returns true if the last item is hovered by the mouse.\n"
                     "@param flags ImGuiHoveredFlags configuration options.\n") {
    return ImGui::IsItemHovered(flags);
}

DefineEngineFunction(ImIsItemClicked, bool, (S32 mouseButton), (0),
                     "Returns true if the last item was clicked.\n"
                     "@param mouseButton 0 = Left, 1 = Right, 2 = Middle.\n") {
    return ImGui::IsItemClicked(mouseButton);
}

DefineEngineFunction(ImIsItemDoubleClicked, bool, (), ,
                     "Returns true if the last item was double-clicked.\n") {
    // ImGui lacks a direct 'IsItemDoubleClicked', we check hover + double click status
    return ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left);
}

DefineEngineFunction(ImIsMouseClicked, bool, (S32 mouseButton, bool repeat), (0, false),
                     "Returns true if the mouse button went from down to up in this frame globally.\n") {
    return ImGui::IsMouseClicked(mouseButton, repeat);
}

DefineEngineFunction(ImIsMouseDown, bool, (S32 mouseButton), (0),
                     "Returns true if the mouse button is currently held down globally.\n") {
    return ImGui::IsMouseDown(mouseButton);
}

// =============================================================================
// Keyboard & Shortcuts
// =============================================================================

DefineEngineFunction(ImIsKeyDown, bool, (S32 imguiKey), ,
                     "Returns true if the specific ImGuiKey is currently held down.\n"
                     "@param imguiKey The native ImGuiKey enum integer value.\n") {
    return ImGui::IsKeyDown(static_cast<ImGuiKey>(imguiKey));
}

DefineEngineFunction(ImIsKeyPressed, bool, (S32 imguiKey, bool repeat), (true),
                     "Returns true if the specific ImGuiKey was pressed down this frame.\n") {
    return ImGui::IsKeyPressed(static_cast<ImGuiKey>(imguiKey), repeat);
}

DefineEngineFunction(ImShortcut, bool, (S32 imguiKeyChord, S32 flags), (0),
                     "Evaluates a full key chord shortcut (e.g., Ctrl+S).\n"
                     "Automatically handles modifier routing and avoids execution if an input text field is active.\n"
                     "@param imguiKeyChord ImGuiKey combined with modifiers like ImGuiMod_Ctrl.\n") {
    // 1. Separate the raw key from the modifier flags
    // ImGui Key Chords embed modifiers in the upper bits
    ImGuiKey key = static_cast<ImGuiKey>(imguiKeyChord & ~ImGuiMod_Mask_);

    // 2. Check if the main key was pressed down this frame
    if (!ImGui::IsKeyPressed(key)) {
        return false;
    }

    // 3. Extract and verify modifier states
    bool reqCtrl  = (imguiKeyChord & ImGuiMod_Ctrl)  != 0;
    bool reqShift = (imguiKeyChord & ImGuiMod_Shift) != 0;
    bool reqAlt   = (imguiKeyChord & ImGuiMod_Alt)   != 0;
    bool reqSuper = (imguiKeyChord & ImGuiMod_Super) != 0;

    ImGuiIO& io = ImGui::GetIO();

    // Fallback block ensuring active modifiers match the exact request
    if (io.KeyCtrl  != reqCtrl)  return false;
    if (io.KeyShift != reqShift) return false;
    if (io.KeyAlt   != reqAlt)   return false;
    if (io.KeySuper != reqSuper) return false;

    // 4. Guardrail: Prevent execution if the user is typing in a text field
    if (ImGui::GetActiveID() != 0) {
        // If an input text field is focused, do not trigger global shortcuts
        return false;
    }

    return true;
}


// =============================================================================
// Active Focus / State
// =============================================================================

DefineEngineFunction(ImIsItemActive, bool, (), ,
                     "Returns true if the last item is currently active (e.g., being dragged, slider adjusting, input typed into).\n") {
    return ImGui::IsItemActive();
}

DefineEngineFunction(ImIsItemFocused, bool, (), ,
                     "Returns true if the last item has keyboard focus.\n") {
    return ImGui::IsItemFocused();
}

// =============================================================================
// Window DrawList API
// =============================================================================
DefineEngineFunction(ImDrawLine, void, (F32 p1_x, F32 p1_y, F32 p2_x, F32 p2_y, Color4F color, F32 thickness), (1.0f),
                     "Draws a line using raw coordinates and a native Color4F struct.\n") {
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    // Direct translation using your inline helper and ImGui's internal packing
    ImU32 packedColor = ImGui::ColorConvertFloat4ToU32(ToImVec4(color));

    drawList->AddLine(ImVec2(p1_x, p1_y), ImVec2(p2_x, p2_y), packedColor, thickness);
}

DefineEngineFunction(ImDrawRect, void, (F32 min_x, F32 min_y, F32 max_x, F32 max_y, Color4F color, F32 rounding, S32 flags, F32 thickness), (0.0f, 0, 1.0f),
                     "Draws an unfilled rectangle using flat coordinates.\n") {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImU32 packedColor = ImGui::ColorConvertFloat4ToU32(ToImVec4(color));

    drawList->AddRect(ImVec2(min_x, min_y), ImVec2(max_x, max_y), packedColor, rounding, flags, thickness);
}

DefineEngineFunction(ImDrawRectFilled, void, (F32 min_x, F32 min_y, F32 max_x, F32 max_y, Color4F color, F32 rounding, S32 flags), (0.0f, 0),
                     "Draws a filled rectangle using flat coordinates.\n") {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImU32 packedColor = ImGui::ColorConvertFloat4ToU32(ToImVec4(color));

    drawList->AddRectFilled(ImVec2(min_x, min_y), ImVec2(max_x, max_y), packedColor, rounding, flags);
}

DefineEngineFunction(ImDrawCircle, void, (F32 center_x, F32 center_y, F32 radius, Color4F color, S32 segments, F32 thickness), (0, 1.0f),
                     "Draws an unfilled circle using flat coordinates.\n") {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImU32 packedColor = ImGui::ColorConvertFloat4ToU32(ToImVec4(color));

    drawList->AddCircle(ImVec2(center_x, center_y), radius, packedColor, segments, thickness);
}

DefineEngineFunction(ImDrawCircleFilled, void, (F32 center_x, F32 center_y, F32 radius, Color4F color, S32 segments), (0),
                     "Draws a filled circle using flat coordinates.\n") {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImU32 packedColor = ImGui::ColorConvertFloat4ToU32(ToImVec4(color));

    drawList->AddCircleFilled(ImVec2(center_x, center_y), radius, packedColor, segments);
}

DefineEngineFunction(ImDrawText, void, (F32 pos_x, F32 pos_y, Color4F color, const char* text), ,
    "Draws a text string directly onto the current window draw list using flat coordinates.\n"
    "@param pos_x X position on screen.\n"
    "@param pos_y Y position on screen.\n"
    "@param color Native Color4F struct.\n"
    "@param text The string message to draw.\n") {
    if (!text || text[0] == '\0') {
        return;
    }

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImU32 packedColor = ImGui::ColorConvertFloat4ToU32(ToImVec4(color));

    // Renders using the default active font and font size
    drawList->AddText(ImVec2(pos_x, pos_y), packedColor, text);
}

// =============================================================================
// Helper ImGetCursorScreenPos
// =============================================================================
DefineEngineFunction(ImGetCursorScreenPos, Point2F, (), ,
        "Returns the current screen coordinate position of the cursor as a native Point2F.\n")
{
    ImVec2 pos = ImGui::GetCursorScreenPos();
    return Point2F(pos.x, pos.y);
}

// =============================================================================
//  ImGui Tables
// =============================================================================
DefineEngineFunction(ImBeginTable, bool, (const char* str_id, S32 column, S32 flags, F32 outer_size_x, F32 outer_size_y, F32 inner_width), (0, 0.0f, 0.0f, 0.0f),
            "Begins a ImGui table framework.\n"
            "Only call ImEndTable() if this returns true.\n"
            "@param flags Combination of ImGuiTableFlags enums.\n") {
    return ImGui::BeginTable(str_id, column, flags, ImVec2(outer_size_x, outer_size_y), inner_width);
}

DefineEngineFunction(ImEndTable, void, (), ,
        "Only call if ImBeginTable() returned true.\n") {
    ImGui::EndTable();
}

DefineEngineFunction(ImTableSetupColumn, void, (const char* label, S32 flags, F32 init_width_or_weight, S32 user_id), (0, 0.0f, 0),
        "Submits configuration properties for a single column. Must be called before ImTableHeadersRow().\n"
        "@param flags Combination of ImGuiTableColumnFlags enums.\n") {
    ImGui::TableSetupColumn(label, flags, init_width_or_weight, static_cast<ImGuiID>(user_id));
}

DefineEngineFunction(ImTableHeadersRow, void, (), ,
        "Submits a header row layout automatically based on your ImTableSetupColumn definitions.\n") {
    ImGui::TableHeadersRow();
}

DefineEngineFunction(ImTableNextRow, void, (S32 row_flags, F32 min_row_height), (0, 0.0f),
        "Appends a new row to the table grid layout.\n") {
    ImGui::TableNextRow(row_flags, min_row_height);
}

DefineEngineFunction(ImTableSetColumnIndex, bool, (S32 column_index), ,
        "Moves the cell cursor to the specific column index within the current row.\n"
        "Returns true if the column is visible and should be rendered.\n") {
    return ImGui::TableSetColumnIndex(column_index);
}

DefineEngineFunction(ImTableSetupScrollFreeze, void, (S32 cols, S32 rows), ,
        "Freezes specific top rows or left columns so they stay visible when scrolling.\n") {
    ImGui::TableSetupScrollFreeze(cols, rows);
}


// -----------------------------------------------------------------------------
ConsoleFunctionGroupEnd(ImGui);
// -----------------------------------------------------------------------------
