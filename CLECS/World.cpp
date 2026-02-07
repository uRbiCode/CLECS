#include "World.h"
#include "SDL3/SDL.h"

namespace
{
	template<class T>
	concept SystemType = std::derived_from<T, System>;

	template<SystemType T>
	std::unique_ptr<T> CreateSystem()
	{
		return std::make_unique<T>();
	}
}

Entity World::CreateEntity()
{
	uint32_t Id;
	uint32_t Version = 0;

	if (!FreeEntityIds.empty())
	{
		// Reuse freed entity ID
		Id = FreeEntityIds.front();
		FreeEntityIds.pop();
		Version = EntityVersions[Id];
	}
	else
	{
		// Create new entity Id (start at 1, since 0 is invalid)
		Id = ++NextEntityId;
		EntityVersions.push_back(0);
	}

	return Entity::Create(Id, Version);
}

void World::DestroyEntity(const Entity& TargetEntity)
{
	if (!IsEntityValid(TargetEntity))
		return;

	const auto EntityId = TargetEntity.GetId();

	// Remove all components from this entity
	for (auto& [TypeId, Pool] : ComponentPools)
	{
		Pool->Remove(TargetEntity);
	}

	// Increment version to invalidate old references
	EntityVersions[EntityId]++;
	FreeEntityIds.push(EntityId);
}

bool World::IsEntityValid(const Entity& TargetEntity) const
{
	if (!TargetEntity.IsValid())
		return false;

	const auto EntityId = TargetEntity.GetId();
	if (EntityId >= EntityVersions.size())
		return false;

	return EntityVersions[EntityId] == TargetEntity.GetVersion();
}

CLECS::ResultType World::Update(float DeltaTime)
{
	for (const auto& CurrentSystem : Systems)
	{
		CurrentSystem->Update(DeltaTime);
	}

	return CLECS::ResultType::Success;
}

CLECS::ResultType World::InitializeWorld()
{
	if (InitializeSystems() != CLECS::ResultType::Success)
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Failed to initialize systems");
		return CLECS::ResultType::Failure;
	}

	return CLECS::ResultType::Success;
}

CLECS::ResultType World::InitializeSystems()
{
	// Systems.push_back(CreateSystem<System>());
	return CLECS::ResultType::Success;
}