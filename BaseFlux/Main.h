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

    public:
        Settings& getSettings() { return Settings::getInstance(); }

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


        //--------------------------------------------------------------------------
        /**
         * replace a path with full path
         * %base => SDL_GetBasePath
         * %pref => Settings::getPrefsPath
         */
        void setFullPath(std::string  &path) {
            if (path.find("base:/", 0) != std::string::npos) {
                path = string_replace_all(path, "base:/", getBasePath());
            }
            if (path.find("pref:/", 0) != std::string::npos) {
                path = string_replace_all(path, "pref:/", getSettings().getPrefsPath());
            }
        }
        //--------------------------------------------------------------------------
        // Load a Texture (.bmp,.png) relative to AssetPath
        bool loadTexture(std::string fileName, SDL_Texture*& texture) {
            if (!mRenderer) return false;
            fileName = getSettings().AssetPath + "/" + fileName;
            setFullPath(fileName);
            // SDL_Log("[info] Loading image: %s", fileName.c_str());
            SDL_Surface* surface = SDL_LoadSurface(fileName.c_str());
            if (!surface) {
                SDL_Log("[error] Failed to load texture at %s: %s", fileName.c_str(), SDL_GetError());
                return false;
            }
            texture = SDL_CreateTextureFromSurface(mRenderer, surface);
            SDL_DestroySurface(surface);
            if (!texture) {
                SDL_Log("[error] Texture Create Error: %s", SDL_GetError());
                return false;
            }
            return true;
        }
        //--------------------------------------------------------------------------
        // Set the window icon relative to AssetPath
        bool setWindowIcon(SDL_Window* window, std::string fileName) {
            fileName = getSettings().AssetPath + "/" + fileName;
            setFullPath(fileName);
            // SDL_Log("[info] Loading icon: %s", fileName.c_str());

            SDL_Surface* iconSurface = SDL_LoadSurface(fileName.c_str());
            if (!iconSurface) {
                SDL_Log("[warn] Failed to load icon: %s Error:%s",fileName.c_str(), SDL_GetError());
                return false;
            }
            SDL_SetWindowIcon(window, iconSurface);
            SDL_DestroySurface(iconSurface);
            return true;
        }
        //----------------------------------------------------------------------
        bool InitSDL() {
            if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
                SDL_Log("[error] SDL_Init failed: %s", SDL_GetError());
                return false;
            }

            SDL_WindowFlags flags = SDL_WINDOW_RESIZABLE;
            if (getSettings().WindowMaximized)  flags |= SDL_WINDOW_MAXIMIZED;

            if (getSettings().sdlWindowFlagsOverwrite != 0) flags = getSettings().sdlWindowFlagsOverwrite;

            mWindow = SDL_CreateWindow(
                getSettings().Caption.c_str()
                , getSettings().ScreenSize[0]
                , getSettings().ScreenSize[1]
                , flags);

            if (!mWindow) {
                SDL_Log("[error] SDL_CreateWindow failed: %s", SDL_GetError());
                return false;
            }

            if (!getSettings().IconFilename.empty()) {
                setWindowIcon(mWindow, getSettings().IconFilename.c_str());
            }

            mRenderer = SDL_CreateRenderer(mWindow, NULL);
            if (!mRenderer) {
                SDL_Log("[error] SDL_CreateRenderer failed: %s", SDL_GetError());
                return false;
            }

            SDL_SetRenderVSync(mRenderer,getSettings().EnableVSync);

            return true;
        }
        //----------------------------------------------------------------------
        bool initImGui() {
            if (!mWindow || !mRenderer) {
                SDL_Log("[error] Init ImGui failed: InitSDL first!");
                return false;
            }

            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            mImGuiIO = &ImGui::GetIO();
            if (!getSettings().IniFileName.empty()) {
                setFullPath(getSettings().IniFileName);
                mImGuiIO->IniFilename = getSettings().IniFileName.c_str();
            } else {
                mImGuiIO->IniFilename = nullptr;
            }
            mImGuiIO->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
            mImGuiIO->ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
            ImGui_ImplSDL3_InitForSDLRenderer(mWindow, mRenderer);
            ImGui_ImplSDLRenderer3_Init(mRenderer);

            return true;
        }
        //----------------------------------------------------------------------
        void shutDown() {
            if (mShutDownComplete) return; //double call safety
            if (OnShutDown) OnShutDown();
            ImGui_ImplSDLRenderer3_Shutdown();
            ImGui_ImplSDL3_Shutdown();
            ImGui::DestroyContext();
            if (mRenderer) { SDL_DestroyRenderer(mRenderer); mRenderer = nullptr; }
            if (mWindow) {SDL_DestroyWindow(mWindow); mWindow = nullptr;}
            SDL_Quit();
            mShutDownComplete = true;
        }
        //----------------------------------------------------------------------
        bool Execute() {
            if (!mWindow || !mRenderer) {
                SDL_Log("[error] Init ImGui failed: InitSDL first!");
                return false;
            }

            bool usingImGui = mImGuiIO != nullptr;

            Uint32 frameStart, frameTime;
            uint16_t frameLimit = 0;

            if (getSettings().FpsLimit > 0) frameLimit = static_cast<uint16_t>(1000 / getSettings().FpsLimit);

            mRunning = true;

            while (mRunning) {
                frameStart = SDL_GetTicks();

                SDL_Event event;
                while (SDL_PollEvent(&event)) {
                    if (usingImGui)  ImGui_ImplSDL3_ProcessEvent(&event);
                    switch (event.type) {
                        case SDL_EVENT_QUIT: mRunning = false; break;
                        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                            if (event.window.windowID == SDL_GetWindowID(mWindow)) {
                                mRunning = false;
                            }
                            break;
                    }

                    if (OnEvent)
                        OnEvent(event);
                }

                // ~~~ ImGui Begin ~~~
                if (usingImGui) {

                    ImGui_ImplSDLRenderer3_NewFrame();
                    ImGui_ImplSDL3_NewFrame();
                    ImGui::NewFrame();

                    if (getSettings().EnableDockSpace)
                    {
                        ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode; //<< this makes it transparent
                        mDockSpaceId = ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), dockspace_flags);
                    }

                }

                SDL_SetRenderDrawColor(mRenderer, 45, 45, 45, 255);
                SDL_RenderClear(mRenderer);

                if (OnRender) OnRender(mRenderer);


                // ~~~ ImGui End ~~~
                if (usingImGui) {
                    ImGui::Render();
                    ImDrawData* drawData = ImGui::GetDrawData();
                    if (drawData)
                    {
                        ImGui_ImplSDLRenderer3_RenderDrawData(drawData, mRenderer);
                    }
                }

                SDL_RenderPresent(mRenderer);

                frameTime = SDL_GetTicks() - frameStart;
                // FPS Limiter
                if ( getSettings().FpsLimit > 0  && frameLimit > 0) {
                    if (frameTime < frameLimit) {
                        SDL_Delay(frameLimit - frameTime);
                    }
                }

            }
            shutDown();
            return true;
        }
        //----------------------------------------------------------------------
        void TerminateApplication(void)
        {
            static SDL_Event Q;
            Q.type = SDL_EVENT_QUIT;
            if(!SDL_PushEvent(&Q))
            {
                exit(1);
            }
            return;
        }
    }; //class
}; //namespace
