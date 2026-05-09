#include "EntityAdmin.h"

Entity EntityAdmin::CreateEntity()
{
	uint32_t Id = 0;
	uint32_t Version = 0;

	if (!FreeEntityIds.empty())
	{
		Id = FreeEntityIds.front();
		FreeEntityIds.pop();
		Version = EntityVersions[Id];
	}
	else
	{
		Id = NextEntityId++;
		EntityVersions.push_back(0);
		Version = 0;
	}

	return Entity::Create(Id, Version);
}

void EntityAdmin::DestroyEntity(const Entity& Entity)
{
	if (!IsEntityValid(Entity))
		return;

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
	const auto EntityId = Entity.GetId();
	if (EntityId >= EntityVersions.size())
		return false;

	return EntityVersions[EntityId] == Entity.GetVersion();
}

void EntityAdmin::Clear()
{
	ComponentPools.clear();
	EntityVersions.clear();

	while (!FreeEntityIds.empty())
		FreeEntityIds.pop();

	NextEntityId = 0;
}