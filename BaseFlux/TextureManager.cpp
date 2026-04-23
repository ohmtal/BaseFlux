
#include "TextureManager.h"
#include "Main.h"
namespace BaseFlux {

    bool TextureManager::loadTexture(std::string fileName, SDL_Texture*& texture){
        if (!mMain) return false;
        if (!mMain->getRenderer()) return false;
        fileName = mMain->getSettings().AssetPath + "/" + fileName;
        mMain->setFullPath(fileName);
        // SDL_Log("[info] Loading image: %s", fileName.c_str());
        SDL_Surface* surface = SDL_LoadSurface(fileName.c_str());
        if (!surface) {
            SDL_Log("[error] Failed to load texture at %s: %s", fileName.c_str(), SDL_GetError());
            return false;
        }
        texture = SDL_CreateTextureFromSurface(mMain->getRenderer(), surface);
        SDL_DestroySurface(surface);
        if (!texture) {
            SDL_Log("[error] Texture Create Error: %s", SDL_GetError());
            return false;
        }
        return true;
    }
    //--------------------------------------------------------------------------
    void TextureManager::shutDown(){
        if (mMain == nullptr) return;
        mMain = nullptr;
        for (auto const& [id, texture] : mTextureMap) {
            if (texture != nullptr) {
                SDL_DestroyTexture(texture);
            }
        }
        mTextureMap.clear();
    }
    //--------------------------------------------------------------------------
    SDL_Texture* TextureManager::get(std::string fileName, bool noAutoLoad) {
        auto it = mTextureMap.find(fileName);

        if (it != mTextureMap.end()) {
            return it->second;
        }

        if (noAutoLoad) return nullptr;

        // auto load
        if (isBlackListed(fileName)) return nullptr;
        if (!add(fileName)) return nullptr;

        // lookup again
        {
            auto it = mTextureMap.find(fileName);

            if (it != mTextureMap.end()) {
                return it->second;
            }
        }
        return nullptr;
    }
    //--------------------------------------------------------------------------
    bool TextureManager::add(const std::string fileName) {
        if (!mMain) return false;

        if (isBlackListed(fileName)) {
            SDL_Log("[error] deny blacklisted texture: %s!", fileName.c_str());
            return false;
        }

        if (get(fileName, true) != nullptr) {
            SDL_Log("[error] deny texture double load: %s!", fileName.c_str());
            return false;
        }
        SDL_Texture* tex = nullptr;
        if (!loadTexture(fileName, tex)) {
            SDL_Log("[error] Error failed to load texture %s!",fileName.c_str());
            blacklist(fileName);
            return false;
        }

        mTextureMap[fileName] = tex;
        return true;
    }
    //--------------------------------------------------------------------------
    bool TextureManager::render(const std::string fileName, const SDL_FRect* srcrect, const SDL_FRect* dstrect){
        if (!mMain) return false;
        if (!mMain->getRenderer()) return false;
        SDL_Texture* texture = get(fileName);
        if (!texture) {
            SDL_Log("[error] Invalid Texture Filename: %s", fileName.c_str());
            return false;
        }

        SDL_RenderTexture(mMain->getRenderer(), texture, srcrect, dstrect);
        return true;
    }


}; //namespace
