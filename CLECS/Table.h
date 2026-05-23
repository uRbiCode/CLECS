#pragma once
#include "Column.h"
#include "Entity.h"
#include <unordered_map>
#include <memory>
#include <typeindex>

template <typename... Components>
class Table
{
public:
	void EmplaceBack(Entity Entity, Components&&... Args)
	{
		Entities.push_back(Entity);

		((std::get<Column<Components>>(Columns).EmplaceBack(std::move(Args)), 0), ...);
	}
	
	void SwapRemoveRow(const Entity& Entity)
	{
		size_t Row = GetColumnIndex(Entity);

		(std::get<Column<Components>>(Columns).SwapRemove(Row), ...);

		std::swap(Entities[Row], Entities.back());
		Entities.pop_back();
	}

	template<typename T>
	bool HasColumn() const
	{
		return (std::is_same_v<T, Components> || ...);
	}

	template <typename T>
	requires (HasColumn<T>())
	const Column<T>& GetColumn() const
	{
		return std::get<Column<T>>(Columns);
	}

private:
	size_t GetColumnIndex(Entity Entity) const
	{
		auto Iterator = std::find(Entities.begin(), Entities.end(), Entity);
		return static_cast<size_t>(std::distance(Entities.begin(), Iterator));
	}

	std::vector<Entity> Entities;
	std::tuple<Column<Components>...> Columns;
};
