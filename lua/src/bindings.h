#pragma once
#include <sol/sol.hpp>

namespace BaseFlux {

    void bindMain(sol::state& lua);

    void bindSDLEvents(sol::state& lua);
    void bindSDLConstants(sol::state& lua);
}; //namespace
