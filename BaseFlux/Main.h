//-----------------------------------------------------------------------------
// Copyright (c) 2026 Thomas Hühn (XXTH)
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
// BaseFlux Main
//-----------------------------------------------------------------------------
#pragma once
#include <SDL3/SDL.h>
// #include <SDL3/SDL_main.h>
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

#include "Tools.h"
#include "Settings.h"
#include "AudioManager.h"
#include "TextureManager.h"

#include <string>
#include <memory>
#include <functional>

namespace BaseFlux {

    class Main {
    protected:
        SDL_Window* mWindow = nullptr;
        SDL_Renderer* mRenderer = nullptr;

        ImGuiIO* mImGuiIO = nullptr;
        ImGuiID  mDockSpaceId = -1;

        bool mShutDownComplete = false;
        bool mRunning = false;

        Settings mSettings;

        std::unique_ptr<BaseFlux::TextureManager> mTextureManager;
        std::unique_ptr<AudioManager> mAudioManager;


    public:
        Settings& getSettings() { return mSettings; }

        BaseFlux::TextureManager& getTextureManager() const {
            return *mTextureManager;
        }
        AudioManager& getAudioManager() const {
            return *mAudioManager;
        }

        Main() = default;
        ~Main()  = default;
        //----------------------------------------------------------------------
        SDL_Window*     getWindow()      const { return mWindow; };
        SDL_Renderer*   getRenderer()    const { return mRenderer; }

        ImGuiIO*        getImGuiIO()     const { return mImGuiIO; }
        ImGuiID         getDockSpaceId() const { return mDockSpaceId; }

        std::function<void(SDL_Renderer*)> OnRender = nullptr;
        std::function<void(const SDL_Event)> OnEvent = nullptr;
        std::function<void()> OnShutDown = nullptr;
        //----------------------------------------------------------------------
        // wraper for getAudioManager().play and getTextureManager.render:
        bool playSound(std::string fileName );
        bool renderImage(std::string fileName , const SDL_FRect *srcrect, const SDL_FRect *dstrect);
        //----------------------------------------------------------------------
        /**
         * replace a path with full path
         * %base => SDL_GetBasePath
         * %pref => Settings::getPrefsPath
         */
        void setFullPath(std::string  &path);


        // Set the window icon relative to AssetPath
        bool setWindowIcon(SDL_Window* window, std::string fileName);

        bool InitSDL();
        bool initImGui();
        void shutDown();
        bool Execute();
        void TerminateApplication(void);
    }; //class
}; //namespace
