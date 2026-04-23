#include "AudioManager.h"
#include "Main.h"
namespace BaseFlux {
    //--------------------------------------------------------------------------
    void SDLCALL MyAudioLoopCallback(void *userdata, SDL_AudioStream *stream, int additional_amount, int total_amount) {
        WavData* data = (WavData*)userdata;
        if (total_amount < (int)data->len) {
            SDL_PutAudioStreamData(stream, data->buffer, data->len);
        }
    }
    //--------------------------------------------------------------------------
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

   // void AudioManager::shutDown() {
   //      if (mShutDown) return;
   //      mShutDown = true;
   //      SDL_CloseAudioDevice(mAudioDevice);
   //      mAudioDevice = 0;
   //
   //      for (auto const& [key, val] : mWavMap) {
   //          if (val.buffer) {
   //              SDL_free(val.buffer);
   //          }
   //          if (val.stream) {
   //              SDL_DestroyAudioStream(val.stream);
   //          }
   //      }
   //      mWavMap.clear();
   //      mInitialized = false;
   //  }

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
    bool AudioManager::play(std::string fileName, float gain, bool loop) {
        if (!isInitialized()) return false;
        WavData* data = get(fileName);
        if ( !data || !data->stream) return false;

        SDL_ClearAudioStream(data->stream);

         SDL_SetAudioStreamGain(data->stream, gain);

        if (loop) {
            SDL_SetAudioStreamPutCallback(data->stream, MyAudioLoopCallback, data);
        } else {
            SDL_SetAudioStreamPutCallback(data->stream, nullptr, nullptr);
        }
        SDL_PutAudioStreamData(data->stream, data->buffer, data->len);
        return true;
    }
    //--------------------------------------------------------------------------
    bool AudioManager::stop(std::string fileName) {
        if (!isInitialized()) return false;
        WavData* data = get(fileName);
        if (!data || !data->stream) return false;

        SDL_SetAudioStreamPutCallback(data->stream, nullptr, nullptr);
        SDL_ClearAudioStream(data->stream);

        return true;
    }
    //--------------------------------------------------------------------------
    WavData* AudioManager::get(std::string fileName) {
        auto it = mWavMap.find(fileName);

        if (it != mWavMap.end()) {
            return &it->second; // Adresse des Objekts in der Map zurückgeben
        }
        return nullptr;
    }
    //--------------------------------------------------------------------------
    bool AudioManager::add(std::__1::string fileName)
    {
        if (!isInitialized()) return false;
        if (!mMain) return false;
        if (get(fileName) != nullptr) {
            SDL_Log("[error] deny texture double load: %s!", fileName.c_str());
            return false;
        }

        std::string lFileName = mMain->getSettings().AssetPath + "/" + fileName;
        mMain->setFullPath(lFileName);


        WavData wav_data;
        if (!SDL_LoadWAV(lFileName.c_str(), &wav_data.spec, &wav_data.buffer, &wav_data.len)) {
            SDL_Log("[error]Couldn't load Wavefile:%s file: %s", fileName.c_str(), SDL_GetError());
            return false;
        }



        wav_data.stream = SDL_CreateAudioStream(&wav_data.spec, nullptr);
        if (!wav_data.stream ) {
            SDL_Log("Couldn't create audio stream: %s", SDL_GetError());
            return false;
        }

        if (!bindStream(wav_data.stream )) {
            SDL_Log("Failed to bind '%s' stream to device: %s", fileName.c_str(), SDL_GetError());
            SDL_DestroyAudioStream(wav_data.stream);
            SDL_free(wav_data.buffer);
            return false;
        }

        mWavMap[fileName] = wav_data;
        return true;
    }
    //--------------------------------------------------------------------------


}; //namespace
