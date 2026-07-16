//-----------------------------------------------------------------------------
// Copyright (c) 2026 Thomas Hühn (XXTH)
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
// BaseFlux Main
//-----------------------------------------------------------------------------

#include "Main.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace BaseFlux {
    double gFrameTime = 0.f; // Global for timming
    double gGameTime  = 0.f;
    Uint32 gFPS = 60;

    double getFrameTime() { return gFrameTime;}
    double getGameTime() { return gGameTime;}
    Uint32 getFPS() { return gFPS;}

    //--------------------------------------------------------------------------
    void Main::setFullPath(std::string& path){
        if (path.find("assets:/", 0) != std::string::npos) {
            path = Tools::string_replace_all(path, "assets:/", getSettings().AssetPath);
        }
        if (path.find("sound:/", 0) != std::string::npos) {
            path = Tools::string_replace_all(path, "sound:/", getSettings().AssetPath + getSettings().SoundPathAppend);
        }
        if (path.find("texture:/", 0) != std::string::npos) {
            path = Tools::string_replace_all(path, "texture:/", getSettings().AssetPath + getSettings().TexturePathAppend);
        }
        if (path.find("base:/", 0) != std::string::npos) {
            path = Tools::string_replace_all(path, "base:/", Tools::getBasePath());
        }
        if (path.find("pref:/", 0) != std::string::npos) {
            path = Tools::string_replace_all(path, "pref:/", getSettings().getPrefsPath());
        }
    }
    //--------------------------------------------------------------------------

    //--------------------------------------------------------------------------
    bool Main::setWindowIcon(SDL_Window* window, std::string fileName) {
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
    //--------------------------------------------------------------------------
    bool Main::InitSDL() {
        if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
            SDL_Log("[error] SDL_Init failed: %s", SDL_GetError());
            return false;
        }

        SDL_WindowFlags flags = SDL_WINDOW_RESIZABLE;
        if (getSettings().WindowMaximized)  flags |= SDL_WINDOW_MAXIMIZED;

        if (getSettings().sdlWindowFlagsOverwrite != 0) flags = getSettings().sdlWindowFlagsOverwrite;

        mWindow = SDL_CreateWindow(
            getSettings().Caption.c_str()
            , getSettings().ScreenSize.x
            , getSettings().ScreenSize.y
            , flags);

        if (!mWindow) {
            SDL_Log("[error] SDL_CreateWindow failed: %s", SDL_GetError());
            return false;
        }
        if (getSettings().FullScreen) SDL_SetWindowFullscreen(mWindow, true);

        if (!getSettings().IconFilename.empty()) {
            setWindowIcon(mWindow, getSettings().IconFilename.c_str());
        }

        mRenderer = SDL_CreateRenderer(mWindow, NULL);
        if (!mRenderer) {
            SDL_Log("[error] SDL_CreateRenderer failed: %s", SDL_GetError());
            return false;
        }

        SDL_SetRenderVSync(mRenderer,getSettings().EnableVSync);

        mTextureManager  = std::make_unique<BaseFlux::TextureManager>(this);
        mAudioManager = std::make_unique<BaseFlux::AudioManager>(this);
        if (!mAudioManager->init()) {
            SDL_Log("[error]Failed to init audio!");
        }



        return true;
    }
    //--------------------------------------------------------------------------
    bool Main::initImGui() {
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
    //--------------------------------------------------------------------------
    void Main::shutDown() {
        if (mShutDownComplete) return; //double call safety
        if (OnShutDown) OnShutDown();

        mTextureManager->shutDown();
        mAudioManager->shutDown();


        if ( mImGuiIO ) {
            ImGui_ImplSDLRenderer3_Shutdown();
            ImGui_ImplSDL3_Shutdown();
            ImGui::DestroyContext();
            mImGuiIO = nullptr;
        }

        if (mRenderer) { SDL_DestroyRenderer(mRenderer); mRenderer = nullptr; }
        if (mWindow) {SDL_DestroyWindow(mWindow); mWindow = nullptr;}
        SDL_Quit();
        mShutDownComplete = true;
    }
    //--------------------------------------------------------------------------
    void Main::IterateFrame() {
        bool usingImGui = mImGuiIO != nullptr;

        mTickCount = SDL_GetPerformanceCounter();
        gFrameTime = (double)(mTickCount - mLastTick) / (double)mPerformanceFrequency;


        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (usingImGui)  ImGui_ImplSDL3_ProcessEvent(&event);
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

        SDL_SetRenderDrawColor(mRenderer
        , mSettings.clearColor.r
        , mSettings.clearColor.g
        , mSettings.clearColor.b
        , mSettings.clearColor.a
        );
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



        // FIXME
        // FPS Limiter
        // if ( getSettings().FpsLimit > 0  && frameLimit > 0) {
        //     if (frameTime < frameLimit) {
        //         SDL_Delay(frameLimit - frameTime);
        //     }
        //     frameTime = SDL_GetTicks() - frameStart;
        // }

        // changed to tick style  all 16ms
        static double accumulator = 0.f;
        accumulator += gFrameTime;
        if (accumulator >= 0.016) {

            if (OnUpdate) OnUpdate(accumulator);
            accumulator = 0.f;
        }


        // fps update every 1 second:
        static double lFpsTimer = 0;
        static Uint32 lFrameCounter = 0;

        lFpsTimer += gFrameTime;
        lFrameCounter++;

        if (lFpsTimer >= 1.0f) { // Every 1 second
            gFPS = lFrameCounter;
            lFrameCounter = 0;
            lFpsTimer -= 1.0f;
        }

        gGameTime += gFrameTime;
        //NOTE keep this at last point
        mLastTick = mTickCount;
    }
    //--------------------------------------------------------------------------
#ifdef __EMSCRIPTEN__
    void HandleAudioResume() {
        // This JavaScript snippet checks if the context is suspended and resumes it.
        // In SDL, the AudioContext is usually stored on the 'Module' or 'SDL2' object.
        MAIN_THREAD_EM_ASM({
            if (window.audioContext && window.audioContext.state === 'suspended') {
                window.audioContext.resume();
            }
        });
    }
    void emscripten_loop_wrapper(void* arg)
    {
        if (!arg) {
            SDL_Log("[error] emscripten_loop_wrapper arg is null!")	;
            return;
        }
        // On the first mouse click or key press, try to resume
        static bool audioResumed = false;
        if (!audioResumed) {
            // Check for any SDL input event
            SDL_PumpEvents();
            SDL_Event event;
            if (SDL_PeepEvents(&event, 1, SDL_PEEKEVENT, SDL_EVENT_MOUSE_BUTTON_DOWN, SDL_EVENT_KEY_DOWN) > 0) {
                HandleAudioResume();
                audioResumed = true;
            }
        }
        Main* app = static_cast<Main*>(arg);
        app->IterateFrame();
    }
#endif
    //--------------------------------------------------------------------------
    bool Main::Execute() {
        if (!mWindow || !mRenderer) {
            SDL_Log("[error] Init ImGui failed: InitSDL first!");
            return false;
        }




        //changed to performanceCounter
        // Uint32 frameStart, frameTime;
        // uint16_t frameLimit = 0;

        //FIXME if (getSettings().FpsLimit > 0) frameLimit = static_cast<uint16_t>(1000 / getSettings().FpsLimit);

        mPerformanceFrequency = SDL_GetPerformanceFrequency();
        // INITIALIZE time once here before the loop starts
        mLastTick = SDL_GetPerformanceCounter();

        mRunning = true;


#ifdef __EMSCRIPTEN__
        emscripten_set_main_loop_arg(emscripten_loop_wrapper, this, 0, 1);
        emscripten_set_main_loop_timing(EM_TIMING_RAF, 1); //force RAF
#else
        while (mRunning) {
            IterateFrame();
        }
#endif

        shutDown();
        return true;
    }
    //--------------------------------------------------------------------------
    void Main::TerminateApplication()
    {
        static SDL_Event Q;
        Q.type = SDL_EVENT_QUIT;
        if(!SDL_PushEvent(&Q))
        {
            exit(1);
        }
        return;
    }
    //--------------------------------------------------------------------------
    // Sound Wrapper
    bool Main::playSound(std::string fileName , float gain , bool loop )  {
        return getAudioManager().play(fileName, gain, loop);
    }
    bool Main::stopSound(std::string fileName  ) {
        return getAudioManager().stop(fileName);
    }
    WavData* Main::getSound(std::string fileName, bool noAutoLoad) {
        return getAudioManager().get(fileName, noAutoLoad);
    }
    //--------------------------------------------------------------------------
    // Texture Wrapper
    bool Main::renderTexture(std::string fileName, const SDL_FRect* srcrect, const SDL_FRect* dstrect) {
        return getTextureManager().render(fileName,srcrect, dstrect);

    }
    SDL_Texture* Main::getTexture(std::string fileName, bool noAutoLoad) {
        return getTextureManager().get(fileName, noAutoLoad);
    }
};
