#pragma once
#include <SDL3/SDL.h>
#include <unordered_map>
#include <string>

class AudioManager
{
public:
    AudioManager() = default;
    ~AudioManager();

    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    void Initialize();
    void Shutdown();

    bool LoadSound(const std::string& Name, const std::string& FilePath);
    
    void PlaySound(const std::string& Name, float Volume = 1.0f) const;
    
    void SetMasterVolume(float Volume);

private:
	void LoadAllSoundsFromAssetsDirectory();

    struct SoundData
    {
        Uint8* Buffer = nullptr;
        Uint32 Length = 0;
        SDL_AudioSpec Spec = {};
    };

    SDL_AudioStream* AudioStream = nullptr;
    std::unordered_map<std::string, SoundData> SoundCache;
    float MasterVolume = 0.5f;
};