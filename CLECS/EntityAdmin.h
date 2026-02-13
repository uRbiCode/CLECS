#pragma once
#include "Entity.h"
#include "ComponentPool.h"

#include <vector>
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <queue>
#include <algorithm>

/* EntityAdmin handles entity lifecycle and component storage.
 * It manages Ids of entities with versioning to prevent stale references.
 * It also stores ComponentPools for efficient access and iteration.
 */
class EntityAdmin
{
public:
	EntityAdmin() = default;
	EntityAdmin(const EntityAdmin&) = delete;
	EntityAdmin& operator=(const EntityAdmin&) = delete;

	Entity CreateEntity();
	void DestroyEntity(const Entity& Entity);
	bool IsEntityValid(const Entity& Entity) const;

	template<typename T, typename... Args>
	T& AddComponent(const Entity& Entity, Args&&... Arguments)
	{
		return GetOrCreatePool<T>()->Emplace(Entity, std::forward<Args>(Arguments)...);
	}

	template<typename T>
	T& AccessComponent(const Entity& Entity)
	{
		return GetPool<T>()->Access(Entity);
	}

	template<typename T>
	const T& GetComponent(const Entity& Entity) const
	{
		return GetPool<T>()->Get(Entity);
	}

	template<typename T>
	bool HasComponent(const Entity& Entity) const
	{
		const auto* Pool = GetPool<T>();
		return Pool != nullptr && Pool->Has(Entity);
	}

	template<typename T>
	void RemoveComponent(const Entity& Entity)
	{
		auto* Pool = GetPool<T>();
		if (Pool == nullptr)
			return;

		Pool->Remove(Entity);
	}

	/* Enables efficient iteration over entities that have a specific combination of components.
	 * Caches matching entitities, so combioned with ComponentPool's sparse set, it allows for very fast iteration and access to components.
	 * Optimized by iterating over the smallest component pool first and checking for the presence of other components, minimizing the number of checks needed.
	 */ 
	template<typename... Components>
	class Group
	{
	public:
		Group(EntityAdmin* Admin) : Admin(Admin)
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

		Entity operator[](size_t Index) const
		{
			return CachedEntities[Index];
		}

		template<typename T>
		T& Access(const Entity& Entity)
		{
			return Admin->AccessComponent<T>(Entity);
		}

		template<typename T>
		const T& Get(const Entity& Entity) const
		{
			return Admin->GetComponent<T>(Entity);
		}

		template<typename Func>
		void ForEach(Func&& Function)
		{
			for (const auto& Entity : CachedEntities)
			{
				Function(Entity, Admin->AccessComponent<Components>(Entity)...);
			}
		}

	private:
		void CacheMatchingEntities()
		{
			const auto* SmallestPool = FindSmallestPool();
			if (SmallestPool == nullptr)
				return;

			for (const auto& Entity : *SmallestPool)
			{
				if (HasAllComponents(Entity))
				{
					CachedEntities.push_back(Entity);
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
			const auto* Pool = Admin->GetPool<T>();
			if (Pool != nullptr && Pool->Size() < SmallestSize)
			{
				SmallestSize = Pool->Size();
				SmallestPool = &Pool->GetEntities();
			}
		}

		bool HasAllComponents(const Entity& Entity) const
		{
			return (Admin->HasComponent<Components>(Entity) && ...);
		}

		EntityAdmin* Admin;
		std::vector<Entity> CachedEntities;
	};

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
			auto Pool = std::make_unique<ComponentPool<T>>();
			auto* PoolPtr = Pool.get();
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

	std::unordered_map<std::type_index, std::unique_ptr<ComponentPoolBase>> ComponentPools;
};