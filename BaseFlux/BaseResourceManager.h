//-----------------------------------------------------------------------------
// Copyright (c) 2026 Thomas Hühn (XXTH)
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
// BaseFlux BaseResourceManager
//-----------------------------------------------------------------------------
#pragma once
#include <vector>
#include <string>
namespace BaseFlux {
    class Main; //fwd
    class BaseResourceManager {
    protected:
        Main* mMain = nullptr;
    private:
        std::vector<std::string> mBlacklist; //blacklisted filename
    public:
         BaseResourceManager(Main* main) : mMain(main) {}

         bool isBlackListed(std::string fileName) {
             return std::find(mBlacklist.begin(), mBlacklist.end(), fileName) != mBlacklist.end();
         }

         void blacklist(std::string fileName) {
             if (!isBlackListed(fileName))
                 mBlacklist.push_back(fileName);
         }


    };

};
