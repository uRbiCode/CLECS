#pragma once
#include "CoreTypes.h"

constexpr uint32_t EntityMask = 0xFFFFF;   
constexpr uint32_t VersionMask = 0xFFF;
constexpr uint32_t EntityShift = 0;
constexpr uint32_t VersionShift = 20;

/* Entity is a lightweight identifier with version control for safe reuse.
 * Combines entity Id and version in a single value.
 */
struct Entity
{
	Entity() = default;

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

	[[nodiscard]] bool IsValid() const
	{
		return Identifier != 0;
	}

	bool operator==(const Entity& Other) const
	{
		return Identifier == Other.Identifier;
	}

	bool operator!=(const Entity& Other) const
	{
		return Identifier != Other.Identifier;
	}

private:
	uint32_t Identifier = 0;

	// Create entity from Id and version
	static Entity Create(uint32_t Id, uint32_t Version)
	{
		Entity NewEntity;
		const auto MaskedId = Id & EntityMask;
		const auto MaskedVersion = Version & VersionMask;
		NewEntity.Identifier = static_cast<uint32_t>((MaskedId << EntityShift) | (MaskedVersion << VersionShift));
		return NewEntity;
	}

	friend class EntityManager;
	friend struct std::hash<Entity>;
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
