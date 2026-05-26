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
	virtual void SwapRemoveRow(Entity Entity) = 0;
	virtual void Reserve(size_t EntityCount) = 0;
	virtual size_t Size() const = 0;
	virtual const std::vector<Entity>& GetEntities() const = 0;

	//TODO: THINK IF ACCESS BY COMPONENTTYPEID
	virtual void* AccessColumnData(std::type_index Type) = 0;
	virtual const void* GetColumnData(std::type_index Type) const = 0;
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

	size_t Size() const override
	{
		return InternalTable.Size();
	}

	const std::vector<Entity>& GetEntities() const override
	{
		return InternalTable.GetEntities();
	}

	void* AccessColumnData(std::type_index Type) override
	{
		return InternalTable.AccessColumnData(Type);
	}

	const void* GetColumnData(std::type_index Type) const override
	{
		return InternalTable.GetColumnData(Type);
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
	Column<T>& AccessColumn()
	{
		return InternalTable.template AccessColumn<T>();
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