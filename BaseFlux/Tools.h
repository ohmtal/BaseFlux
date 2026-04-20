//-----------------------------------------------------------------------------
// Copyright (c) 2026 Thomas Hühn (XXTH)
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
// BaseFlux Tools
//-----------------------------------------------------------------------------
#pragma once
#include <SDL3/SDL.h>
#include <string>


namespace BaseFlux::Tools {


    std::string sanitizeFilenameWithUnderScores(std::string name);

    // String replace helper
    std::string string_replace_all(std::string str, const std::string& from, const std::string& to);

    // getBasePath wrapper
    std::string getBasePath();

}; //namespace
