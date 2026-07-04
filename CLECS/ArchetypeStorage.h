#pragma once
#include "Archetype.h"
#include "ComponentTypesCollection.h"
#include "EntitySpawner.h"
#include "AddEntitiesCommand.h"
#include "AddComponentsCommand.h"
#include "RemoveComponentsCommand.h"
#include "RemoveEntitiesCommand.h"
#include "ArchetypeHandle.h"
#include <unordered_map>
#include <algorithm>
#include <ranges>

/* Provides communication for World to access and manage Archetypes, and thus components and entities.
 */
class ArchetypeStorage
{
public:
	static ArchetypeStorage Create(ComponentTypesCollection&& Data);

	template<ComponentType... Components>
	void EmplaceEntities(AddEntitiesCommand<Components...>&& Command)
	{
		Archetype& Arch = AccessArchetype<Components...>();
		Arch.Reserve(Command.AccessEntries().size());

		const ArchetypeKey Key = CreateArchetypeKeyFromComponents<Components...>();
		const ArchetypeId ArchId = KeysToArchetypes.find(Key)->second;

		for (std::tuple<Components...>& Row : Command.AccessEntries())
		{
			const Entity NewEntity = Spawner.CreateEntity();
			std::apply([&](Components&&... Values)
			{
				Arch.EmplaceTypedRow(ComponentTypes, NewEntity, std::forward<Components>(Values)...);
			}, std::move(Row));
			EntitiesToArchetypes[NewEntity.GetId()] = ArchId;
		}
	}

	void RemoveEntities(RemoveEntitiesCommand&& Command);

	template<ComponentType... NewComponents>
	void AddComponents(AddComponentsCommand<NewComponents...>&& Command)
	{
		for (auto& [E, NewData] : Command.AccessEntries())
		{
			const auto EntityIt = EntitiesToArchetypes.find(E.GetId());
			if (EntityIt == EntitiesToArchetypes.end())
				continue;

			const ArchetypeId SrcId = EntityIt->second;

			ArchetypeKey TargetKey = Archetypes[SrcId].Key;
			([&]()
			{
				const ComponentTypeId Id = ComponentTypes.GetComponentTypeId<NewComponents>();
				const auto Pos = std::lower_bound(TargetKey.begin(), TargetKey.end(), Id);
				if (Pos == TargetKey.end() || *Pos != Id)
				{
					TargetKey.insert(Pos, Id);
				}
			}(), ...);

			if (TargetKey == Archetypes[SrcId].Key)
				continue;

			const ArchetypeId DstId = AccessOrCreateExtendedArchetype<NewComponents...>(TargetKey, SrcId);

			Archetype& SrcArch = Archetypes[SrcId].Archetype;
			Archetype& DstArch = Archetypes[DstId].Archetype;

			SrcArch.MigrateRowTo(E, DstArch);

			std::apply([&](NewComponents&&... Values)
			{
				([&]()
				{
					const ComponentTypeId Id = ComponentTypes.GetComponentTypeId<NewComponents>();
					if (!SrcArch.HasComponentType(Id))
					{
						DstArch.EmplaceInColumn<NewComponents>(Id, std::forward<NewComponents>(Values));
					}
				}(), ...);
			}, std::move(NewData));

			EntityIt->second = DstId;
		}
	}

	template<ComponentType... RemovedComponents>
	void RemoveComponents(RemoveComponentsCommand<RemovedComponents...>&& Command)
	{
		for (const Entity E : Command.GetEntries())
		{
			const auto EntityIt = EntitiesToArchetypes.find(E.GetId());
			if (EntityIt == EntitiesToArchetypes.end())
				continue;

			const ArchetypeId SrcId = EntityIt->second;

			ArchetypeKey TargetKey = Archetypes[SrcId].Key;
			([&]()
			{
				const ComponentTypeId Id = ComponentTypes.GetComponentTypeId<RemovedComponents>();
				const auto Pos = std::lower_bound(TargetKey.begin(), TargetKey.end(), Id);
				if (Pos != TargetKey.end() && *Pos == Id)
				{
					TargetKey.erase(Pos);
				}
			}(), ...);

			if (TargetKey == Archetypes[SrcId].Key)
				continue;

			const ArchetypeId DstId = AccessOrCreateReducedArchetype(TargetKey, SrcId);

			Archetype& SrcArch = Archetypes[SrcId].Archetype;
			Archetype& DstArch = Archetypes[DstId].Archetype;

			SrcArch.MigrateRowTo(E, DstArch);

			EntityIt->second = DstId;
		}
	}

