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

void AudioManager::PlaySound(const std::string& Name, float Volume) const
{
    if (AudioDeviceId == 0)
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_AUDIO, "AudioManager::PlaySound -> Audio device not initialized");
        return;
    }

    const auto It = SoundCache.find(Name);
    if (It == SoundCache.end())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_AUDIO, "AudioManager::PlaySound -> Sound '%s' not found", Name.c_str());
        return;
    }

    const SoundData& SoundData = It->second;
    
    SDL_AudioSpec DeviceSpec;
    if (!SDL_GetAudioDeviceFormat(AudioDeviceId, &DeviceSpec, nullptr))
    {
        SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "AudioManager::PlaySound -> Failed to get audio device format: %s", SDL_GetError());
        return;
    }

    auto SoundStream = SDL_CreateAudioStream(&SoundData.Spec, &DeviceSpec);
    if (SoundStream == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "AudioManager::PlaySound -> Failed to create sound stream: %s", SDL_GetError());
        return;
    }

    auto StreamPropertiesId = SDL_GetAudioStreamProperties(SoundStream);
    if (StreamPropertiesId != 0)
    {
        SDL_SetBooleanProperty(StreamPropertiesId, SDL_PROP_AUDIOSTREAM_AUTO_CLEANUP_BOOLEAN, true);
    }

    SDL_SetAudioStreamGain(SoundStream, Volume * MasterVolume);
    
    if (!SDL_PutAudioStreamData(SoundStream, SoundData.Buffer, SoundData.Length))
    {
        SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "AudioManager::PlaySound -> Failed to put audio data: %s", SDL_GetError());
        SDL_DestroyAudioStream(SoundStream);
        return;
    }
    
    if (!SDL_FlushAudioStream(SoundStream))
    {
        SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "AudioManager::PlaySound -> Failed to flush audio stream: %s", SDL_GetError());
        SDL_DestroyAudioStream(SoundStream);
        return;
    }

    if (!SDL_BindAudioStream(AudioDeviceId, SoundStream))
    {
        SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "AudioManager::PlaySound -> Failed to bind audio stream: %s", SDL_GetError());
        SDL_DestroyAudioStream(SoundStream);
        return;
    }
}

void AudioManager::SetMasterVolume(float Volume)
{
    MasterVolume = std::clamp(Volume, 0.0f, 1.0f);
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

        const auto FilePath = Entry.path().string();
        const auto SoundName = Entry.path().stem().string();
        LoadSound(SoundName, FilePath);
    }
}