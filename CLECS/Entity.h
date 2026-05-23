#pragma once
#include <cstdint>

/* Entity is a fundmanetal concept in CLECS architecture.
 * Represents a unique instance in a world that we can attach components to.
 * Under the hood translated to a simple Id;
 */
struct Entity
{
	Entity(uint32_t Id) : Identifier(Id) {}

	[[nodiscard]] uint32_t GetId() const { return Identifier; }

	bool operator==(Entity Other) const
	{
		return Identifier == Other.Identifier;
	}

	bool operator!=(Entity Other) const
	{
		return Identifier != Other.Identifier;
	}

private:
	uint32_t Identifier = 0;
};