#pragma once
#include <cstdint>

enum class CollisionChannel : uint8_t
{
	Static = 0,
	Player,
	Ball,
	Brick,
	Trigger,
	COUNT
};

constexpr size_t MAX_COLLISION_CHANNELS = static_cast<size_t>(CollisionChannel::COUNT);

enum class CollisionResponse : uint8_t
{
	Block,
	Ignore
};

constexpr size_t ChannelToIndex(CollisionChannel Channel)
{
	return static_cast<size_t>(Channel);
}

struct CollisionComponent
{
	CollisionChannel Channel = CollisionChannel::Static;
	
	// Block all by default
	CollisionResponse ResponseTable[MAX_COLLISION_CHANNELS] = {};
};