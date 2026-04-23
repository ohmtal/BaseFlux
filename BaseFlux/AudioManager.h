//-----------------------------------------------------------------------------
// Copyright (c) 2026 Thomas Hühn (XXTH)
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
// BaseFlux TextureManager
// Desk: simple AudioManager using filename as key and texture as value
//       the fileName is without the pre path. This is set in Main.
// Fileformats: .WAV
// * loosely put together with parts from OhmFlux
//-----------------------------------------------------------------------------
#pragma once
#include <SDL3/SDL.h>
#include <map>
#include <string>

#include "BaseResourceManager.h"


namespace BaseFlux {
    struct WavData {
        Uint8* buffer = nullptr;
        Uint32 len = 0;
        SDL_AudioStream* stream = nullptr;
        SDL_AudioSpec spec;
    };


    class AudioManager : public BaseResourceManager {
        SDL_AudioDeviceID mAudioDevice = 0;
        std::map<std::string, WavData> mWavMap;

        bool mInitialized = false;
        bool mShutDown = false;

    public:


        AudioManager(Main* main):
        BaseResourceManager(main)
        ,mAudioDevice(0)
        ,mInitialized(false)
        ,mShutDown(false)
        {        }
        ~AudioManager() {
            //shutDown(); << too late!
        }

        bool init();
        const bool isInitialized() { return mInitialized; }
        void shutDown();


        SDL_AudioDeviceID getDeviceID() const { return mAudioDevice; }

        bool bindStream(SDL_AudioStream* stream);
        bool unBindStream(SDL_AudioStream* stream);

        // Global Master Volume
        float getMasterVolume();
        bool setMasterVolume(float value);


        //-----------------------------------------------------------------------------
        bool add(std::string fileName);
        WavData* get(std::string fileName, bool noAutoLoad = false);
        bool stop(std::string fileName);
        bool play(std::string fileName, float gain = 1.0f, bool loop = false);



    }; //class
}; //namespace
