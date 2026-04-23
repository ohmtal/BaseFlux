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


        BaseFluxDemo::Myst mMyst;

        void OnRender(SDL_Renderer* renderer) {
            mBaseFlux.getTextureManager().render("back.bmp", NULL,NULL);


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

            ImGui::SetNextWindowSize(ImVec2(100.f,100.f), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSizeConstraints(ImVec2(60.f, 60.f), ImVec2(FLT_MAX, FLT_MAX));
            if (ImGui::Begin("Hello Image"))
            {
                ImGui::Image((ImTextureID)mBaseFlux.getTexture("icon.bmp"), ImGui::GetContentRegionAvail());
            }
            ImGui::End();

        }

        void OnEvent(const SDL_Event event) {


            if ( event.type == SDL_EVENT_KEY_UP ) {
                switch ( event.key.key )
                {
                    case SDLK_F1:
                        mBaseFlux.getAudioManager().play("dungeon_witch.wav", 0.8f, true);
                        break;
                    case SDLK_F2:
                        mBaseFlux.getAudioManager().stop("dungeon_witch.wav");
                        break;
                    case SDLK_F3:
                        mBaseFlux.getAudioManager().play("dungeon_witch.wav", 1.8f, false);
                        break;
                    case SDLK_ESCAPE:
                        mBaseFlux.TerminateApplication();
                        break;
                }
            }

        }

        void OnShutDown() {

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

            mBaseFlux.OnRender = [this](SDL_Renderer* renderer) { OnRender(renderer);};
            mBaseFlux.OnEvent  = [this](const SDL_Event event) { OnEvent(event);};
            mBaseFlux.OnShutDown = [this]() { OnShutDown();};

            mMyst.onCollide = [this]() { mBaseFlux.getAudioManager().play("beep.wav", 0.04f,false); };

            // schedule welcome sound
            std::thread([this]() {
                std::this_thread::sleep_for(std::chrono::milliseconds((1000)));
                mBaseFlux.getAudioManager().play("dungeon_witch.wav");
            }).detach();

            // call an invalid sound:
             mBaseFlux.getAudioManager().play("not existing wav");

            // lazy loading and using main wrapper
             mBaseFlux.playSound("sound1.wav");

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
