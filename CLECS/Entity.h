#pragma once
#include "CoreTypes.h"
#include <cstdint>

constexpr uint32_t EntityMask = 0xFFFFF;   // 20 bits for entity ID (1M entities)
constexpr uint32_t VersionMask = 0xFFF;    // 12 bits for version (4096 versions)
constexpr uint32_t EntityShift = 0;
constexpr uint32_t VersionShift = 20;

// Use a sentinel bit pattern that's clearly invalid
constexpr uint32_t INVALID_ENTITY_IDENTIFIER = 0;  // 0 = invalid (ID=0, Version=0)

/* Entity is a lightweight identifier with version control for safe reuse.
 * Following EnTT's approach: combining entity Id and version in a single value.
 *
 * NOTE: Entity IDs start at 1 (0 is reserved as invalid)
 */
struct Entity
{
	uint32_t Identifier = INVALID_ENTITY_IDENTIFIER;

	// Extract entity ID from the identifier
	[[nodiscard]] uint32_t GetId() const
	{
		return (Identifier >> EntityShift) & EntityMask;
	}

	// Extract version from the identifier
	[[nodiscard]] uint32_t GetVersion() const
	{
		return (Identifier >> VersionShift) & VersionMask;
	}

	// Create entity from Id and version
	static Entity Create(uint32_t Id, uint32_t Version)
	{
		Entity NewEntity;
		NewEntity.Identifier = ((Id & EntityMask) << EntityShift) | ((Version & VersionMask) << VersionShift);
		return NewEntity;
	}

	[[nodiscard]] bool IsValid() const
	{
		return Identifier != INVALID_ENTITY_IDENTIFIER;
	}

	bool operator==(const Entity& Other) const
	{
		return Identifier == Other.Identifier;
	}

	bool operator!=(const Entity& Other) const
	{
		return Identifier != Other.Identifier;
	}
};

// Hash function for use in std::unordered_map
namespace std
{
	template <>
	struct hash<Entity>
	{
		size_t operator()(const Entity& TargetEntity) const noexcept
		{
			return hash<uint32_t>()(TargetEntity.Identifier);
		}
	};
}
