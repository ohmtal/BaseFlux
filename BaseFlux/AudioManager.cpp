#include "AudioManager.h"
#include "Main.h"
namespace BaseFlux {
    //--------------------------------------------------------------------------
    bool AudioManager::init() {
        // if (!mMain) {
        //     SDL_Log("[error] Init failed! Main object not set.");
        //     return false;
        // }

        if (mAudioDevice != 0) {
             SDL_Log("[info] Skipping init - mAudioDevice is set (%d)?!", mAudioDevice);
            return true;
        }


        mAudioDevice = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);

        // if (SDL_GetAudioDeviceFormat(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &mOutputSpec, nullptr)) {
        //     Log("[info] Audio output Hardware: %d Hz, %d Channels, Format id: %d ", mOutputSpec.freq, mOutputSpec.channels, mOutputSpec.format);
        // }
        SDL_Log("[info] Init Audiomanager ID:%d.", mAudioDevice);

        mInitialized = mAudioDevice != 0;

        return mInitialized;
    }
    //--------------------------------------------------------------------------
    void AudioManager::shutDown() {
        if (mShutDown) return;
        mShutDown = true;

        for (auto& [key, val] : mWavMap) {
            if (val.stream) {
                SDL_SetAudioStreamPutCallback(val.stream, nullptr, nullptr);
                unBindStream(val.stream);
                SDL_DestroyAudioStream(val.stream);
                val.stream = nullptr;
            }
            if (val.buffer) {
                SDL_free(val.buffer);
                val.buffer = nullptr;
            }
        }
        mWavMap.clear();

        if (mAudioDevice != 0) {
            SDL_CloseAudioDevice(mAudioDevice);
            mAudioDevice = 0;
        }
    }

    //--------------------------------------------------------------------------
    bool AudioManager::bindStream(SDL_AudioStream* stream)
    {
        if (!isInitialized() || !stream) return false;
        SDL_AudioDeviceID currentDevice = SDL_GetAudioStreamDevice(stream);

        if (currentDevice == mAudioDevice) {
            return true;
        }
        if (currentDevice != 0) {
            SDL_UnbindAudioStream(stream);
        }
        return SDL_BindAudioStream(mAudioDevice, stream);
    }
    //--------------------------------------------------------------------------
    bool AudioManager::unBindStream(SDL_AudioStream* stream)
    {
        if (!isInitialized() || !stream) return false;
        SDL_UnbindAudioStream( stream);
        return (SDL_GetAudioStreamDevice(stream) == 0);
    }
    //--------------------------------------------------------------------------
    bool AudioManager::setMasterVolume(float value){
        if (isInitialized()) {
            return SDL_SetAudioDeviceGain(mAudioDevice, value);
        }
        return false;
    }
    //--------------------------------------------------------------------------
    float AudioManager::getMasterVolume(){
        return mAudioDevice ? SDL_GetAudioDeviceGain(mAudioDevice) : 1.0f;
    }
    //--------------------------------------------------------------------------
    void SDLCALL MyAudioLoopCallback(void *userdata, SDL_AudioStream *stream, int additional_amount, int total_amount) {
        WavData* data = (WavData*)userdata;
        if (additional_amount > 0) {
            SDL_PutAudioStreamData(stream, data->buffer, data->len);
        }
    }

    // 2. In deiner play-Funktion:
    bool AudioManager::play(std::string fileName, float gain, bool loop) {
        if (!isInitialized()) return false;
        WavData* data = get(fileName);

        if (!data || !data->stream)  {
            SDL_Log("[error] Invalid filename: %s", fileName.c_str());
            return false;
        }


        SDL_ClearAudioStream(data->stream);
        SDL_SetAudioStreamGain(data->stream, gain);

        if (loop) {
            SDL_SetAudioStreamGetCallback(data->stream, MyAudioLoopCallback, data);
        } else {
            SDL_SetAudioStreamGetCallback(data->stream, nullptr, nullptr);
        }

        SDL_PutAudioStreamData(data->stream, data->buffer, data->len);
        SDL_ResumeAudioStreamDevice(data->stream);

        return true;
    }

    //--------------------------------------------------------------------------
    bool AudioManager::stop(std::string fileName) {
        if (!isInitialized()) return false;
        WavData* data = get(fileName, false); // no autoload on stop!
        if (!data || !data->stream) return false;

        SDL_SetAudioStreamGetCallback(data->stream, nullptr, nullptr);
        SDL_ClearAudioStream(data->stream);

        return true;
    }
    //--------------------------------------------------------------------------
    WavData* AudioManager::get(std::string fileName, bool noAutoLoad) {
        auto it = mWavMap.find(fileName);

        if (it != mWavMap.end()) {
            return &it->second;
        }

        if (noAutoLoad) return nullptr;

        // auto load
        // checked on add: if (isBlackListed(fileName)) return nullptr;
        if (!add(fileName)) return nullptr;

        // lookup again
        {
            auto it = mWavMap.find(fileName);

            if (it != mWavMap.end()) {
                return &it->second;
            }
        }

        return nullptr;
    }
    //--------------------------------------------------------------------------
    bool AudioManager::add(std::string fileName)
    {
        if (!isInitialized()) return false;
        if (!mMain) return false;
        if (isBlackListed(fileName)) {
            SDL_Log("[error] deny texture it's blacklisted: %s!", fileName.c_str());
            return false;
        }


        if (get(fileName, true) != nullptr) {
            SDL_Log("[error] deny texture double load: %s!", fileName.c_str());
            return false;
        }

        std::string lFileName = mMain->getSettings().AssetPath + "/" + fileName;
        mMain->setFullPath(lFileName);


        WavData wav_data;
        if (!SDL_LoadWAV(lFileName.c_str(), &wav_data.spec, &wav_data.buffer, &wav_data.len)) {
            SDL_Log("[error]Couldn't load Wavefile:%s file: %s", fileName.c_str(), SDL_GetError());
            blacklist(fileName);
            return false;
        }



        wav_data.stream = SDL_CreateAudioStream(&wav_data.spec, nullptr);
        if (!wav_data.stream ) {
            SDL_Log("Couldn't create audio stream: %s", SDL_GetError());
            blacklist(fileName);
            return false;
        }

        if (!bindStream(wav_data.stream )) {
            SDL_Log("Failed to bind '%s' stream to device: %s", fileName.c_str(), SDL_GetError());
            SDL_DestroyAudioStream(wav_data.stream);
            SDL_free(wav_data.buffer);
            blacklist(fileName);
            return false;
        }

        mWavMap[fileName] = wav_data;
        return true;
    }
    //--------------------------------------------------------------------------


}; //namespace
