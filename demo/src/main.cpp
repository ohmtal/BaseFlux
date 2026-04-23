//-----------------------------------------------------------------------------
// Copyright (c) 2026 Thomas Hühn (XXTH)
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
// BaseFlux Demo
//-----------------------------------------------------------------------------
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "imgui.h"

#include "BaseFlux/Main.h"
#include "BaseFlux/TextureManager.h"
#include "BaseFlux/AudioManager.h"
#include "Myst.h"
#include <map>
#include <vector>
#include <algorithm>
#include <thread>
#include <chrono>


namespace BaseFluxDemo {
    class Demo {
    private:
        BaseFlux::Main mBaseFlux;

        std::unique_ptr<BaseFlux::TextureManager> mTextureManager;
        std::unique_ptr<BaseFlux::AudioManager> mAudioManager;

        BaseFluxDemo::Myst mMyst;
        // std::map<std::string, SDL_Texture*> mTextureMap;

        void loadAssets() {
            std::vector<std::string> textures = {
                "back.bmp"
            };
            for (int i = 0; i < textures.size(); i++) {
                mTextureManager->add(textures[i]);
            }

            std::vector<std::string> waves = {
                "dungeon_witch.wav"
                , "beep.wav"
            };
            for (int i = 0; i < waves.size(); i++) {
                mAudioManager->add(waves[i]);
            }


        }

        void OnRender(SDL_Renderer* renderer) {
            mTextureManager->render("back.bmp", NULL,NULL);
            // auto it = mTextureMap.find("back.bmp");
            // if (it != mTextureMap.end()) {
            //     SDL_RenderTexture(renderer, it->second, NULL, NULL);
            // }

            ImGui::SetNextWindowSize(ImVec2(600.f,400.f), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSizeConstraints(ImVec2(300.f, 300.f), ImVec2(FLT_MAX, FLT_MAX));
            if (ImGui::Begin("Hello World"))
            {
                ImGui::SetNextItemWidth(80.f);
                ImGui::SliderInt("History", &mMyst.mMaxHistory, 1,1000);
                ImGui::SameLine();
                ImGui::SetNextItemWidth(80.f);
                ImGui::SliderInt("Frameskip", &mMyst.mFrameSkip, 0,10);

                mMyst.RenderBouncingLines();
            }
            ImGui::End();

        }

        void OnEvent(const SDL_Event event) {


            if ( event.type == SDL_EVENT_KEY_UP ) {
                switch ( event.key.key )
                {
                    case SDLK_F1:
                        mAudioManager->play("dungeon_witch.wav");
                        break;
                    case SDLK_ESCAPE:
                        mBaseFlux.TerminateApplication();
                        break;
                }
            }

        }

        void OnShutDown() {
            mTextureManager->shutDown();
            mAudioManager->shutDown();
            // for (auto const& [id, texture] : mTextureMap) {
            //     if (texture != nullptr) {
            //         SDL_DestroyTexture(texture);
            //     }
            // }
            // mTextureMap.clear();
        }

    public:
        Demo() = default;
        ~Demo() = default;

        bool init() {
            mBaseFlux.getSettings() = {
                .WindowMaximized = true,
                .Company = "The Company",
                .Caption = "BaseFlux Demo",
                // IconFilename is append to .AssetsPath
                .IconFilename = "icon.bmp",
                // IniFileName : you may also use something like: "pref:/appgui.ini"
                //               where pref:/ is replaced with your prefs path build be
                //               Company and Caption
                //               If empty no ini file is written
                .IniFileName = "BaseFluxDemo.ini",
            };

            if ( !mBaseFlux.InitSDL() ) return false;
            mBaseFlux.initImGui();

            mTextureManager  = std::make_unique<BaseFlux::TextureManager>(&mBaseFlux);
            mAudioManager = std::make_unique<BaseFlux::AudioManager>(&mBaseFlux);
            if (!mAudioManager->init()) {
               SDL_Log("[error]Failed to init audio!");
            }

            loadAssets();
            mBaseFlux.OnRender = [this](SDL_Renderer* renderer) { OnRender(renderer);};
            mBaseFlux.OnEvent  = [this](const SDL_Event event) { OnEvent(event);};
            mBaseFlux.OnShutDown = [this]() { OnShutDown();};

            mMyst.onCollide = [this]() { mAudioManager->play("beep.wav", 0.02f,false); };

            // schedule welcome sound
            std::thread([this]() {
                std::this_thread::sleep_for(std::chrono::milliseconds((200)));
                mAudioManager->play("dungeon_witch.wav");
            }).detach();

            // call an invalid sound:
             mAudioManager->play("not existing wav");

            return true;

        }
        void Execute() {
            mBaseFlux.Execute();
        }

    }; //class
}; //namespace


int main(int argc, char* argv[]) {
    BaseFluxDemo::Demo demo;

    if (!demo.init())
        return 1;
    demo.Execute();

    return 0;
}
