#pragma once
#include "Table.h"
#include "Entity.h"
#include <set>
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
};

template<typename... Components>
class Archetype : public ArchetypeBase
{
public:
	Archetype() = default;
	Archetype(const Archetype&) = delete;
	Archetype& operator=(const Archetype&) = delete;

	void SwapRemoveRow(Entity Entity) override
	{
		InternalTable.SwapRemoveRow(Entity);
	}

	void EmplaceBack(Entity Entity, Components&&... Args)
	{
		InternalTable.EmplaceBack(Entity, std::forward(Args)...);
	}

	template<typename T>
	bool HasColumn() const
	{
		return InternalTable.template HasColumn<T>();
	}

	template<typename T>
	const Column<T>& GetColumn() const
	{
		return InternalTable.template GetColumn<T>();
	}

private:
	Table<Components...> InternalTable;
};