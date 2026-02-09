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
 * Uses sparse sets for O(1) component operations.
 */
class EntityAdmin
{
public:
	EntityAdmin() = default;
	EntityAdmin(const EntityAdmin&) = delete;
	EntityAdmin& operator=(const EntityAdmin&) = delete;

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
		T& Access(const Entity& TargetEntity)
		{
			return Admin->AccessComponent<T>(TargetEntity);
		}

		template<typename T>
		const T& Get(const Entity& TargetEntity) const
		{
			return Admin->GetComponent<T>(TargetEntity);
		}

		template<typename Func>
		void ForEach(Func&& Function)
		{
			for (const auto& TargetEntity : CachedEntities)
			{
				Function(TargetEntity, Admin->AccessComponent<Components>(TargetEntity)...);
			}
		}

	private:
		void CacheMatchingEntities()
		{
			const auto* SmallestPool = FindSmallestPool();
			if (SmallestPool == nullptr)
				return;

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
			const auto* Pool = Admin->GetPool<T>();
			if (Pool != nullptr && Pool->Size() < SmallestSize)
			{
				SmallestSize = Pool->Size();
				SmallestPool = &Pool->GetEntities();
			}
		}

		bool HasAllComponents(const Entity& TargetEntity) const
		{
			return (Admin->HasComponent<Components>(TargetEntity) && ...);
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