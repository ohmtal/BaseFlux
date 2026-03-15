//-----------------------------------------------------------------------------
// Copyright (c) 2026 Thomas Hühn (XXTH)
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
// BaseFlux Demo
//-----------------------------------------------------------------------------
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "imgui.h"
// #include "imgui_impl_sdl3.h"
// #include "imgui_impl_sdlrenderer3.h"

#include <BaseFlux.h>
#include "Myst.h"

BaseFlux::Main baseFluxMain;
BaseFluxDemo::Myst mystObj;
SDL_Texture* backGroundTex = nullptr;

void OnRender(SDL_Renderer* renderer) {
    if (backGroundTex) {
        SDL_RenderTexture(renderer, backGroundTex, NULL, NULL);
    }

    ImGui::SetNextWindowSize(ImVec2(600.f,400.f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSizeConstraints(ImVec2(300.f, 300.f), ImVec2(FLT_MAX, FLT_MAX));
    if (ImGui::Begin("Hello World"))
    {
            ImGui::SetNextItemWidth(80.f);
            ImGui::SliderInt("History", &mystObj.mMaxHistory, 1,1000);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(80.f);
            ImGui::SliderInt("Frameskip", &mystObj.mFrameSkip, 0,10);

            mystObj.RenderBouncingLines();
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
        .WindowMaximized = true,
        .Company = "The Company",
        .Caption = "BaseFlux Demo",
        // IconFilename is append to .AssetsPath
        .IconFilename = "icon.bmp",
        // IniFileName : you may also use something like: "pref:/appgui.ini"
        //               where pref:/ is replaced with your prefs path build be
        //               Company and Caption
        //               If empty no ini file is written
        .IniFileName = "BaseFluxDemo.ini",
    };

    if ( !baseFluxMain.InitSDL() ) return 1;
    baseFluxMain.initImGui();

    if (!baseFluxMain.loadTexture("back.bmp", backGroundTex))
        SDL_Log("Error failed to load background texture!");

    baseFluxMain.OnRender = [&](SDL_Renderer* renderer) { OnRender(renderer);};
    baseFluxMain.OnEvent  = [&](const SDL_Event event) { OnEvent(event);};

    baseFluxMain.Execute();

    if (backGroundTex) SDL_DestroyTexture(backGroundTex);
    return 0;
}
