#define SOL_ALL_SAFETIES_ON 1

#include "bindings.h"
#include "BaseFlux/Main.h"
#include <SDL3/SDL.h>

namespace BaseFlux {

    // void bindMain(sol::state& lua) {
    //     // lua.new_usertype<SDL_Window>("SDL_Window");
    //
    //     auto type = lua.new_usertype<BaseFlux::Main>("BaseFlux::Main");
    //     type.set(sol::call_constructor, sol::constructors<BaseFlux::Main()>());
    //     type["getWindow"] = [](BaseFlux::Main& self) -> void* {
    //         return static_cast<void*>(self.getWindow());
    //     };
    //
    //
    //     type["playSound"] = &BaseFlux::Main::playSound;
    //     // type["renderTexture"] = &BaseFlux::Main::renderTexture;
    //     type["renderTexture"] = [](BaseFlux::Main& self, std::string fileName,
    //                                sol::optional<SDL_FRect> src,
    //                                sol::optional<SDL_FRect> dst) {
    //
    //         SDL_FRect* pSrc = src ? &(*src) : nullptr;
    //         SDL_FRect* pDst = dst ? &(*dst) : nullptr;
    //         return self.renderTexture(fileName, pSrc, pDst);
    //     };
    //
    // }

    void bindSDLBasics(sol::state& lua) {

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


}; //namespace