	template<ComponentType... Components>
	std::vector<ArchetypeHandle<Components...>> AccessArchetypesWithComponents()
	{
		constexpr size_t ComponentCount = sizeof...(Components);
		std::array<const std::vector<ArchetypeId>*, ComponentCount> Lists{};
		size_t ListIndex = 0;
		bool AnyMissing = false;

		([&]()
		{
			const auto It = ComponentsToArchetypes.find(ComponentTypes.GetComponentTypeId<Components>());
			if (It == ComponentsToArchetypes.end())
			{
				AnyMissing = true;
				return;
			}
			Lists[ListIndex++] = &It->second;
		}(), ...);

		if (AnyMissing)
			return {};

		std::ranges::sort(Lists, [](const std::vector<ArchetypeId>* A, const std::vector<ArchetypeId>* B)
		{
			return A->size() < B->size();
		});

		std::vector<ArchetypeHandle<Components...>> Result;
		for (const ArchetypeId Candidate : *Lists[0])
		{
			const bool FoundInAll = std::ranges::all_of(Lists.begin() + 1, Lists.end(),
			[Candidate](const std::vector<ArchetypeId>* List)
			{
				return std::ranges::binary_search(*List, Candidate);
			});

			if (FoundInAll && Archetypes[Candidate].Archetype.Size() > 0)
			{
				Result.emplace_back(Archetypes[Candidate].Archetype, ComponentTypes);
			}
		}

		return Result;
	}

private:
	using ArchetypeId = size_t;
	using ArchetypeKey = std::vector<ComponentTypeId>;

	struct ArchetypeEntry
	{
		Archetype Archetype;
		ArchetypeKey Key;
	};

	struct ArchetypeKeyHash
	{
		std::size_t operator()(const ArchetypeKey& Key) const noexcept
		{
			std::size_t Seed = 0xcbf29ce484222325ULL;
			constexpr std::size_t GoldenRatio = sizeof(std::size_t) == 8 ? 0x9e3779b97f4a7c15ULL : 0x9e3779b9UL;
			for (const ComponentTypeId ComponentId : Key)
			{
				Seed ^= static_cast<std::size_t>(ComponentId) + GoldenRatio + (Seed << 6) + (Seed >> 2);
			}

			return Seed;
		}
	};

	template<typename... Components>
	Archetype& AccessArchetype()
	{
		const ArchetypeKey Key = CreateArchetypeKeyFromComponents<Components...>();
		const auto It = KeysToArchetypes.find(Key);
		if (It != KeysToArchetypes.end())
			return Archetypes[It->second].Archetype;

		const ArchetypeId NewId = Archetypes.size();
		Archetypes.push_back({ Archetype::MakeArchetype<Components...>(ComponentTypes), Key });
		KeysToArchetypes[Key] = NewId;

		for (const ComponentTypeId CompId : Key)
		{
			ComponentsToArchetypes[CompId].push_back(NewId);
		}

		return Archetypes.back().Archetype;
	}

	template<ComponentType... NewComponents>
	ArchetypeId AccessOrCreateExtendedArchetype(const ArchetypeKey& TargetKey, ArchetypeId SrcId)
	{
		const auto It = KeysToArchetypes.find(TargetKey);
		if (It != KeysToArchetypes.end())
			return It->second;

		const ArchetypeId NewId = Archetypes.size();
		Archetypes.push_back({ Archetype::MakeExtended<NewComponents...>(Archetypes[SrcId].Archetype, ComponentTypes), TargetKey });
		KeysToArchetypes[TargetKey] = NewId;

		for (const ComponentTypeId CompId : TargetKey)
		{
			ComponentsToArchetypes[CompId].push_back(NewId);
		}

		return NewId;
	}

	ArchetypeId AccessOrCreateReducedArchetype(const ArchetypeKey& TargetKey, ArchetypeId SrcId);

	template<typename... Components>
	ArchetypeKey CreateArchetypeKeyFromComponents()
	{
		ArchetypeKey Key;
		Key.reserve(sizeof...(Components));
		((Key.push_back(ComponentTypes.GetComponentTypeId<Components>())), ...);
		std::ranges::sort(Key);
		return Key;
	}

	ComponentTypesCollection ComponentTypes;
	EntitySpawner Spawner;

	std::vector<ArchetypeEntry> Archetypes;

	// Find Archetype of exact Components set
	std::unordered_map<ArchetypeKey, ArchetypeId, ArchetypeKeyHash> KeysToArchetypes;

	// Filter Archetypes by Component type held
	std::unordered_map<ComponentTypeId, std::vector<ArchetypeId>> ComponentsToArchetypes;

	// Find Archetype for Entity
	std::unordered_map<EntityId, ArchetypeId> EntitiesToArchetypes;
};