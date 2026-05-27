#include "EntitySpawner.h"

Entity EntitySpawner::CreateEntity()
{
	uint32_t Id;
	if (!FreeEntityIds.empty())
	{
		Id = FreeEntityIds.front();
		FreeEntityIds.pop();
	}
	else
	{
		Id = NextEntityId++;
	}
	return Entity(Id);
}

void EntitySpawner::DestroyEntity(Entity EntityToDestroy)
{
	FreeEntityIds.push(EntityToDestroy.GetId());
}