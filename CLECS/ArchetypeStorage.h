#pragma once
#include "Archetype.h"
#include "ComponentTypesCollection.h"
#include "EntityStorage.h"
#include "AddEntitiesCommand.h"
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
		const auto InserterIt = ArchetypeKeyToInserter.find(Key);
		const RowInserter& Inserter = InserterIt->second;

		for (const std::tuple<Components...>& Row : Command.AccessEntries())
		{
			const Entity NewEntity = Entities.CreateEntity();
			Inserter(NewEntity, Row);
		}
	}

	template<typename... Components>
	ArchetypeBase* GetArchetype()
	{
		const ArchetypeKey Key = CreateArchetypeKeyFromComponents<Components...>();
		if (const auto It = ArchetypeKeyToId.find(Key); It != ArchetypeKeyToId.end())
			return Archetypes[It->second].get();

		const size_t NewId = Archetypes.size();
		std::unique_ptr<ArchetypeBase> NewArchetype = std::make_unique<Archetype<Components...>>();
		Archetypes.emplace_back(std::move(NewArchetype));
		ArchetypeKeyToId.insert({ Key, NewId });
		for (const ComponentTypeId ComponentType : Key)
		{
			ComponentsToArchetypes[ComponentType].push_back(NewId);
		}

		Archetype<Components...>* ConcreteArchetype = static_cast<Archetype<Components...>*>(Archetypes.back().get());
		ArchetypeKeyToInserter.emplace(Key, [ConcreteArchetype](Entity NewEntity, const std::tuple<Components...>& Row)
		{
			std::apply([ConcreteArchetype, NewEntity](const Components&... Values)
			{
				ConcreteArchetype->EmplaceBack(NewEntity, Values...);
			}, Row);
		});

		return Archetypes.back().get();
	}

	// TODO: OPTIMIZE LOOK FROM SMALLEST PERSPECTIVE
	template<typename... Components>
	std::vector<ArchetypeBase*> GetArchetypesWithComponents()
	{
		const size_t ComponentCount = sizeof...(Components);
		if (ComponentCount == 0)
			return {};

		std::unordered_map<ArchetypeId, size_t> HitCount;

		([&]()
			{
				const ComponentTypeId ComponentType = ComponentTypes.GetComponentTypeId<Components>();
				const auto It = ComponentsToArchetypes.find(ComponentType);
				if (It == ComponentsToArchetypes.end())
					return;

				for (const ArchetypeId Id : It->second)
					++HitCount[Id];
			}(), ...);

		std::vector<ArchetypeBase*> Result;
		for (const auto& [Id, Count] : HitCount)
		{
			if (Count == ComponentCount)
				Result.push_back(Archetypes[Id].get());
		}
		return Result;
	}

private:
	using ArchetypeId = size_t;
	using ArchetypeKey = std::vector<ComponentTypeId>;
	using RowInserter = std::function<void(Entity, const void*)>;

	template<typename ... Components>
	ArchetypeKey CreateArchetypeKeyFromComponents()
	{
		ArchetypeKey Key;
		Key.reserve(sizeof...(Components));
		((Key.push_back(ComponentTypes.GetComponentTypeId<Components>())), ...);
		std::sort(Key.begin(), Key.end());
		return Key;
	}

	struct ArchetypeKeyHash {
		std::size_t operator()(const ArchetypeKey& Key) const noexcept
		{
			std::size_t Seed = 0xcbf29ce484222325ULL;
			constexpr std::size_t GoldenRatio = sizeof(std::size_t) == 8 ? 0x9e3779b97f4a7c15ULL : 0x9e3779b9UL;
			for (uint32_t ComponentId : Key) {
				Seed ^= static_cast<std::size_t>(ComponentId) + GoldenRatio	+ (Seed << 6) + (Seed >> 2);
			}

			return Seed;
		}
	};

	ComponentTypesCollection ComponentTypes;
	EntityStorage Entities;

	std::vector<std::unique_ptr<ArchetypeBase>> Archetypes;

	// Find Archetype of exact Components set
	std::unordered_map<ArchetypeKey, ArchetypeId, ArchetypeKeyHash> ArchetypeKeyToId;

	// Filter Archetypes by Component type held
	std::unordered_map<ComponentTypeId, std::vector<ArchetypeId>> ComponentsToArchetypes;

	// Capture archetype functions at creation time
	std::unordered_map<ArchetypeKey, RowInserter, ArchetypeKeyHash> ArchetypeKeyToInserter;
};