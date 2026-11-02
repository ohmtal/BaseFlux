//-----------------------------------------------------------------------------
// Copyright (c) 2026 Thomas Hühn (XXTH)
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
// DLS Playground
//-----------------------------------------------------------------------------
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "imgui.h"

#include "BaseFlux/Main.h"
#include "spice/gui/ImConsole.h"
#include <format>
#include <iostream>
#include <filesystem>
#include <vector>


namespace fs = std::filesystem;

BaseFlux::Main app;
ImConsole gConsole;


std::vector<fs::path> dlsFiles;
namespace DLSFlux {
    extern void Init();
    extern bool Run(const std::string& command);
    extern size_t getTapePos();
    extern void OnRender();
    bool RunFile(const std::string& fileName, bool ResetTape );
}

#define Log(...) SDL_Log(__VA_ARGS__)

//-----------------------------------------------------------------------------
// console redirect ....
void SDLCALL ConsoleLogFunction(void *userdata, int category, SDL_LogPriority priority, const char *message)
{
    char lBuffer[1024];
    snprintf(lBuffer, sizeof(lBuffer), "%s", message);
    gConsole.AddLog("%s", message);
}
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
void initConsole() {
    SDL_SetLogOutputFunction(ConsoleLogFunction, nullptr);

    gConsole.OnCommand = [&](ImConsole* console, const char* command_line) {
        DLSFlux::Run(command_line);
    };

}
//-----------------------------------------------------------------------------

bool fetchDLSFiles() {
    std::string path = BaseFlux::Tools::getBasePath() + "assets/";

    try {
        if (fs::exists(path) && fs::is_directory(path)) {
             for (const auto& entry : fs::recursive_directory_iterator(path)) {
                if (entry.is_regular_file() && entry.path().extension() == ".dls") {
                    dlsFiles.push_back(fs::relative(entry.path(), path));

                }
            }
        }
    } catch (const fs::filesystem_error& e) {
        SDL_Log("[error]%s",e.what());
        return false;
    }



    return true;
}

//-----------------------------------------------------------------------------
void onDraw(SDL_Renderer* renderer) {

    static bool showConsole = true;
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Window")) {
            ImGui::MenuItem("Console", "GraveAccent", &showConsole);
            ImGui::EndMenu();
        }
        // ImGui::Text(std::format("TAPE:%zu", DLSFlux::vm.tell()).c_str());
        ImGui::Text("[TAPE: %06zu]", DLSFlux::getTapePos());
        if (ImGui::BeginMenu("Scripts")) {
            // if (ImGui::MenuItem(std::format("Hot Reload {}", LuaState.getScript()).c_str(),"CTRL+R")) {
            //     LuaState.LoadScript();
            // }

            ImGui::SeparatorText("Files");
            bool selected = false;
            for (const auto& f : dlsFiles) {
                // selected = LuaState.getScript() == f.string();
                // if (ImGui::MenuItem(f.string().c_str(), nullptr, selected)) {
                //     LuaState.setScript(f.string());
                // }

            }

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
    if (ImGui::IsKeyPressed(ImGuiKey_GraveAccent)) showConsole = !showConsole;
    // // if (app.getImGuiIO()->KeyCtrl) {
    // //     if (ImGui::IsKeyPressed(ImGuiKey_R)) LuaState.LoadScript();
    // // }


    gConsole.Draw("DLS Console",&showConsole);
}
//-----------------------------------------------------------------------------
void shutDown() {
    SDL_SetLogOutputFunction(nullptr, nullptr);
}
//-----------------------------------------------------------------------------
bool initApp() {

    if ( !app.InitSDL() ) return false;
    app.initImGui();

    app.OnRender = [&](SDL_Renderer* renderer) {
        onDraw(renderer);
        DLSFlux::OnRender();
    };

    app.OnUpdate = [&](const float deltaTime) {
        const float fixedStep = 1.0f / 60.0f;
        static float accumulator = 0.0f;

        accumulator += deltaTime;
        while (accumulator >= fixedStep) {
            // // LuaState.OnUpdate(fixedStep);

            accumulator -= fixedStep;
        }
    };

    app.OnEvent = [&](const SDL_Event& event) {

        if (app.getImGuiIO() && app.getImGuiIO()->WantTextInput) {
            if (event.type == SDL_EVENT_KEY_DOWN ||
                event.type == SDL_EVENT_KEY_UP ||
                event.type == SDL_EVENT_TEXT_INPUT) {
                return;
            }
        }
        // // LuaState.OnEvent(event);
    };


    return true;
}
//-----------------------------------------------------------------------------
int main(int argc, char* argv[]) {

    app.getSettings() = {
        .Company = "BaseFlux",
        .Caption = "DLSFlux",
        .Version = "0.261102.01",
        .SoundPathAppend = "sound/",
        .TexturePathAppend = "texture/"
    };

    if (!initApp()) return 1;

    DLSFlux::Init();

    fetchDLSFiles();
    initConsole();
    SDL_Log("[info] Welcome to BaseFlux DLS Console!");
    DLSFlux::RunFile("assets/main.dls", false);
    app.Execute();
    shutDown();

    return 0;
}
