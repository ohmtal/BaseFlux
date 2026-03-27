//-----------------------------------------------------------------------------
// Copyright (c) 2026 Thomas Hühn (XXTH)
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
// BaseFlux Settings - Singelton
//-----------------------------------------------------------------------------
#pragma once
#include <SDL3/SDL.h>
#include <string>
#include <array>

#include "Tools.h"

namespace BaseFlux {

    //--------------------------------------------------------------------------
    struct Settings {
        std::array<uint16_t, 2> ScreenSize = { 1152, 648};
        // not the exact fps since i use integer and round it.
        uint16_t  FpsLimit = 0;
        bool WindowMaximized  = false;
        // you also can set FpsLimit
        bool EnableVSync      = true;
        std::string Company = "BaseFlux Company";
        std::string Caption = "BaseFlux Caption";
        std::string Version = "BaseFlux Version 1";

        // your window icon (have to be .bmp or .png)
        std::string IconFilename = "";

        //pre path for IconFilename and loadTexture
        // base:/ is replaced with your BasePath
        std::string AssetPath = "base:/assets/";

        //imgui
        bool EnableDockSpace = true;
        // pref:/ is replaced with your pref Path
        std::string IniFileName = "pref:/appgui.ini";

        // overwrite SDL FLAGS:
        SDL_WindowFlags sdlWindowFlagsOverwrite = 0;

        std::string getPrefsPath() {
            static std::string cachedPath = "";
            if (!cachedPath.empty()) return cachedPath;
            const char* rawPath = SDL_GetPrefPath(getSafeCompany().c_str(), getSafeCaption().c_str());
            cachedPath = rawPath ? std::string(rawPath) : "";
            return cachedPath;
        }


        std::string getSafeCompany() {
            return Tools::sanitizeFilenameWithUnderScores(Company);
        }
        std::string getSafeCaption() {
            return Tools::sanitizeFilenameWithUnderScores(Caption);
        }
    };

}; //namespace
