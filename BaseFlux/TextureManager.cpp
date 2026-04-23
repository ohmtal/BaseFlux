
#include "TextureManager.h"
#include "Main.h"
namespace BaseFlux {

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
    SDL_Texture* TextureManager::get(std::string fileName) {
        auto it = mTextureMap.find(fileName);

        if (it != mTextureMap.end()) {
            return it->second;
        }
        return nullptr;
    }
    //--------------------------------------------------------------------------
    bool TextureManager::add(const std::string fileName) {
        if (!mMain) return false;
        if (get(fileName) != nullptr) {
            SDL_Log("[error] deny texture double load: %s!", fileName.c_str());
            return false;
        }
        SDL_Texture* tex = nullptr;
        if (!mMain->loadTexture(fileName, tex)) {
            SDL_Log("[error] Error failed to load texture %s!",fileName.c_str());
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
        SDL_RenderTexture(mMain->getRenderer(), texture, srcrect, dstrect);
        return true;
    }


}; //namespace
