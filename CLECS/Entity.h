#pragma once
#include "CoreTypes.h"

constexpr uint32_t EntityMask = 0xFFFFF;   
constexpr uint32_t VersionMask = 0xFFF;
constexpr uint32_t EntityShift = 0;
constexpr uint32_t VersionShift = 20;

// 0 == invalid (Id == 0, Version == 0)
constexpr uint32_t INVALID_ENTITY_IDENTIFIER = 0;  

/* Entity is a lightweight identifier with version control for safe reuse.
 * Combines entity Id and version in a single value.
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
		const auto Id64 = static_cast<uint64_t>(Id & EntityMask);
		const auto Version64 = static_cast<uint64_t>(Version & VersionMask);
		NewEntity.Identifier = static_cast<uint32_t>((Id64 << EntityShift) | (Version64 << VersionShift));
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
