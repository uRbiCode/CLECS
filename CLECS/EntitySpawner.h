#pragma once
#include "Entity.h"
#include <queue>

#pragma warning(push)
#pragma warning(disable : 4820)
/* Stores information about entities.
 */
class EntitySpawner
{
public:
	Entity CreateEntity();

	void DestroyEntity(Entity E);

private:
	std::queue<uint32_t> FreeEntityIds;
	uint32_t NextEntityId = 0;
};
#pragma warning(pop)