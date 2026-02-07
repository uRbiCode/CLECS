#pragma once
#include "Entity.h"
#include "Component.h"
#include <vector>

template<class T>
concept ComponentType = std::derived_from<T, Component>;

/* Base class for type-erased component storage.
 * Allows to manage different component types uniformly.
 */
class IComponentPool
{
public:
	virtual ~IComponentPool() = default;
	virtual void Remove(const Entity& TargetEntity) = 0;
	virtual bool Has(const Entity& TargetEntity) const = 0;
	virtual void Clear() = 0;
};

/* Templated component pool using sparse set for O(1) lookups.
 * Sparse array maps entity IDs to dense array indices.
 */
template<ComponentType T>
class ComponentPool : public IComponentPool
{
public:
	// Add component to entity
	template<typename... Args>
	T& Emplace(const Entity& TargetEntity, Args&&... Arguments)
	{
		const auto EntityId = TargetEntity.GetId();
		
		// Ensure sparse array is large enough
		if (EntityId >= Sparse.size())
		{
			Sparse.resize(EntityId + 1, INVALID_ENTITY_IDENTIFIER);
		}

		// Check if entity already has this component
		if (Sparse[EntityId] != INVALID_ENTITY_IDENTIFIER)
			return Components[Sparse[EntityId]];

		// Add new component
		const auto DenseIndex = static_cast<uint32_t>(Components.size());
		Sparse[EntityId] = DenseIndex;
		Entities.push_back(TargetEntity);
		Components.emplace_back(std::forward<Args>(Arguments)...);

		return Components.back();
	}

	T& Get(const Entity& TargetEntity)
	{
		const auto EntityId = TargetEntity.GetId();
		return Components[Sparse[EntityId]];
	}

	const T& Get(const Entity& TargetEntity) const
	{
		const auto EntityId = TargetEntity.GetId();
		return Components[Sparse[EntityId]];
	}

	void Remove(const Entity& TargetEntity) override
	{
		const auto EntityId = TargetEntity.GetId();
		if (EntityId >= Sparse.size() || Sparse[EntityId] == INVALID_ENTITY_IDENTIFIER)
			return;

		const auto DenseIndex = Sparse[EntityId];
		const auto LastIndex = static_cast<uint32_t>(Components.size() - 1);

		if (DenseIndex != LastIndex)
		{
			Components[DenseIndex] = std::move(Components[LastIndex]);
			Entities[DenseIndex] = Entities[LastIndex];
			
			// Update sparse index for swapped entity
			const auto SwappedEntityId = Entities[DenseIndex].GetId();
			Sparse[SwappedEntityId] = DenseIndex;
		}

		Components.pop_back();
		Entities.pop_back();
		Sparse[EntityId] = INVALID_ENTITY_IDENTIFIER;
	}

	bool Has(const Entity& TargetEntity) const override
	{
		const auto EntityId = TargetEntity.GetId();
		return EntityId < Sparse.size() && Sparse[EntityId] != INVALID_ENTITY_IDENTIFIER;
	}

	std::vector<T>& GetComponents()
	{
		return Components;
	}

	const std::vector<T>& GetComponents() const
	{
		return Components;
	}

	const std::vector<Entity>& GetEntities() const
	{
		return Entities;
	}

	void Clear() override
	{
		Sparse.clear();
		Components.clear();
		Entities.clear();
	}

	size_t Size() const
	{
		return Components.size();
	}

private:
	// Sparse set: entity ID -> dense index
	std::vector<uint32_t> Sparse;
	
	// Dense arrays for cache-friendly iteration
	std::vector<T> Components;
	std::vector<Entity> Entities;
};