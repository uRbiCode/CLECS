#pragma once
#include "Entity.h"
#include <queue>

/* Stores information about entities.
 */
class EntityStorage
{
public:
	Entity CreateEntity();

	void DestroyEntity(Entity E);

private:
	std::queue<uint32_t> FreeEntityIds;
	uint32_t NextEntityId = 0;
};