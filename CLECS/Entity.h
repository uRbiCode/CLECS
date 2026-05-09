#pragma once
#include <cstdint>
#include <functional>

/* Entity is a fundmanetal concept in CLECS architecture.
 * Represents a unique instance in a world that we can attach components to.
 * Consits of and id and version number.
 */
struct Entity
{
	[[nodiscard]] uint32_t GetId() const { return Identifier; }

	[[nodiscard]] uint32_t GetVersion() const { return Version; }

	bool operator==(const Entity& Other) const
	{
		return Identifier == Other.Identifier;
	}

	bool operator!=(const Entity& Other) const
	{
		return Identifier != Other.Identifier;
	}

private:
	Entity() = default;
	static Entity Create(uint32_t Id, uint32_t Version)
	{
		Entity NewEntity;
		NewEntity.Identifier = Id;
		NewEntity.Version = Version;
		return NewEntity;
	}

	uint32_t Identifier = 0;
	uint32_t Version = 0;

	friend class EntityAdmin;
};