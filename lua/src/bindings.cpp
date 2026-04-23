#include "bindings.h"
#include "BaseFlux/Main.h"
#include <SDL3/SDL.h>

namespace BaseFlux {

    void bindMain(sol::state& lua) {
        // lua.new_usertype<SDL_Window>("SDL_Window");

        auto type = lua.new_usertype<BaseFlux::Main>("BaseFlux::Main");
        type.set(sol::call_constructor, sol::constructors<BaseFlux::Main()>());
        // type["getWindow"] = &BaseFlux::Main::getWindow;
        // type["getWindow"] = [](BaseFlux::Main& self) {return sol::lightuserdata(self.getWindow());};
        type["getWindow"] = [](BaseFlux::Main& self) -> void* {
            return static_cast<void*>(self.getWindow());
        };
    }

    void bindSDLEvents(sol::state& lua) {
        lua.new_usertype<SDL_KeyboardEvent>("SDL_KeyboardEvent",
                                            "key", &SDL_KeyboardEvent::key,
                                            "scancode", &SDL_KeyboardEvent::scancode,
                                            "down", &SDL_KeyboardEvent::down
        );

        lua.new_usertype<SDL_Event>("SDL_Event",
                                    "type", &SDL_Event::type,
                                    "key", [](SDL_Event& ev) { return ev.key; },
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
