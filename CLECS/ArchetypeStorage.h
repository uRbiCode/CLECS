#pragma once
#include "Archetype.h"
#include "ComponentTypesCollection.h"
#include "EntitySpawner.h"
#include "AddEntitiesCommand.h"
#include "RemoveEntitiesCommand.h"
#include "ArchetypeHandle.h"
#include <unordered_map>
#include <memory>
#include <functional>

/* Provides communication for World to access and manage Archetypes, and thus components and entities.
 */
class ArchetypeStorage
{
public:
	static ArchetypeStorage Create(ComponentTypesCollection&& Data);

	template<typename... Components>
	void EmplaceEntities(AddEntitiesCommand<Components...>&& Command)
	{
		const ArchetypeKey Key = CreateArchetypeKeyFromComponents<Components...>();

		GetArchetype<Components...>()->Reserve(Command.AccessEntries().size());
		const ArchetypeDescription& ArchetypeDesc = ArchetypeDescriptions.find(Key)->second;
		const RowInserter& Inserter = ArchetypeDesc.Inserter;

		for (const std::tuple<Components...>& Row : Command.AccessEntries())
		{
			const Entity NewEntity = Spawner.CreateEntity();
			Inserter(NewEntity, Row);
			EntitiesToArchetypes[NewEntity.GetId()] = ArchetypeDesc.Id;
		}
	}

	void RemoveEntities(RemoveEntitiesCommand&& Command)
	{
		for (const Entity& EntityToRemove : Command.GetEntities())
		{
			const auto It = EntitiesToArchetypes.find(EntityToRemove.GetId());
			Archetypes[It->second]->SwapRemoveRow(EntityToRemove);
			EntitiesToArchetypes.erase(It);
			Spawner.DestroyEntity(EntityToRemove);
		}
	}

	template<typename... Components>
	ArchetypeBase* GetArchetype()
	{
		const ArchetypeKey Key = CreateArchetypeKeyFromComponents<Components...>();
		if (const auto It = ArchetypeDescriptions.find(Key); It != ArchetypeDescriptions.end())
			return Archetypes[It->second.Id].get();

		const size_t NewId = Archetypes.size();
		std::unique_ptr<ArchetypeBase> NewArchetype = std::make_unique<Archetype<Components...>>();
		Archetypes.emplace_back(std::move(NewArchetype));
		for (const ComponentTypeId ComponentType : Key)
		{
			ComponentsToArchetypes[ComponentType].push_back(NewId);
		}

		Archetype<Components...>* ConcreteArchetype = static_cast<Archetype<Components...>*>(Archetypes.back().get());
		ArchetypeDescriptions.insert({ Key, { NewId, [ConcreteArchetype](Entity NewEntity, const std::tuple<Components...>& Row)
		{
			std::apply([ConcreteArchetype, NewEntity](const Components&... Values)
			{
				ConcreteArchetype->EmplaceBack(NewEntity, Values...);
			}, Row);
		}}});

		return Archetypes.back().get();
	}

	template<typename... Components>
	std::vector<ArchetypeHandle<Components...>> GetArchetypesWithComponents()
	{
		constexpr size_t ComponentCount = sizeof...(Components);
		if (ComponentCount == 0)
			return {};

		const std::vector<ArchetypeId>* Lists[ComponentCount];
		size_t ListIndex = 0;
		bool AnyMissing = false;

		([&]()
		{
			const ComponentTypeId TypeId = ComponentTypes.GetComponentTypeId<Components>();
			const auto It = ComponentsToArchetypes.find(TypeId);
			if (It == ComponentsToArchetypes.end())
			{
				AnyMissing = true;
				return;
			}
			Lists[ListIndex++] = &It->second;
		}(), ...);

		if (AnyMissing)
			return {};

		std::sort(Lists, Lists + ComponentCount, [](const std::vector<ArchetypeId>* A, const std::vector<ArchetypeId>* B)
		{
			return A->size() < B->size();
		});

		std::vector<ArchetypeHandle<Components...>> Result;
		for (const ArchetypeId Candidate : *Lists[0])
		{
			bool FoundInAll = true;
			for (size_t i = 1; i < ComponentCount; ++i)
			{
				if (!std::binary_search(Lists[i]->begin(), Lists[i]->end(), Candidate))
				{
					FoundInAll = false;
					break;
				}
			}

			if (FoundInAll && Archetypes[Candidate]->Size() > 0)
			{
				Result.emplace_back(*Archetypes[Candidate]);
			}
		}

		return Result;
	}

private:
	using ArchetypeId = size_t;
	using ArchetypeKey = std::vector<ComponentTypeId>;
	using RowInserter = std::function<void(Entity, const std::tuple<void*>&)>;
	struct ArchetypeDescription
	{
		ArchetypeId Id;
		RowInserter Inserter;
	};

	template<typename... Components>
	ArchetypeKey CreateArchetypeKeyFromComponents()
	{
		ArchetypeKey Key;
		Key.reserve(sizeof...(Components));
		((Key.push_back(ComponentTypes.GetComponentTypeId<Components>())), ...);
		std::sort(Key.begin(), Key.end());
		return Key;
	}

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

	ComponentTypesCollection ComponentTypes;
	EntitySpawner Spawner;

	std::vector<std::unique_ptr<ArchetypeBase>> Archetypes;

	// Find Archetype of exact Components set
	std::unordered_map<ArchetypeKey, ArchetypeDescription, ArchetypeKeyHash> ArchetypeDescriptions;

	// Filter Archetypes by Component type held
	std::unordered_map<ComponentTypeId, std::vector<ArchetypeId>> ComponentsToArchetypes;

	// Find Archetype for Entity
	std::unordered_map<EntityId, ArchetypeId> EntitiesToArchetypes;

	// Capture archetype functions at creation time
	std::unordered_map<ArchetypeKey, RowInserter, ArchetypeKeyHash> ArchetypeKeyToInserter;
};