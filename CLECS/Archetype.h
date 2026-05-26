#pragma once
#include "Table.h"
#include "Entity.h"
#include <typeindex>
#include <vector>
#include <cstddef>
#include <type_traits>
#include <algorithm>

class ArchetypeBase
{
public:
	virtual ~ArchetypeBase() = default;
	// TODO: TRY TO REMOVE VIRTUALS AND USE TEMPLATED STATIC POLYMORPHISM INSTEAD
	virtual void SwapRemoveRow(Entity Entity) = 0;
	virtual void Reserve(size_t EntityCount) = 0;
};

template<typename... Components>
class Archetype : public ArchetypeBase
{
public:
	Archetype() = default;
	Archetype(const Archetype&) = delete;
	Archetype& operator=(const Archetype&) = delete;

	void Reserve(size_t EntityCount) override
	{
		InternalTable.Reserve(EntityCount);
	}

	void SwapRemoveRow(Entity Entity) override
	{
		InternalTable.SwapRemoveRow(Entity);
	}

	template<typename... Args>
	requires ValidTableArgs<Args..., Components...>
	void EmplaceBack(Entity Entity, Args&&... ArgValues)
	{
		InternalTable.EmplaceBack(Entity, std::forward<Args>(ArgValues)...);
	}

	template<typename T>
	bool HasColumn() const
	{
		return InternalTable.template HasColumn<T>();
	}

	template<typename T>
	requires (HasColumn<T>())
	const Column<T>& GetColumn() const
	{
		return InternalTable.template GetColumn<T>();
	}

private:
	Table<Components...> InternalTable;
};