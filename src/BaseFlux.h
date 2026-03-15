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
        std::string CursorFilename = "";;   // bmp!

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
    // Pathes
    //-----------------------------------------------------------------------------
    inline std::array<std::string, SDL_FOLDER_COUNT> globFolderCache;

    inline std::string getUserFolder(SDL_Folder folder) {
        // Return cached value if already fetched
        if (!globFolderCache[folder].empty()) {
            return globFolderCache[folder];
        }
        // SDL_GetUserFolder returns a pointer to internal memory; DO NOT free it
        const char* rawPath = SDL_GetUserFolder(folder);
        if (rawPath) {
            globFolderCache[folder] = rawPath;
            return globFolderCache[folder];
        }
        return "";
    }


    inline std::string getHomePath()      { return getUserFolder( SDL_FOLDER_HOME ) ; }
    inline std::string getDesktopPath()   { return getUserFolder( SDL_FOLDER_DESKTOP ) ; }
    inline std::string getDocumentsPath() { return getUserFolder( SDL_FOLDER_DOCUMENTS ) ; }
    inline std::string getDownloadPath()  { return getUserFolder( SDL_FOLDER_DOWNLOADS ) ; }
    inline std::string getMusicPath()     { return getUserFolder( SDL_FOLDER_MUSIC ) ; }
    inline std::string getPicturesPath()  { return getUserFolder( SDL_FOLDER_PICTURES ) ; }
    inline std::string getVideosPath()    { return getUserFolder( SDL_FOLDER_VIDEOS ) ; }

    inline std::string getGamePath() {
        static std::string cachedPath = "";
        if (!cachedPath.empty()) return cachedPath;

        const char* rawPath = SDL_GetBasePath();
        if (rawPath) {
            cachedPath = rawPath;
            SDL_free(const_cast<char*>(rawPath));
        }
        return cachedPath;
    }

    // example: dumpPathes(&game->mSettings);
    inline void dumpPathes(Settings* settings=nullptr) {
        if ( settings )
            SDL_Log("Prefs Path:%s", settings->getPrefsPath().c_str());

        SDL_Log("Game Path:%s", getGamePath().c_str());
        SDL_Log("Home Path:%s", getHomePath().c_str());
        SDL_Log("Desktop Path:%s", getDesktopPath().c_str());
        SDL_Log("Document Path:%s", getDocumentsPath().c_str());
        SDL_Log("Download Path:%s", getDownloadPath().c_str());
        SDL_Log("Music Path:%s", getMusicPath().c_str());
        SDL_Log("Video Path:%s", getVideosPath().c_str());
    }


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
