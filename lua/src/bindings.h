#pragma once
#include <sol/sol.hpp>
#include <SDL3/SDL.h>

namespace BaseFlux {
    class Main;
}

namespace BaseFlux::Lua {


    void bindMain(sol::state& lua);
    void bindSDLBasics(sol::state& lua);
    void bindSDLEvents(sol::state& lua);
    void bindSDLConstants(sol::state& lua);



    class LuaState{
        bool mInitialied = false;
        BaseFlux::Main* mApp;
        sol::state mLua;
        sol::protected_function mLua_OnSDLEvent;
        sol::protected_function mLua_OnRender;
        sol::protected_function mLua_OnUpdate;
        std::string mCurrentScript = "main.lua";
    public:
        const std::string getScript() {
            return mCurrentScript;

        }
        void setScript(std::string script) {
            mCurrentScript = script;
            LoadScript();
        }
        sol::state* getLua() { return &mLua; }
        bool init(BaseFlux::Main* lApp);
        bool LoadScript();
        void OnEvent(const SDL_Event event);
        void OnUpdate(const float deltaTime);
        void OnRender(SDL_Renderer* renderer);


    };


}; //namespace
