#include "EntityStorage.h"

Entity EntityStorage::CreateEntity()
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

void EntityStorage::DestroyEntity(Entity E)
{
	FreeEntityIds.push(E.GetId());
}