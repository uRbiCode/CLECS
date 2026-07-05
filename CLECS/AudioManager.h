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
    void PlaySound(const std::string& Name, float Volume);

    void PlayMusic(const std::string& Name, float Volume);
	bool IsMusicPlaying() const;
    void StopMusic();

private:
    enum class AudioType : uint8_t
    {
        Sfx,
        Music,
        Invalid
	};

    struct SoundData
    {
        Uint8* Buffer = nullptr;
        Uint32 Length = 0;
        SDL_AudioSpec Spec{};
    };

    void LoadAllSoundsFromAssetsDirectory();
    SDL_AudioStream* CreateAndBindAudioStream(const std::string& Name, float Volume, AudioType Type);
    static void SDLCALL MusicCallback(void* Userdata, SDL_AudioStream* Stream, int AdditionalAmount, int TotalAmount) noexcept;

#pragma warning(push)
#pragma warning(disable : 4820)
    std::unordered_map<std::string, SoundData> SoundCache;
    std::string CurrentMusicName;
    SDL_AudioStream* MusicStream = nullptr;
    SDL_AudioDeviceID AudioDeviceId = 0;
#pragma warning(pop)
};