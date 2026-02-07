#pragma once
#include "Entity.h"
#include "ComponentPool.h"

#include <vector>
#include <memory>
#include <unordered_map>
#include <typeindex>
#include <queue>

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
	T& GetComponent(const Entity& TargetEntity)
	{
		return GetPool<T>()->Get(TargetEntity);
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
	std::unordered_map<std::type_index, std::unique_ptr<IComponentPool>> ComponentPools;
};