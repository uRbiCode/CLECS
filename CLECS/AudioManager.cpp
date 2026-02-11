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
    constexpr SDL_AudioSpec AudioSpecification = { SDL_AUDIO_F32, 2, 44100 };

    AudioStream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &AudioSpecification, nullptr, nullptr);

    if (AudioStream == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "AudioManager::Initialize -> Failed to create audio stream: %s", SDL_GetError());
        return;
    }

    SDL_SetAudioStreamGain(AudioStream, MasterVolume);
    SDL_ResumeAudioStreamDevice(AudioStream);

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

    if (AudioStream != nullptr)
    {
        SDL_DestroyAudioStream(AudioStream);
        AudioStream = nullptr;
    }
}

bool AudioManager::LoadSound(const std::string& Name, const std::string& FilePath)
{
    if (SoundCache.find(Name) != SoundCache.end())
        return true;

    SoundData SoundData;

    if (!SDL_LoadWAV(FilePath.c_str(), &SoundData.Spec, &SoundData.Buffer, &SoundData.Length))
    {
        SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "AudioManager::LoadSound -> Failed to load sound '%s' from '%s': %s", Name.c_str(), FilePath.c_str(), SDL_GetError());
        return false;
    }

    SoundCache[Name] = SoundData;
    SDL_LogInfo(SDL_LOG_CATEGORY_AUDIO, "AudioManager::LoadSound -> Loaded sound '%s' (%u bytes, %d Hz, %d channels)", Name.c_str(), SoundData.Length, SoundData.Spec.freq, SoundData.Spec.channels);

    return true;
}

void AudioManager::PlaySound(const std::string& Name, float Volume) const
{
    if (AudioStream == nullptr)
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_AUDIO, "AudioManager::PlaySound -> Audio stream not initialized");
        return;
    }

    const auto It = SoundCache.find(Name);
    if (It == SoundCache.end())
    {
        SDL_LogWarn(SDL_LOG_CATEGORY_AUDIO, "AudioManager::PlaySound -> Sound '%s' not found", Name.c_str());
        return;
    }

    const SoundData& SoundData = It->second;
    auto TempStream = SDL_CreateAudioStream(&SoundData.Spec, nullptr);
    if (TempStream == nullptr)
    {
        SDL_LogError(SDL_LOG_CATEGORY_AUDIO, "AudioManager::PlaySound -> Failed to create temp stream: %s", SDL_GetError());
        return;
    }

    SDL_SetAudioStreamGain(TempStream, Volume * MasterVolume);
    SDL_PutAudioStreamData(TempStream, SoundData.Buffer, SoundData.Length);
    SDL_FlushAudioStream(TempStream);

    const int AvailableBytes = SDL_GetAudioStreamAvailable(TempStream);
    if (AvailableBytes > 0)
    {
        void* ConvertedData = SDL_malloc(AvailableBytes);
        if (ConvertedData != nullptr)
        {
            const int gotBytes = SDL_GetAudioStreamData(TempStream, ConvertedData, AvailableBytes);
            if (gotBytes > 0)
            {
                SDL_PutAudioStreamData(AudioStream, ConvertedData, gotBytes);
            }
            SDL_free(ConvertedData);
        }
    }

    SDL_DestroyAudioStream(TempStream);
}

void AudioManager::SetMasterVolume(float Volume)
{
    MasterVolume = std::clamp(Volume, 0.0f, 1.0f);
    if (AudioStream == nullptr)
        return;

    SDL_SetAudioStreamGain(AudioStream, MasterVolume);
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