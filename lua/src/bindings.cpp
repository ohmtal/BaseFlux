
#include "bindings.h"
#include "BaseFlux/Main.h"
#include "BaseFlux/Tools.h"
#include <SDL3/SDL.h>
#include "draw.h" //FIXME move to BaseFlux/Draw.h
#include <string>

namespace BaseFlux::Lua {


    void bindSDLBasics(sol::state& lua) {
        lua.new_usertype<SDL_Color>("SDL_Color",
                                     sol::constructors<SDL_Color(), SDL_Color(int, int, int, int)>(),
                                     "r", &SDL_Color::r,
                                     "g", &SDL_Color::g,
                                     "b", &SDL_Color::b,
                                     "a", &SDL_Color::a
        );
        lua.new_usertype<SDL_FColor>("SDL_FColor",
                                    sol::constructors<SDL_FColor(), SDL_FColor(float, float, float, float)>(),
                                    "r", &SDL_FColor::r,
                                    "g", &SDL_FColor::g,
                                    "b", &SDL_FColor::b,
                                    "a", &SDL_FColor::a
        );


        lua.new_usertype<SDL_FRect>("SDL_FRect",
                                    sol::constructors<SDL_FRect(), SDL_FRect(float, float, float, float)>(),
                                    "x", &SDL_FRect::x,
                                    "y", &SDL_FRect::y,
                                    "w", &SDL_FRect::w,
                                    "h", &SDL_FRect::h
        );
        lua.new_usertype<SDL_FPoint>("SDL_FPoint",
                                    sol::constructors<SDL_FPoint(), SDL_FPoint(float, float)>(),
                                    "x", &SDL_FPoint::x,
                                    "y", &SDL_FPoint::y
        );

        lua.new_usertype<SDL_Point>("SDL_Point",
                                     sol::constructors<SDL_Point(), SDL_Point(int, int)>(),
                                     "x", &SDL_Point::x,
                                     "y", &SDL_Point::y
        );

        lua.new_usertype<SDL_Rect>("SDL_Rect",
                                    sol::constructors<SDL_Rect(), SDL_Rect(int, int, int, int)>(),
                                    "x", &SDL_Rect::x,
                                    "y", &SDL_Rect::y,
                                    "w", &SDL_Rect::w,
                                    "h", &SDL_Rect::h
        );

        lua.set_function("SDL_PointInRect", [](const SDL_Point& p, const SDL_Rect& r) {
            return (bool)SDL_PointInRect(&p, &r);
        });
        lua.set_function("SDL_HasRectIntersection", [](const SDL_Rect& A, const SDL_Rect& B) {
            return (bool)SDL_HasRectIntersection(&A, &B);
        });


        lua.new_usertype<SDL_Vertex>("SDL_Vertex",
                                   sol::constructors<SDL_Vertex(), SDL_Vertex(SDL_FPoint, SDL_FColor, SDL_FPoint)>(),
                                   "position", &SDL_Vertex::position,
                                   "color", &SDL_Vertex::color,
                                   "tex_coord", &SDL_Vertex::tex_coord

        );


        lua.new_usertype<SDL_Texture>("SDL_Texture");
        // lua.new_usertype<SDL_Renderer>("SDL_Renderer")
        // lua.new_usertype<SDL_Renderer>("SDL_Renderer", sol::no_constructor);;

    }

