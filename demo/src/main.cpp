//-----------------------------------------------------------------------------
// Copyright (c) 2026 Thomas Hühn (XXTH)
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
// BaseFlux Demo
//-----------------------------------------------------------------------------
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

#include <BaseFlux.h>

BaseFlux::Main baseFluxMain;

void OnRender(const SDL_Renderer* renderer) {

    ImGui::SetNextWindowSize(ImVec2(600.f,400.f));
    if (ImGui::Begin("Hello BaseFlux"))
    {
            ImGui::TextColored(ImVec4(0.3f ,0.3f ,0.9f, 1.0f), "Hello World");
    }
    ImGui::End();
}

void OnEvent(const SDL_Event event) {

    if ( event.type == SDL_EVENT_KEY_UP ) {
        switch ( event.key.key )
        {
            case SDLK_ESCAPE:
                baseFluxMain.TerminateApplication();
                break;
        }
    }

}

int main(int argc, char* argv[]) {

    baseFluxMain.mSettings = {
        .Company = "Tom's Company",
        .Caption = "BaseFlux Demo",
        .IconFilename = "assets/icon.bmp",
    };

    if ( !baseFluxMain.InitSDL() ) return 1;
    baseFluxMain.initImGui();

    baseFluxMain.OnRender = [&](const SDL_Renderer* renderer) { OnRender(renderer);};
    baseFluxMain.OnEvent  = [&](const SDL_Event event) { OnEvent(event);};

    baseFluxMain.Execute();

    return 0;
}
