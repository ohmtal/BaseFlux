//-----------------------------------------------------------------------------
// Copyright (c) 2026 Thomas Hühn (XXTH)
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
// BaseFlux Demo
//-----------------------------------------------------------------------------
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "imgui.h"

#include <BaseFlux.h>

int main(int argc, char* argv[]) {
    BaseFlux::Main app;

    app.mSettings = {
        .ScreenSize = { 300, 300},
        .Caption = "BaseFlux minimalistic Demo with Fullscreen Window",
    };
    if ( !app.InitSDL() ) return 1;
    app.initImGui();


    app.OnRender = [&](SDL_Renderer* renderer) {
        // ~~~~ FullscreenWindow ~~~~

        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration
            | ImGuiWindowFlags_NoMove
            | ImGuiWindowFlags_NoResize
            | ImGuiWindowFlags_NoSavedSettings
            | ImGuiWindowFlags_NoBringToFrontOnFocus;

        if (ImGui::Begin("##FullscreenWindow", nullptr, window_flags))
        {
            ImGui::Text("Hello World!");
        }
        ImGui::End();


    };

    app.Execute();

    return 0;
}
