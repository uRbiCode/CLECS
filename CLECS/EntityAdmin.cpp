#include "EntityAdmin.h"

Entity EntityAdmin::CreateEntity()
{
	uint32_t Id = 0;

	if (!FreeEntityIds.empty())
	{
		Id = FreeEntityIds.front();
		FreeEntityIds.pop();
	}
	else
	{
		Id = NextEntityId++;
		EntityVersions.push_back(0);
	}

	return Entity(Id);
}

void EntityAdmin::DestroyEntity(const Entity& Entity)
{
	const auto EntityId = Entity.GetId();

	for (auto& [TypeId, Pool] : ComponentPools)
	{
		Pool->Remove(Entity);
	}

	// Increment version to invalidate old references
	EntityVersions[EntityId]++;
	FreeEntityIds.push(EntityId);
}

bool EntityAdmin::IsEntityValid(const Entity& Entity) const
{
	return true;
}

void EntityAdmin::Clear()
{
	ComponentPools.clear();
	EntityVersions.clear();

	while (!FreeEntityIds.empty())
		FreeEntityIds.pop();

	NextEntityId = 0;
}