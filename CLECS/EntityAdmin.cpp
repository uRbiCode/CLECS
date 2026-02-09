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

void EntityAdmin::DestroyEntity(const Entity& TargetEntity)
{
	if (!IsEntityValid(TargetEntity))
		return;

	const auto EntityId = TargetEntity.GetId();

	for (auto& [TypeId, Pool] : ComponentPools)
	{
		Pool->Remove(TargetEntity);
	}

	// Increment version to invalidate old references
	EntityVersions[EntityId]++;
	FreeEntityIds.push(EntityId);
}

bool EntityAdmin::IsEntityValid(const Entity& TargetEntity) const
{
	if (!TargetEntity.IsValid())
		return false;

	const auto EntityId = TargetEntity.GetId();
	if (EntityId >= EntityVersions.size())
		return false;

	return EntityVersions[EntityId] == TargetEntity.GetVersion();
}

void EntityAdmin::Clear()
{
	ComponentPools.clear();
	EntityVersions.clear();
	while (!FreeEntityIds.empty())
		FreeEntityIds.pop();
	NextEntityId = 0;
}