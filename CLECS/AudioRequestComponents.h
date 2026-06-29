#pragma once
#include <string>

// Used by AudioRequestConsumerSystem to request a sound effect to be played
struct SfxRequestComponent
{
    std::string Name;
    float Volume = 1.f;
};

// Used by AudioRequestConsumerSystem to request music to be played
struct MusicRequestComponent
{
    std::string Name;
    float Volume = 1.f;
};