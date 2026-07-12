#include "console/engineAPI.h"
#include "console/consoleExtras.h"
#include "ConsoleTypes.h"

#include <string>
#include <format>

#include "BaseFlux/Main.h"
extern BaseFlux::Main app;

#include "imgui.h"

// -----------------------------------------------------------------------------
inline ImVec4 ToImVec4(const Color4F& color) {
    return ImVec4(color.r, color.g, color.b, color.a);
}
inline ImVec2 ToImVec2(const Point2F& p) {
    return ImVec2(p.x,p.y);
}
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
ConsoleFunctionGroupBegin( ImGui, "ImGui/BaseFlux functions");
// -----------------------------------------------------------------------------
DefineEngineFunction(ImSetNextWindowSize, void, (Point2F size),,"")
{
    ImGui::SetNextWindowSize(ToImVec2(size));
}

// -----------------------------------------------------------------------------
DefineEngineFunction(ImBegin, bool, (String name),,"")
{
    // IMGUI_API bool          Begin(const char* name, bool* p_open = NULL, ImGuiWindowFlags flags = 0);
    return ImGui::Begin(name.c_str());
}
DefineEngineFunction(ImEnd, void, (),,"") { ImGui::End(); }

// -----------------------------------------------------------------------------
DefineEngineFunction(ImText, void, (String text),,"")
{
    ImGui::Text("%s", text.c_str());
}
// -----------------------------------------------------------------------------
DefineEngineFunction(ImTextColored, void, (Color4F color, String text),,"")
{
    ImGui::TextColored(ToImVec4(color), "%s", text.c_str());
}
// -----------------------------------------------------------------------------
ConsoleFunctionGroupEnd(ImGui);
// -----------------------------------------------------------------------------
