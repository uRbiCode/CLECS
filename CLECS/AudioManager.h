#pragma once
#include <SDL3/SDL.h>
#include <unordered_map>
#include <string>

class AudioManager
{
public:
    ~AudioManager();
    
    void Initialize();
    void Shutdown();
    void LoadSound(const std::string& Name, const std::string& FilePath);
    void PlaySound(const std::string& Name, float Volume) const;
    void SetMasterVolume(float Volume);

private:
    void LoadAllSoundsFromAssetsDirectory();

    struct SoundData
    {
        SDL_AudioSpec Spec;
        Uint8* Buffer;
        Uint32 Length;
    };

    SDL_AudioDeviceID AudioDeviceId = 0;
    float MasterVolume = 1.0f;
    std::unordered_map<std::string, SoundData> SoundCache;
};