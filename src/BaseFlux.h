//-----------------------------------------------------------------------------
// Copyright (c) 2026 Thomas Hühn (XXTH)
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
// BaseFlux Header
//-----------------------------------------------------------------------------
#pragma once
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"


#include <string>
#include <array>
#include <memory>
#include <functional>

namespace BaseFlux {

    //--------------------------------------------------------------------------
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
    inline bool loadTexture(SDL_Renderer* renderer, const char* fileName, SDL_Texture*& texture) {
        SDL_Log("Loading image: %s", fileName);
        const char* basePath = SDL_GetBasePath();
        if (!basePath) {
            SDL_Log("Could not get base path: %s", SDL_GetError());
            return false;
        }
        char pathBuff[256];
        snprintf(pathBuff, sizeof(pathBuff), "%s/%s", basePath, fileName);
        SDL_Surface* surface = SDL_LoadBMP(pathBuff);
        if (!surface) {
            SDL_Log("Failed to load BMP at %s: %s", pathBuff, SDL_GetError());
            return false;
        }
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_DestroySurface(surface);
        if (!texture) {
            SDL_Log("Texture Create Error: %s", SDL_GetError());
            return false;
        }
        return true;
    }
    //--------------------------------------------------------------------------
    // Helper function to set the window icon
    inline bool setWindowIcon(SDL_Window* window, const char* fileName) {
        const char* basePath = SDL_GetBasePath();
        if (!basePath) return false;
        char pathBuff[256];
        snprintf(pathBuff, sizeof(pathBuff), "%s/%s", basePath, fileName);
        SDL_Surface* iconSurface = SDL_LoadBMP(pathBuff);
        if (!iconSurface) {
            SDL_Log("[warn]Failed to load icon: %s Error:%s",fileName, SDL_GetError());
            return false;
        }
        SDL_SetWindowIcon(window, iconSurface);
        SDL_DestroySurface(iconSurface);
        return true;
    }

    //-----------------------------------------------------------------------------
    // Settings
    //-----------------------------------------------------------------------------
    struct Settings {
        std::array<uint16_t, 2> ScreenSize = { 1152, 648};
        uint16_t  FpsLimit = 0; // not exactly since i use integer and round it.
        bool WindowMaximized  = false;
        bool EnableVSync      = true; //you can set fps limit if you disable it
        std::string Company = "BaseFlux Company";
        std::string Caption = "BaseFlux Caption";
        std::string Version = "BaseFlux Version 1";

        std::string IconFilename = "";      // bmp!

        //imgui
        bool EnableDockSpace = true;
        const char* IniFileName = "imGui.ini";

        std::string getPrefsPath() {
            std::unique_ptr<char, void(*)(void*)> rawPath(
                SDL_GetPrefPath(getSafeCompany().c_str(), getSafeCaption().c_str()),
                                                          SDL_free
            );
            return rawPath ? std::string(rawPath.get()) : "";
        }

        std::string getSafeCompany() {
            return sanitizeFilenameWithUnderScores(Company);
        }
        std::string getSafeCaption() {
            return sanitizeFilenameWithUnderScores(Caption);
        }

    };

    //-----------------------------------------------------------------------------
    // BaseFlux Main Class
    //-----------------------------------------------------------------------------
    class Main {
    protected:
        SDL_Window* mWindow = nullptr;
        SDL_Renderer* mRenderer = nullptr;

        ImGuiIO* mImGuiIO = nullptr;
        ImGuiID  mDockSpaceId = -1;



    public:
        Settings mSettings;
        bool     mRunning = false;



        Main() = default;
        ~Main()  = default;
        //----------------------------------------------------------------------
        SDL_Window*     getWindow()      const { return mWindow; };
        SDL_Renderer*   getRenderer()    const { return mRenderer; }

        ImGuiIO*        getImGuiIO()     const { return mImGuiIO; }
        ImGuiID         getDockSpaceId() const { return mDockSpaceId; }

        std::function<void(const SDL_Renderer*)> OnRender = nullptr;
        std::function<void(const SDL_Event)> OnEvent = nullptr;

        //----------------------------------------------------------------------
        bool InitSDL() {
            if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
                SDL_Log("[error] SDL_Init failed: %s", SDL_GetError());
                return false;
            }

            SDL_WindowFlags flags = SDL_WINDOW_RESIZABLE;
            if (mSettings.WindowMaximized)  flags |= SDL_WINDOW_MAXIMIZED;
            mWindow = SDL_CreateWindow(
                mSettings.Caption.c_str()
                , mSettings.ScreenSize[0]
                , mSettings.ScreenSize[1]
                , flags);

            if (!mWindow) {
                SDL_Log("[error] SDL_CreateWindow failed: %s", SDL_GetError());
                return false;
            }

            if (!mSettings.IconFilename.empty()) {
                setWindowIcon(mWindow, mSettings.IconFilename.c_str());
            }

            mRenderer = SDL_CreateRenderer(mWindow, NULL);
            if (!mRenderer) {
                SDL_Log("SDL_CreateRenderer failed: %s", SDL_GetError());
                return false;
            }

            SDL_SetRenderVSync(mRenderer,mSettings.EnableVSync);

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
            mImGuiIO->IniFilename = mSettings.IniFileName;
            mImGuiIO->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
            mImGuiIO->ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
            ImGui_ImplSDL3_InitForSDLRenderer(mWindow, mRenderer);
            ImGui_ImplSDLRenderer3_Init(mRenderer);

            return true;
        }
        //----------------------------------------------------------------------
        void shutDown() {
            ImGui_ImplSDLRenderer3_Shutdown();
            ImGui_ImplSDL3_Shutdown();
            ImGui::DestroyContext();
            if (mRenderer) SDL_DestroyRenderer(mRenderer);
            if (mWindow) SDL_DestroyWindow(mWindow);
            SDL_Quit();
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

            if (mSettings.FpsLimit > 0) frameLimit = static_cast<uint16_t>(1000 / mSettings.FpsLimit);

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

                    if (mSettings.EnableDockSpace)
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
                if ( mSettings.FpsLimit > 0  && frameLimit > 0) {
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

    }; //class BaseFlux


}; //namespace