    void bindSDLEvents(sol::state& lua) {
        lua.new_usertype<SDL_KeyboardEvent>("SDL_KeyboardEvent",
                                            "key", &SDL_KeyboardEvent::key,
                                            "scancode", &SDL_KeyboardEvent::scancode,
                                            "down", &SDL_KeyboardEvent::down
        );

        lua.new_usertype<SDL_Event>("SDL_Event",
                                    "type", &SDL_Event::type,
                                    "key", sol::property([](SDL_Event& ev) { return ev.key; }),
                                    // "key", [](SDL_Event& ev) { return ev.key; },
                                    "motion", [](SDL_Event& ev) { return ev.motion; }
        );

    }
    void bindSDLConstants(sol::state& lua) {

        lua["SDL_EVENT_QUIT"] = SDL_EVENT_QUIT;
        lua["SDL_EVENT_KEY_DOWN"] = SDL_EVENT_KEY_DOWN;
        lua["SDL_EVENT_KEY_UP"] = SDL_EVENT_KEY_UP;
        lua["SDL_EVENT_MOUSE_MOTION"] = SDL_EVENT_MOUSE_MOTION;
        lua["SDL_EVENT_MOUSE_BUTTON_DOWN"] = SDL_EVENT_MOUSE_BUTTON_DOWN;

        lua["SDL_EVENT_WINDOW_RESIZED"]     = SDL_EVENT_WINDOW_RESIZED;
        lua["SDL_EVENT_WINDOW_MINIMIZED"]   =  SDL_EVENT_WINDOW_MINIMIZED;
        lua["SDL_EVENT_WINDOW_RESTORED"]    = SDL_EVENT_WINDOW_RESTORED;
        lua["SDL_EVENT_WINDOW_FOCUS_GAINED"] = SDL_EVENT_WINDOW_FOCUS_GAINED;
        lua["SDL_EVENT_WINDOW_FOCUS_LOST"] = SDL_EVENT_WINDOW_FOCUS_LOST;



        // Keycodes (SDLK_...)
        // Lua: sdl.K_SPACE
        sol::table k = lua.create_named_table("sdl");

        k["K_SPACE"] = SDLK_SPACE;
        k["K_RETURN"] = SDLK_RETURN;
        k["K_ESCAPE"] = SDLK_ESCAPE;
        k["K_BACKSPACE"] = SDLK_BACKSPACE;


        k["K_UP"] = SDLK_UP;
        k["K_DOWN"] = SDLK_DOWN;
        k["K_LEFT"] = SDLK_LEFT;
        k["K_RIGHT"] = SDLK_RIGHT;

        // A-Z
        for (char c = 'a'; c <= 'z'; ++c) {
            std::string name = "K_";
            name += (char)std::toupper(c);
            k[name] = (SDL_Keycode)c;
        }

        //  0-9
        for (char c = '0'; c <= '9'; ++c) {
            std::string name = "K_";
            name += c;
            k[name] = (SDL_Keycode)c;
        }
    }
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
bool LuaState::init(BaseFlux::Main* lApp){

    if (!lApp) return false;

    if (mInitialied) {
        SDL_Log("[error] LuaState already initialied");
        return false;
    }
    mApp = lApp;

    mLua.open_libraries(
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

    // ------  set package path
    std::string current_path = mLua["package"]["path"];
    mLua["package"]["path"] = current_path + ";" + BaseFlux::Tools::getBasePath() + "assets/?.lua";
    current_path = mLua["package"]["path"];
    SDL_Log("[info] package path = %s", current_path.c_str());

    // --------- SDK (keyboard) events -----
    bindSDLBasics(mLua);
    bindSDLEvents(mLua);
    bindSDLConstants(mLua);


    mLua["OnRender"] = [&](sol::function func) {
        SDL_Log("[debug] lua called OnRender");
        mLua_OnRender = func;
    };

    // lua["OnUpdate"] = [&](sol::protected_function func) {
    mLua["OnUpdate"] = [&](sol::function func) {
        SDL_Log("[debug] lua called OnUpdate");
        mLua_OnUpdate = func;
    };


    mLua["OnSDLEvent"] = [&](sol::function func) {
        SDL_Log("[debug] lua called OnSDLEvent");
        mLua_OnSDLEvent = func;
    };

    mLua.set_function("drawDebugText", [&](float x, float y, const std::string& text) {
        SDL_RenderDebugText(mApp->getRenderer(), x, y, text.c_str());
    });

    mLua.set_function("setColor", [&](u_int8_t R, u_int8_t G, u_int8_t B, sol::optional<u_int8_t> A) {
        SDL_SetRenderDrawColor(mApp->getRenderer(), R, G, B, A.value_or(255));
    });

    mLua.set_function("setScale", [&](float x, float y) {
        SDL_SetRenderScale(mApp->getRenderer(), x, y);
    });

    // primitives
    mLua.set_function("drawPoint", [&](float x, float y) {
        SDL_RenderPoint(mApp->getRenderer(), x, y);
    });

    mLua.set_function("drawLine", [&](float x1, float y1, float x2, float y2) {
        SDL_RenderLine(mApp->getRenderer(), x1, y1, x2 , y2);
    });


    mLua.set_function("drawRect", [&](SDL_FRect * rect) {
        SDL_RenderRect(mApp->getRenderer(), rect);
    });

    mLua.set_function("drawFillRect", [&](SDL_FRect * rect) {
        SDL_RenderFillRect(mApp->getRenderer(), rect);
    });

    // void DrawCircle(SDL_Renderer *renderer, float radius, SDL_FPoint pos, SDL_Color color, bool fill) {

    mLua.set_function("drawCircle", [&](float radius, SDL_FPoint pos, SDL_Color color, bool fill) {
        BaseFlux::DrawCircle(mApp->getRenderer(), radius, pos, color, fill);
    });

    // void DrawArc(SDL_Renderer *renderer, float radius, float startRad, float endRad, SDL_FPoint pos, SDL_Color color, bool fill) {
    mLua.set_function("drawArc", [&](float radius, float startRad, float endRad, SDL_FPoint pos, SDL_Color color, bool fill) {
        BaseFlux::DrawArc(mApp->getRenderer(), radius,  startRad, endRad, pos, color, fill);
    });
    //     inline void DrawDonut(SDL_Renderer *renderer, float innerRadius, float outerRadius, SDL_FPoint pos, SDL_Color color, bool fill) {
    mLua.set_function("drawDonut", [&](float innerRadius, float outerRadius, SDL_FPoint pos, SDL_Color color, bool fill) {
        BaseFlux::DrawDonut(mApp->getRenderer(),  innerRadius, outerRadius, pos, color, fill);
    });

    mLua.set_function("getWindowSize", [&]() {
        int w, h;
        if (SDL_GetWindowSize(mApp->getWindow(), &w, &h)) {
            return SDL_Point(w,h);
        }
        return SDL_Point(0,0);
    });


    // ------ register sound function ----

    // lua.set_function("playSound", [&](std::string fileName, sol::optional<float> gain = 1.f, sol::optional<bool> loop=false) {
    //     return mApp->playSound(fileName, gain, loop);
    // });
    // THIS for optional parameter =>

    /* TEST:
     *      playSound("sound1.wav", 0.1, true);
     *      stopSound("sound1.wav");
     */

    mLua.set_function("playSound", [&](std::string fileName, sol::optional<float> gain, sol::optional<bool> loop) {
        return mApp->playSound(fileName, gain.value_or(1.0f), loop.value_or(false));
    });
    mLua.set_function("stopSound", [&](std::string fileName) {
        return mApp->stopSound(fileName);
    });

    // ------ register texture function ----
    mLua.set_function("getTexture", [&](std::string file) {
        return mApp->getTextureManager().get(file);
    });
    mLua.set_function("renderTexture", sol::overload(
        // with filename:
        [&](std::string file, sol::optional<SDL_FRect> src, sol::optional<SDL_FRect> dst) {
            return mApp->renderTexture(file, src ? &*src : nullptr, dst ? &*dst : nullptr);
        },
        // with pointer:
        [&](SDL_Texture* texture, sol::optional<SDL_FRect> src, sol::optional<SDL_FRect> dst) {
            return mApp->getTextureManager().render(texture, src ? &*src : nullptr, dst ? &*dst : nullptr);
        }
    ));


    mLua.set_function("quit", [&]() { mApp->TerminateApplication(); });



    // --------- CONSOLE REDIRECT -----------
    // took me ages to find this !
    mLua.set_function("print", [](sol::variadic_args va, sol::this_state s) {
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

    mInitialied = true;
    return true;
}
// -----------------------------------------------------------------------------

bool LuaState::LoadScript() {

    if (!mInitialied) return false;
    auto result = mLua.safe_script_file( BaseFlux::Tools::getBasePath() + "assets/" + mCurrentScript, sol::script_pass_on_error);
    if (!result.valid()) {
        sol::error err = result;
        SDL_Log("[error] SCRIPT %s LOAD ERROR: %s\n", mCurrentScript.c_str(),  err.what());
        return false;
    } else {
        SDL_Log("[info] script %s loaded", mCurrentScript.c_str());
        return true;
    }
}
// -----------------------------------------------------------------------------
void LuaState::OnEvent(const SDL_Event event){
    if (!mInitialied) return;
    if (mLua_OnSDLEvent.valid()) {
        auto result = mLua_OnSDLEvent.call(event/*, traceback_handler*/);

        if (!result.valid()) {
            sol::error err = result;
            SDL_Log("[error] lua: %s", err.what());
        }
    }
}
// -----------------------------------------------------------------------------

void LuaState::OnRender(SDL_Renderer* renderer){
    if (!mInitialied) return;
    if (mLua_OnRender.valid()) {
        auto result = mLua_OnRender.call(/*traceback_handler*/);
        if (!result.valid()) {
            sol::error err = result;
            SDL_Log("[error] lua: %s", err.what());
        }
    }
}
// -----------------------------------------------------------------------------

void LuaState::OnUpdate(const float deltaTime){
    if (!mInitialied) return;
    if (mLua_OnUpdate.valid()) {
        auto result = mLua_OnUpdate.call(deltaTime);
        if (!result.valid()) {
            sol::error err = result;
            SDL_Log("[error] %s", err.what());
        }
    }
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------


}; //namespace
