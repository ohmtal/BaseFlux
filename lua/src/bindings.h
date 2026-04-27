#pragma once
#include <sol/sol.hpp>

namespace BaseFlux {

    void bindMain(sol::state& lua);
    void bindSDLBasics(sol::state& lua);
    void bindSDLEvents(sol::state& lua);
    void bindSDLConstants(sol::state& lua);

    class Lua{


    public:
        void OnEvent(const SDL_Event event);
        void OnUpdate(const float deltaTime);
        void OnRender();


    };


}; //namespace
