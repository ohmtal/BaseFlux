#define SOL_ALL_SAFETIES_ON 1

#include "bindings.h"
#include "BaseFlux/Main.h"
#include <SDL3/SDL.h>
#include "draw.h" //FIXME move to BaseFlux/Draw.h

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
