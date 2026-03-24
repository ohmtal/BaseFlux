//-----------------------------------------------------------------------------
// Copyright (c) 2026 Thomas Hühn (XXTH)
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
// BaseFlux Tools
//-----------------------------------------------------------------------------
#pragma once
#include <SDL3/SDL.h>
#include <string>


namespace BaseFlux {

    //-----------------------------------------------------------------------------
    inline std::string sanitizeFilenameWithUnderScores(std::string name)
    {
        std::string result;
        for (unsigned char c : name) {
            if (std::isalnum(c)) {
                result += c;
            } else if (std::isspace(c)) {
                result += '_';
            }
            // Special characters (like '.') are ignored/dropped here
        }
        return result;
    }
    //--------------------------------------------------------------------------
    // String replace helper
    inline std::string string_replace_all(std::string str, const std::string& from, const std::string& to) {
        size_t start_pos = 0;
        while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length();
        }
        return str;
    }
    //--------------------------------------------------------------------------
    // getBasePath wrapper
    inline std::string getBasePath() {
        static std::string cachedPath = "";
        if (!cachedPath.empty()) return cachedPath;

        const char* rawPath = SDL_GetBasePath();
        if (rawPath) {
            cachedPath = rawPath;
        }
        return cachedPath;
    }

}; //namespace
