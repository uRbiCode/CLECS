#pragma once
#include "Entity.h"
#include <vector>

/* Command batches Entities to be removed along with all their components.
 */
class RemoveEntitiesCommand
{
public:
	RemoveEntitiesCommand() = default;
	explicit RemoveEntitiesCommand(size_t EntityCount) { Entities.reserve(EntityCount); }

	RemoveEntitiesCommand& WithEntity(Entity EntityToRemove)
	{
		Entities.push_back(EntityToRemove);
		return *this;
	}

	const std::vector<Entity>& GetEntities() const { return Entities; }

private:
	std::vector<Entity> Entities;
};