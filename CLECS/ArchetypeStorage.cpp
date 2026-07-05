#include "ArchetypeStorage.h"

ArchetypeStorage ArchetypeStorage::Create(ComponentTypesCollection&& Data)
{
	ArchetypeStorage Storage = ArchetypeStorage();
	Storage.ComponentTypes = std::move(Data);
	Storage.ComponentsToArchetypes.reserve(Storage.ComponentTypes.GetSize());
	for (const auto& [ComponentType, ComponentTypeId] : Storage.ComponentTypes.GetRegisteredComponents())
	{
		Storage.ComponentsToArchetypes.insert({ ComponentTypeId, {} });
	}
	return Storage;
}

void ArchetypeStorage::RemoveEntities(RemoveEntitiesCommand&& Command)
{
	for (const Entity EntityToRemove : Command.GetEntries())
	{
		const auto It = EntitiesToArchetypes.find(EntityToRemove.GetId());
		if (It == EntitiesToArchetypes.end())
			continue;

		Archetypes[It->second].Archetype.SwapRemoveRow(EntityToRemove);
		EntitiesToArchetypes.erase(It);
		Spawner.DestroyEntity(EntityToRemove);
	}
}

ArchetypeStorage::ArchetypeId ArchetypeStorage::AccessOrCreateReducedArchetype(const ArchetypeKey& TargetKey, ArchetypeId SrcId)
{
	const auto It = KeysToArchetypes.find(TargetKey);
	if (It != KeysToArchetypes.end())
		return It->second;

	const ArchetypeId NewId = Archetypes.size();
	Archetypes.push_back({ Archetype::MakeNarrowed(Archetypes[SrcId].Archetype, TargetKey), TargetKey });
	KeysToArchetypes[TargetKey] = NewId;

	for (const ComponentTypeId CompId : TargetKey)
	{
		ComponentsToArchetypes[CompId].push_back(NewId);
	}

	return NewId;
}

std::size_t ArchetypeStorage::ArchetypeKeyHash::operator()(const ArchetypeKey& Key) const noexcept
{
	std::size_t Seed = 0xcbf29ce484222325ULL;
	constexpr std::size_t GoldenRatio = sizeof(std::size_t) == 8 ? 0x9e3779b97f4a7c15ULL : 0x9e3779b9UL;
	for (const ComponentTypeId ComponentId : Key)
	{
		Seed ^= static_cast<std::size_t>(ComponentId) + GoldenRatio + (Seed << 6) + (Seed >> 2);
	}

	return Seed;
}