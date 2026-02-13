#pragma once
#include <SDL3/SDL.h>
#include <unordered_map>
#include <string>

/* Responsible for managing audio in the game.
 * Interacts with SDL to set up audio resources.
 * Provides API to play sound effects and music independently.
 */
class AudioManager
{
public:
    ~AudioManager();
    
    void Initialize();
    void Shutdown();
    void LoadSound(const std::string& Name, const std::string& FilePath);
    void PlaySound(const std::string& Name, float Volume = 1.f);
    void SetMasterVolume(float Volume);

    void PlayMusic(const std::string& Name, float Volume = 1.f);
	bool IsMusicPlaying() const;
    void StopMusic();

private:
    void LoadAllSoundsFromAssetsDirectory();

    struct SoundData
    {
        SDL_AudioSpec Spec;
        Uint8* Buffer;
        Uint32 Length;
    };

    SDL_AudioDeviceID AudioDeviceId = 0;
    float MasterVolume = 1.f;
    std::unordered_map<std::string, SoundData> SoundCache;

    SDL_AudioStream* MusicStream = nullptr;
    std::string CurrentMusicName;

    static void SDLCALL MusicCallback(void* Userdata, SDL_AudioStream* Stream, int AdditionalAmount, int TotalAmount);

    struct StreamConfig
    {
        bool Loop = false;
        bool AutoCleanup = true;
        SDL_AudioStreamCallback Callback = nullptr;
    };

    SDL_AudioStream* CreateAndBindAudioStream(const std::string& Name, float Volume, const StreamConfig& Config);
};