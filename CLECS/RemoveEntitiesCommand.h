#pragma once
#include "Entity.h"
#include <vector>

/* Command batches Entities to be removed along with all their components.
 */
class RemoveEntitiesCommand
{
public:
	RemoveEntitiesCommand(size_t EntityCount);

	RemoveEntitiesCommand& WithEntry(Entity EntityToRemove);

	const std::vector<Entity>& GetEntries() const;

private:
	std::vector<Entity> Entries;
};