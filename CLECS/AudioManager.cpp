#include "AudioManager.h"
#include <algorithm>
#include <filesystem>

namespace
{
    constexpr const char* SoundsDirectory = "../Assets/Sounds/";
}

AudioManager::~AudioManager()
{
    Shutdown();
}

void AudioManager::Initialize()
{
    constexpr SDL_AudioSpec DesiredSpec = {SDL_AUDIO_F32, 2, 44100};
    AudioDeviceId = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &DesiredSpec);

    if (AudioDeviceId == 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "AudioManager::Initialize -> Failed to open audio device: %s", SDL_GetError());
        return;
    }

    SDL_ResumeAudioDevice(AudioDeviceId);

    LoadAllSoundsFromAssetsDirectory();
}

void AudioManager::Shutdown()
{
	StopMusic();

    for (auto& [Name, Sound] : SoundCache)
    {
        if (Sound.Buffer != nullptr)
        {
            SDL_free(Sound.Buffer);
        }
    }
    SoundCache.clear();

    if (AudioDeviceId != 0)
    {
        SDL_CloseAudioDevice(AudioDeviceId);
        AudioDeviceId = 0;
    }

    if (MusicStream != nullptr)
    {
        SDL_DestroyAudioStream(MusicStream);
        MusicStream = nullptr;
	}
}

void AudioManager::LoadSound(const std::string& Name, const std::string& FilePath)
{
    if (SoundCache.find(Name) != SoundCache.end())
        return;

    SoundData SoundData;

    if (!SDL_LoadWAV(FilePath.c_str(), &SoundData.Spec, &SoundData.Buffer, &SoundData.Length))
    {
        SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "AudioManager::LoadSound -> Failed to load sound '%s' from '%s': %s", Name.c_str(), FilePath.c_str(), SDL_GetError());
        return;
    }

    SoundCache[Name] = SoundData;
    SDL_LogInfo(SDL_LOG_CATEGORY_AUDIO, "AudioManager::LoadSound -> Loaded sound '%s' (%u bytes, %d Hz, %d channels)", Name.c_str(), SoundData.Length, SoundData.Spec.freq, SoundData.Spec.channels);
}

SDL_AudioStream* AudioManager::CreateAndBindAudioStream(const std::string& Name, float Volume, const StreamConfig& Config)
{
    if (AudioDeviceId == 0)
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_AUDIO, "AudioManager::CreateAndBindAudioStream -> Audio device not initialized");
        return nullptr;
    }

    const auto It = SoundCache.find(Name);
    if (It == SoundCache.end())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_AUDIO, "AudioManager::CreateAndBindAudioStream -> Audio '%s' not found", Name.c_str());
        return nullptr;
    }

    const SoundData& SoundData = It->second;

    SDL_AudioSpec DeviceSpec;
    if (!SDL_GetAudioDeviceFormat(AudioDeviceId, &DeviceSpec, nullptr))
    {
        SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "AudioManager::CreateAndBindAudioStream -> Failed to get audio device format: %s", SDL_GetError());
        return nullptr;
    }

    SDL_AudioStream* Stream = SDL_CreateAudioStream(&SoundData.Spec, &DeviceSpec);
    if (Stream == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "AudioManager::CreateAndBindAudioStream -> Failed to create audio stream: %s", SDL_GetError());
        return nullptr;
    }

    if (Config.Callback != nullptr)
    {
        if (!SDL_SetAudioStreamGetCallback(Stream, Config.Callback, this))
        {
            SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "AudioManager::CreateAndBindAudioStream -> Failed to set audio callback: %s", SDL_GetError());
            SDL_DestroyAudioStream(Stream);
            return nullptr;
        }
    }

    if (Config.AutoCleanup)
    {
        SDL_PropertiesID StreamPropertiesId = SDL_GetAudioStreamProperties(Stream);
        if (StreamPropertiesId != 0)
        {
            SDL_SetBooleanProperty(StreamPropertiesId, SDL_PROP_AUDIOSTREAM_AUTO_CLEANUP_BOOLEAN, true);
        }
    }

    SDL_SetAudioStreamGain(Stream, Volume);
    
    if (!SDL_PutAudioStreamData(Stream, SoundData.Buffer, SoundData.Length))
    {
        SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "AudioManager::CreateAndBindAudioStream -> Failed to put audio data: %s", SDL_GetError());
        SDL_DestroyAudioStream(Stream);
        return nullptr;
    }
    
    if (!SDL_FlushAudioStream(Stream))
    {
        SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "AudioManager::CreateAndBindAudioStream -> Failed to flush audio stream: %s", SDL_GetError());
        SDL_DestroyAudioStream(Stream);
        return nullptr;
    }

    if (!SDL_BindAudioStream(AudioDeviceId, Stream))
    {
        SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "AudioManager::CreateAndBindAudioStream -> Failed to bind audio stream: %s", SDL_GetError());
        SDL_DestroyAudioStream(Stream);
        return nullptr;
    }

    return Stream;
}

void AudioManager::PlaySound(const std::string& Name, float Volume)
{
    StreamConfig Config;
    Config.Loop = false;
    Config.AutoCleanup = true;
    Config.Callback = nullptr;

    CreateAndBindAudioStream(Name, Volume, Config);
}

void AudioManager::PlayMusic(const std::string& Name, float Volume)
{
    if (IsMusicPlaying() && CurrentMusicName == Name)
        return;

    StopMusic();

    StreamConfig Config;
    Config.Loop = true;
    Config.AutoCleanup = false;
    Config.Callback = MusicCallback;

    MusicStream = CreateAndBindAudioStream(Name, Volume, Config);
    
    if (MusicStream != nullptr)
    {
        CurrentMusicName = Name;
        SDL_LogInfo(SDL_LOG_CATEGORY_AUDIO, "AudioManager::PlayMusic -> Started playing music '%s'", Name.c_str());
    }
}

bool AudioManager::IsMusicPlaying() const
{
	return MusicStream != nullptr;
}

void AudioManager::StopMusic()
{
    if (MusicStream == nullptr)
        return;

	SDL_UnbindAudioStream(MusicStream);
	SDL_DestroyAudioStream(MusicStream);
	MusicStream = nullptr;
	CurrentMusicName.clear();
}

void AudioManager::LoadAllSoundsFromAssetsDirectory()
{
    if (!std::filesystem::exists(SoundsDirectory))
    {
        SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "AudioManager::LoadAllSoundsFromAssetsDirectory -> Directory does not exist: %s", SoundsDirectory);
        return;
    }

    if (!std::filesystem::is_directory(SoundsDirectory))
    {
        SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "AudioManager::LoadAllSoundsFromAssetsDirectory -> Path is not a directory: %s", SoundsDirectory);
        return;
    }

    for (const auto& Entry : std::filesystem::directory_iterator(SoundsDirectory))
    {
        if (!Entry.is_regular_file())
            continue;

        const std::string FilePath = Entry.path().string();
        const std::string SoundName = Entry.path().stem().string();
        LoadSound(SoundName, FilePath);
    }
}

void SDLCALL AudioManager::MusicCallback(void* Userdata, SDL_AudioStream* Stream, int AdditionalAmount, int TotalAmount)
{
    AudioManager* Manager = static_cast<AudioManager*>(Userdata);

    const auto It = Manager->SoundCache.find(Manager->CurrentMusicName);
    if (It == Manager->SoundCache.end())
        return;

    const SoundData& Data = It->second;

    if (SDL_GetAudioStreamQueued(Stream) < (Data.Length / 2))
    {
        SDL_PutAudioStreamData(Stream, Data.Buffer, Data.Length);
    }
}
