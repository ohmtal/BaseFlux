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
#include "bindings.h"

BaseFlux::Main app;
ImConsole console;
sol::state lua;

sol::protected_function lua_OnSDLEvent;
sol::protected_function lua_OnRender;
sol::protected_function lua_OnUpdate;
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
void LoadScript() {
    // auto result = lua.script_file("assets/main.lua");
    auto result = lua.safe_script_file("assets/main.lua", sol::script_pass_on_error);
    if (!result.valid()) {
        sol::error err = result;
        SDL_Log("[error] SCRIPT LOAD ERROR: %s\n", err.what());
    } else {
        SDL_Log("[info] scipt loaded...");
    }
}


//-----------------------------------------------------------------------------
void initConsole() {
    SDL_SetLogOutputFunction(ConsoleLogFunction, nullptr);

    console.OnCommand = [&](ImConsole* console, const char* cmd) {
        std::string cmdStr = cmd;

        if (cmdStr == "update") {
            lua_OnUpdate.call(8.15f);
            return;
        }


        if (cmdStr == "rl") {
            LoadScript();
            return;
        }

        if (cmdStr == "foo") {
            try {
                lua.safe_script(R"(
                    local f = foo.new()
                    print("myFloat is: " .. f.myFloat)

                    f.myFloat = 10.0
                    local result = f:bar(15.5)

                    print("result: " .. tostring(result))
                )");
            } catch (const sol::error& e) {
                console->AddLog("Lua Error: %s", e.what());
            } catch (const std::exception& e) {
                console->AddLog("C++ Error: %s", e.what());
            }

            return;
        }

        // Lua commands:
        try {

            auto result = lua.safe_script(cmd, sol::script_pass_on_error);

            if (!result.valid()) {
                sol::error err = result;
                console->AddLog("Error: %s", err.what());
                return;
            }

            if (result.return_count() == 0 || result[0].is<sol::nil_t>()) {
                return;
            }

            sol::object obj = result[0];
            std::string output = lua["tostring"](obj);

            console->AddLog("%s", output.c_str());

        } catch (const std::exception &e) {
            console->AddLog("C++ Exception: %s", e.what());
        }

    };

}
//-----------------------------------------------------------------------------

void initLua() {
    // lua.open_libraries(sol::lib::base);
    lua.open_libraries(
        sol::lib::base
        ,sol::lib::package
        ,sol::lib::coroutine
        ,sol::lib::os
        ,sol::lib::math
        ,sol::lib::table
        ,sol::lib::string
        ,sol::lib::debug
        ,sol::lib::io

    );

    // --------- SDK (keyboard) events -----
    BaseFlux::bindSDLBasics(lua);
    BaseFlux::bindSDLEvents(lua);
    BaseFlux::bindSDLConstants(lua);


    lua["OnRender"] = [&](sol::function func) {
        SDL_Log("[debug] lua called OnRender");
        lua_OnRender = func;
    };

    // lua["OnUpdate"] = [&](sol::protected_function func) {
    lua["OnUpdate"] = [&](sol::function func) {
        SDL_Log("[debug] lua called OnUpdate");
        lua_OnUpdate = func;
    };


    lua["OnSDLEvent"] = [&](sol::function func) {
        SDL_Log("[debug] lua called OnSDLEvent");
        lua_OnSDLEvent = func;
    };

    lua.set_function("drawDebugText", [&](float x, float y, const std::string& text) {
        SDL_RenderDebugText(app.getRenderer(), x, y, text.c_str());
    });

    lua.set_function("setColor", [&](u_int8_t R, u_int8_t G, u_int8_t B, sol::optional<u_int8_t> A) {
        SDL_SetRenderDrawColor(app.getRenderer(), R, G, B, A.value_or(255));
    });



    // ------ register sound function ----

    // lua.set_function("playSound", [&](std::string fileName, sol::optional<float> gain = 1.f, sol::optional<bool> loop=false) {
    //     return app.playSound(fileName, gain, loop);
    // });
    // THIS for optional parameter =>

    /* TEST:
      playSound("sound1.wav", 0.1, true);
      stopSound("sound1.wav");
    */

    lua.set_function("playSound", [&](std::string fileName, sol::optional<float> gain, sol::optional<bool> loop) {
        return app.playSound(fileName, gain.value_or(1.0f), loop.value_or(false));
    });
    lua.set_function("stopSound", [&](std::string fileName) {
        return app.stopSound(fileName);
    });

    // ------ register texture function ----
    lua.set_function("getTexture", [&](std::string file) {
        return app.getTextureManager().get(file);
    });
    lua.set_function("renderTexture", sol::overload(
        // with filename:
        [&](std::string file, sol::optional<SDL_FRect> src, sol::optional<SDL_FRect> dst) {
            return app.renderTexture(file, src ? &*src : nullptr, dst ? &*dst : nullptr);
        },
        // with pointer:
        [&](SDL_Texture* texture, sol::optional<SDL_FRect> src, sol::optional<SDL_FRect> dst) {
            return app.getTextureManager().render(texture, src ? &*src : nullptr, dst ? &*dst : nullptr);
        }
    ));


    lua.set_function("HelloLua", &Hello);

    // ----- register struct -----
    lua.new_usertype<foo>("foo",
        // constructor
        sol::constructors<foo()>(),
        // Member var
        "myFloat", &foo::myFloat,
        // Member function
        "bar", &foo::bar
    );


    // took me ages to find this !
    lua.set_function("print", [](sol::variadic_args va, sol::this_state s) {
        std::string out;
        for (sol::object obj : va) {
            obj.push();
            size_t len;
            const char* str = luaL_tolstring(s, -1, &len);
            if (str) {
                out += std::string(str, len) + "    ";
            }
            lua_pop(s, 2);
        }
        SDL_Log("[Lua] %s", out.c_str());
    });


    LoadScript();
}

//-----------------------------------------------------------------------------
void onDraw(SDL_Renderer* renderer) {
    console.Draw("Lua Console",nullptr);
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
        if (lua_OnRender.valid()) {
            auto result = lua_OnRender.call(/*traceback_handler*/);
            if (!result.valid()) {
                sol::error err = result;
                SDL_Log("[error] lua: %s", err.what());
            }
        }

    };

    app.OnUpdate = [&](const float deltaTime) {
        const float fixedStep = 1.0f / 60.0f;
        static float accumulator = 0.0f;

        accumulator += deltaTime;
        while (accumulator >= fixedStep) {
            if (lua_OnUpdate.valid()) {
                auto result = lua_OnUpdate.call(fixedStep);

                if (!result.valid()) {
                    sol::error err = result;
                    SDL_Log("[LUA ERROR] %s", err.what());
                }
            }
            accumulator -= fixedStep;
        }
    };

    app.OnEvent = [&](const SDL_Event event) {

        if (lua_OnSDLEvent.valid()) {
            auto result = lua_OnSDLEvent.call(event/*, traceback_handler*/);

            if (!result.valid()) {
                sol::error err = result;
                SDL_Log("[error] lua: %s", err.what());
            }
        }
    };


    return true;
}
//-----------------------------------------------------------------------------
int main(int argc, char* argv[]) {

    app.getSettings() = {
        .Company = "ScriptFlux",
        .Caption = "LuaFlux",
        .Version = "0.260423.23",
        .SoundPathAppend = "sound/",
        .TexturePathAppend = "texture/"
    };

    if (!initApp()) return 1;

    initLua();
    initConsole();
    SDL_Log("[info] Welcome to Lua Console!");
    app.Execute();
    shutDown();

    return 0;
}
