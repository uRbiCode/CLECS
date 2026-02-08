#pragma once
#include "Entity.h"
#include "ComponentPool.h"

#include <vector>
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <queue>
#include <algorithm>

/* EntityManager handles entity lifecycle and component storage.
 * Uses sparse sets for O(1) component operations.
 */
class EntityManager
{
public:
	EntityManager() = default;
	EntityManager(const EntityManager&) = delete;
	EntityManager& operator=(const EntityManager&) = delete;

	Entity CreateEntity();
	void DestroyEntity(const Entity& TargetEntity);
	bool IsEntityValid(const Entity& TargetEntity) const;

	template<typename T, typename... Args>
	T& AddComponent(const Entity& TargetEntity, Args&&... Arguments)
	{
		return GetOrCreatePool<T>()->Emplace(TargetEntity, std::forward<Args>(Arguments)...);
	}

	template<typename T>
	T& AccessComponent(const Entity& TargetEntity)
	{
		return GetPool<T>()->Access(TargetEntity);
	}

	template<typename T>
	const T& GetComponent(const Entity& TargetEntity) const
	{
		return GetPool<T>()->Get(TargetEntity);
	}

	template<typename T>
	bool HasComponent(const Entity& TargetEntity) const
	{
		const auto* Pool = GetPool<T>();
		return Pool != nullptr && Pool->Has(TargetEntity);
	}

	template<typename T>
	void RemoveComponent(const Entity& TargetEntity)
	{
		auto* Pool = GetPool<T>();
		if (Pool == nullptr)
			return;

		Pool->Remove(TargetEntity);
	}

	// View pattern for efficient iteration
	template<typename T>
	std::vector<Entity> View()
	{
		const auto* Pool = GetPool<T>();
		return Pool != nullptr ? Pool->GetEntities() : std::vector<Entity>{};
	}

	// Group pattern for efficient multi-component iteration
	template<typename... Components>
	class Group
	{
	public:
		Group(EntityManager* Manager) : Manager(Manager)
		{
			CacheMatchingEntities();
		}

		// Iterator support for range-based for loops
		class Iterator
		{
		public:
			Iterator(const std::vector<Entity>* Entities, size_t Index)
				: Entities(Entities), Index(Index) {}

			Entity operator*() const { return (*Entities)[Index]; }
			
			Iterator& operator++()
			{
				++Index;
				return *this;
			}

			bool operator!=(const Iterator& Other) const
			{
				return Index != Other.Index;
			}

		private:
			const std::vector<Entity>* Entities;
			size_t Index;
		};

		Iterator begin() const { return Iterator(&CachedEntities, 0); }
		Iterator end() const { return Iterator(&CachedEntities, CachedEntities.size()); }

		size_t Size() const { return CachedEntities.size(); }
		bool Empty() const { return CachedEntities.empty(); }

		// Get all components for an entity
		template<typename T>
		T& Access(const Entity& TargetEntity)
		{
			return Manager->GetComponent<T>(TargetEntity);
		}

		template<typename T>
		const T& Get(const Entity& TargetEntity) const
		{
			return Manager->GetComponent<T>(TargetEntity);
		}

		// Utility to iterate with components directly
		template<typename Func>
		void ForEach(Func&& Function)
		{
			for (const auto& TargetEntity : CachedEntities)
			{
				Function(TargetEntity, Manager->GetComponent<Components>(TargetEntity)...);
			}
		}

	private:
		void CacheMatchingEntities()
		{
			// Find the smallest pool to minimize intersection checks
			const auto* SmallestPool = FindSmallestPool();
			if (SmallestPool == nullptr)
				return;

			// Check which entities have all required components
			for (const auto& TargetEntity : *SmallestPool)
			{
				if (HasAllComponents(TargetEntity))
				{
					CachedEntities.push_back(TargetEntity);
				}
			}
		}

		const std::vector<Entity>* FindSmallestPool() const
		{
			const std::vector<Entity>* SmallestPool = nullptr;
			size_t SmallestSize = SIZE_MAX;

			((FindSmaller<Components>(SmallestPool, SmallestSize)), ...);

			return SmallestPool;
		}

		template<typename T>
		void FindSmaller(const std::vector<Entity>*& SmallestPool, size_t& SmallestSize) const
		{
			const auto* Pool = Manager->GetPool<T>();
			if (Pool != nullptr && Pool->Size() < SmallestSize)
			{
				SmallestSize = Pool->Size();
				SmallestPool = &Pool->GetEntities();
			}
		}

		bool HasAllComponents(const Entity& TargetEntity) const
		{
			return (Manager->HasComponent<Components>(TargetEntity) && ...);
		}

		EntityManager* Manager;
		std::vector<Entity> CachedEntities;
	};

	// Create a group for entities with all specified components
	template<typename... Components>
	Group<Components...> GetGroup()
	{
		return Group<Components...>(this);
	}

	void Clear();

private:
	template<typename T>
	ComponentPool<T>* GetOrCreatePool()
	{
		const auto TypeId = std::type_index(typeid(T));
		
		const auto It = ComponentPools.find(TypeId);
		if (It == ComponentPools.end())
		{
			const auto Pool = std::make_unique<ComponentPool<T>>();
			const auto* PoolPtr = Pool.get();
			ComponentPools[TypeId] = std::move(Pool);
			return PoolPtr;
		}
		
		return static_cast<ComponentPool<T>*>(It->second.get());
	}

	template<typename T>
	ComponentPool<T>* GetPool()
	{
		const auto TypeId = std::type_index(typeid(T));
		
		const auto It = ComponentPools.find(TypeId);
		if (It == ComponentPools.end())
			return nullptr;
		
		return static_cast<ComponentPool<T>*>(It->second.get());
	}

	template<typename T>
	const ComponentPool<T>* GetPool() const
	{
		const auto TypeId = std::type_index(typeid(T));
		
		const auto It = ComponentPools.find(TypeId);
		if (It == ComponentPools.end())
			return nullptr;
		
		return static_cast<const ComponentPool<T>*>(It->second.get());
	}

	// Entity management with versioning
	std::vector<uint32_t> EntityVersions;
	std::queue<uint32_t> FreeEntityIds;
	uint32_t NextEntityId = 0;

	// Type-erased component pools
	std::unordered_map<std::type_index, std::unique_ptr<ComponentPoolBase>> ComponentPools;
};