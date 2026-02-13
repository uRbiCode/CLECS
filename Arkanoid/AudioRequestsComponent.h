#pragma once
#include <cstdint>
#include <string>
#include <vector>

enum class AudioType : uint8_t
{
	Invalid,
	Sfx,
	Music
};

struct AudioRequest
{
	AudioType Type = AudioType::Invalid;
	std::string Name;
	float Volume = 0.f;
};

// Tracks requested sounds each frame to prevent sounds overlaping
struct AudioRequestsComponent
{
	std::vector<AudioRequest> Requests;
	float RequestTimer = 0.f;
};