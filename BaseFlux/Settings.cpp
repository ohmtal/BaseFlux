#include "Settings.h"


namespace BaseFlux {
    //--------------------------------------------------------------------------
    std::string Settings::getPrefsPath(){
        static std::string cachedPath = "";
        if (!cachedPath.empty()) return cachedPath;
        char* rawPath = SDL_GetPrefPath(getSafeCompany().c_str(), getSafeCaption().c_str());
        cachedPath = rawPath ? std::string(rawPath) : "";
        if (rawPath) SDL_free(rawPath);
        return cachedPath;
    }
    //--------------------------------------------------------------------------
    std::string Settings::getSafeCompany(){
        return Tools::sanitizeFilenameWithUnderScores(Company);
    }
    //--------------------------------------------------------------------------
    std::string Settings::getSafeCaption(){
        return Tools::sanitizeFilenameWithUnderScores(Caption);
    }


};

