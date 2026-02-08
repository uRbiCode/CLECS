#pragma once
#include <cstdint>
#include <algorithm>

enum class CollisionChannel : uint8_t
{
	Default = 0,
	Static,          // Static world geometry
	Dynamic,         // Moving objects
	Player,          // Player entities
	Enemy,           // Enemy entities
	Projectile,      // Bullets, projectiles
	Trigger,         // Trigger volumes
	Custom1,
	Custom2,
	Custom3,
	COUNT
};

constexpr size_t MAX_COLLISION_CHANNELS = static_cast<size_t>(CollisionChannel::COUNT);

enum class CollisionResponse : uint8_t
{
	Ignore,    
	Overlap,  
	Block    
};

constexpr size_t ChannelToIndex(CollisionChannel Channel)
{
	return static_cast<size_t>(Channel);
}

// Collision component stores channel and interaction rules
struct CollisionComponent
{
	CollisionChannel Channel = CollisionChannel::Default;

	// Ignore everything by default
	CollisionResponse ResponseTable[MAX_COLLISION_CHANNELS] = {};
};