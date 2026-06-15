#pragma once
#include "Entity.h"
#include <vector>

/* Command batches Entities to be removed along with all their components.
 */
class RemoveEntitiesCommand
{
public:
	RemoveEntitiesCommand(size_t EntityCount) { Entries.reserve(EntityCount); }

	RemoveEntitiesCommand& WithEntry(Entity EntityToRemove)
	{
		Entries.push_back(EntityToRemove);
		return *this;
	}

	const std::vector<Entity>& GetEntries() const { return Entries; }

private:
	std::vector<Entity> Entries;
};