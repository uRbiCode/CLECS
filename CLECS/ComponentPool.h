#pragma once
#include "Entity.h"
#include <vector>
#include <limits>

// Base class allows storing different component pools in a single container.
class ComponentPoolBase
{
public:
	virtual ~ComponentPoolBase() = default;
	virtual void Remove(const Entity& Entity) = 0;
	virtual bool Has(const Entity& Entity) const = 0;
	virtual void Clear() = 0;
};

constexpr uint32_t INVALID_DENSE_INDEX = std::numeric_limits<uint32_t>::max();

// Templated component pool utilizing sparse set.
template<typename T>
class ComponentPool : public ComponentPoolBase
{
public:
	// Emplace a new component for the given entity.
	template<typename... Args>
	T& Emplace(const Entity& Entity, Args&&... Arguments)
	{
		const auto EntityId = Entity.GetId();
		
		if (EntityId >= Sparse.size())
		{
			Sparse.resize(EntityId + 1, INVALID_DENSE_INDEX);
		}

		if (Sparse[EntityId] != INVALID_DENSE_INDEX)
			return Components[Sparse[EntityId]];

		const auto DenseIndex = static_cast<uint32_t>(Components.size());
		Sparse[EntityId] = DenseIndex;
		Entities.push_back(Entity);
		Components.emplace_back(std::forward<Args>(Arguments)...);

		return Components.back();
	}

	T& Access(const Entity& Entity)
	{
		const auto EntityId = Entity.GetId();
		return Components[Sparse[EntityId]];
	}

	const T& Get(const Entity& Entity) const
	{
		const auto EntityId = Entity.GetId();
		return Components[Sparse[EntityId]];
	}

	// O(1) removal by swapping with the last element.
	void Remove(const Entity& Entity) override
	{
		const auto EntityId = Entity.GetId();
		if (EntityId >= Sparse.size() || Sparse[EntityId] == INVALID_DENSE_INDEX)
			return;

		const auto DenseIndex = Sparse[EntityId];
		const auto LastIndex = static_cast<uint32_t>(Components.size() - 1);

		if (DenseIndex != LastIndex)
		{
			Components[DenseIndex] = std::move(Components[LastIndex]);
			Entities[DenseIndex] = Entities[LastIndex];
			
			const auto SwappedEntityId = Entities[DenseIndex].GetId();
			Sparse[SwappedEntityId] = DenseIndex;
		}

		Components.pop_back();
		Entities.pop_back();
		Sparse[EntityId] = INVALID_DENSE_INDEX;
	}

	bool Has(const Entity& Entity) const override
	{
		const auto EntityId = Entity.GetId();
		if (EntityId >= Sparse.size())
			return false;

		const auto DenseIndex = Sparse[EntityId];
		if (DenseIndex == INVALID_DENSE_INDEX)
			return false;

		return Entities[DenseIndex] == Entity;
	}

	std::vector<T>& AccessComponents()	{ return Components; }

	const std::vector<T>& GetComponents() const	{ return Components; }

	const std::vector<Entity>& GetEntities() const{ return Entities; }

	void Clear() override
	{
		Sparse.clear();
		Components.clear();
		Entities.clear();
	}

	size_t Size() const { return Components.size(); }

private:
	// Sparse array. At the position equal to entity Id, it stores the index at which the owned component and this entity are located in the dense arrays.
	std::vector<uint32_t> Sparse;
	
	// Dense arrays storing components and their corresponding entities.
	std::vector<T> Components;
	std::vector<Entity> Entities;
};