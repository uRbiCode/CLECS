#pragma once
#include <cstdint>

enum class CollisionChannel : uint8_t
{
	Default = 0,
	Static,
	Player,
	Ball,
	Brick,
	COUNT
};

constexpr size_t MAX_COLLISION_CHANNELS = static_cast<size_t>(CollisionChannel::COUNT);

enum class CollisionResponse : uint8_t
{
	Ignore,
	Block
};

constexpr size_t ChannelToIndex(CollisionChannel Channel)
{
	return static_cast<size_t>(Channel);
}

struct CollisionComponent
{
	CollisionChannel Channel = CollisionChannel::Default;
	
	// Response to each channel type (default: ignore all)
	CollisionResponse ResponseTable[MAX_COLLISION_CHANNELS] = {};
};