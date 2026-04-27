//-----------------------------------------------------------------------------
// Copyright (c) 2026 Thomas Hühn (XXTH)
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
// Lua / Sol2 Playground
//-----------------------------------------------------------------------------
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "imgui.h"

#include <sol/sol.hpp>

#include "BaseFlux/Main.h"
#include "spice/gui/ImConsole.h"
#include "spice/tools/fluxStr.h"
#include "bindings.h"
#include <format>
#include <iostream>
#include <filesystem>
#include <vector>
namespace fs = std::filesystem;

BaseFlux::Main app;
BaseFlux::Lua::LuaState LuaState;
ImConsole console;


std::vector<fs::path> luaFiles;



//-----------------------------------------------------------------------------
// console redirect ....
void SDLCALL ConsoleLogFunction(void *userdata, int category, SDL_LogPriority priority, const char *message)
{
    char lBuffer[1024];
    snprintf(lBuffer, sizeof(lBuffer), "%s", message);
    console.AddLog("%s", message);
}
//-----------------------------------------------------------------------------
// Test  function ....
void Hello(std::string caption) {
    SDL_Log("Hello %s", caption.c_str());
}

// Test  struct / class
struct foo {
    float myFloat = 5.f;
    bool bar(float value) {
        SDL_Log("BAR! value=%f", value);
        return value > myFloat;
    }
};


//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
void initConsole() {
    SDL_SetLogOutputFunction(ConsoleLogFunction, nullptr);

    console.OnCommand = [&](ImConsole* console, const char* command_line) {
        std::string cmdLineStr = command_line;
        std::string cmd = FluxStr::getWord(cmdLineStr, 0);

        if (cmd == "load" || cmd == "exec" ) {
            std::string file = FluxStr::getWord(cmdLineStr,1);
            if (file != "") {
                if (file.find(".lua") == std::string::npos) {
                    file = file + ".lua";
                }
                LuaState.setScript(file);
            }
            return;
        }


        if (cmd == "rl") {
            LuaState.LoadScript();
            return;
        }


        // Lua commands:
        try {
            if (!LuaState.getLua()) {
                SDL_Log("[error] failed to get LuaState!");
                return;
            }
            auto result =  LuaState.getLua()->safe_script(cmdLineStr, sol::script_pass_on_error);

            if (!result.valid()) {
                sol::error err = result;
                console->AddLog("Error: %s", err.what());
                return;
            }

            if (result.return_count() == 0 || result[0].is<sol::nil_t>()) {
                return;
            }

            sol::object obj = result[0];
            std::string output = (*LuaState.getLua())["tostring"](obj);

            console->AddLog("%s", output.c_str());

        } catch (const std::exception &e) {
            console->AddLog("C++ Exception: %s", e.what());
        }

    };

}
//-----------------------------------------------------------------------------

void initLua() {
    if (LuaState.init(&app)) {
        LuaState.LoadScript();
    }
}



bool fetchLuaFiles() {
    std::string path = BaseFlux::Tools::getBasePath() + "assets/";

    try {
        if (fs::exists(path) && fs::is_directory(path)) {
             for (const auto& entry : fs::recursive_directory_iterator(path)) {
                if (entry.is_regular_file() && entry.path().extension() == ".lua") {
                    // Full path: luaFiles.push_back(entry.path());
                    luaFiles.push_back(fs::relative(entry.path(), path));

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
        if (ImGui::BeginMenu("Scripts")) {
            if (ImGui::MenuItem(std::format("Hot Reload {}", LuaState.getScript()).c_str(),"CTRL+R")) {
                LuaState.LoadScript();
            }

            ImGui::SeparatorText("Files");
            bool selected = false;
            for (const auto& f : luaFiles) {
                selected = LuaState.getScript() == f.string();
                if (ImGui::MenuItem(f.string().c_str(), nullptr, selected)) {
                    LuaState.setScript(f.string());
                }

            }

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
    if (ImGui::IsKeyPressed(ImGuiKey_GraveAccent)) showConsole = !showConsole;
    if (app.getImGuiIO()->KeyCtrl) {
        if (ImGui::IsKeyPressed(ImGuiKey_R)) LuaState.LoadScript();
    }


    console.Draw("Lua Console",&showConsole);
}
//-----------------------------------------------------------------------------
void shutDown() {
    SDL_SetLogOutputFunction(nullptr, nullptr);
}
//-----------------------------------------------------------------------------
bool initApp() {

    //FIXME  onetime   sol::protected_function traceback_handler = lua["debug"]["traceback"];
    // need to reorganize calls so initLua is before the event calls =!
    if ( !app.InitSDL() ) return false;
    app.initImGui();

    app.OnRender = [&](SDL_Renderer* renderer) {
        onDraw(renderer);
        LuaState.OnRender(renderer);


    };

    app.OnUpdate = [&](const float deltaTime) {
        const float fixedStep = 1.0f / 60.0f;
        static float accumulator = 0.0f;

        accumulator += deltaTime;
        while (accumulator >= fixedStep) {
            LuaState.OnUpdate(fixedStep);

            accumulator -= fixedStep;
        }
    };

    app.OnEvent = [&](const SDL_Event event) {

        if (app.getImGuiIO() && app.getImGuiIO()->WantTextInput) {
            if (event.type == SDL_EVENT_KEY_DOWN ||
                event.type == SDL_EVENT_KEY_UP ||
                event.type == SDL_EVENT_TEXT_INPUT) {
                return;
            }
        }
        LuaState.OnEvent(event);
    };


    return true;
}
//-----------------------------------------------------------------------------
int main(int argc, char* argv[]) {

    app.getSettings() = {
        .Company = "BaseFlux",
        .Caption = "LuaFlux",
        .Version = "0.260426.01",
        .SoundPathAppend = "sound/",
        .TexturePathAppend = "texture/"
    };

    if (!initApp()) return 1;

    initLua();
    fetchLuaFiles();
    initConsole();
    SDL_Log("[info] Welcome to BaseFlux Lua Console!");
    app.Execute();
    shutDown();

    return 0;
}
