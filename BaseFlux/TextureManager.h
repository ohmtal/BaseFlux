//-----------------------------------------------------------------------------
// Copyright (c) 2026 Thomas Hühn (XXTH)
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
// BaseFlux TextureManager
// Desk: simple TextureManager using filename as key and texture as value
//       the fileName is without the pre path. This is set in Main.
// Fileformats: .BMP and .PNG (since SDL3.4)
//-----------------------------------------------------------------------------
#pragma once
#include <SDL3/SDL.h>
#include <map>
#include <string>

#include "Main.h"
#include "BaseResourceManager.h"

namespace BaseFlux {

   class TextureManager: public BaseResourceManager {
      std::map<std::string, SDL_Texture*> mTextureMap;

      public:
         TextureManager(Main* main):BaseResourceManager(main) {}
         ~TextureManager() {
           shutDown();
         }
        void shutDown();
        SDL_Texture* get(std::string fileName);
        bool add(const std::string fileName);
        bool render(const std::string fileName,  const SDL_FRect *srcrect, const SDL_FRect *dstrect);


   }; //class
}; //namespace
