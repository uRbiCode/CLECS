#pragma once
#include <cstdint>

using EntityId = uint32_t;

/* Entity is a fundmanetal concept in CLECS architecture.
 * Represents a unique instance in a world that we can attach components to.
 * Under the hood translated to a simple Id;
 */
struct Entity
{
	Entity(EntityId Id) : Identifier(Id) {}

	[[nodiscard]] EntityId GetId() const { return Identifier; }

	bool operator==(Entity Other) const
	{
		return Identifier == Other.Identifier;
	}

	bool operator!=(Entity Other) const
	{
		return Identifier != Other.Identifier;
	}

private:
	EntityId Identifier = 0;
};