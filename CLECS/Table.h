#pragma once
#include "Column.h"
#include "Entity.h"
#include <typeindex>
#include <vector>

template<typename... Args, typename... Components>
concept ValidTableArgs = requires { sizeof...(Components) == sizeof...(Args) && (HasColumn<Args>() && ...); };

template<typename... Components>
class Table
{
public:
	void Reserve(size_t EntityCount)
	{
		Entities.reserve(EntityCount);
		(std::get<Column<Components>>(Columns).Reserve(EntityCount), ...);
	}

	template<typename... Args>
	requires ValidTableArgs<Args..., Components...>
	void EmplaceBack(Entity Entity, Args&&... ArgValues)
	{
		Entities.push_back(Entity);
		(std::get<Column<std::decay_t<Args>>>(Columns).EmplaceBack(std::forward<Args>(ArgValues)), ...);
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

	template<typename T>
	requires (HasColumn<T>())
	Column<T>& AccessColumn()
	{
		return std::get<Column<T>>(Columns);
	}

	template<typename T>
	requires (HasColumn<T>())
	const Column<T>& GetColumn() const
	{
		return std::get<Column<T>>(Columns);
	}

	void* AccessColumnData(std::type_index Type)
	{
		void* Result = nullptr;
		((std::type_index(typeid(Components)) == Type
			? (Result = std::get<Column<Components>>(Columns).AccessData().data(), true)
			: false) || ...);
		return Result;
	}

	const void* GetColumnData(std::type_index Type) const
	{
		const void* Result = nullptr;
		((std::type_index(typeid(Components)) == Type
			? (Result = std::get<Column<Components>>(Columns).GetData().data(), true)
			: false) || ...);
		return Result;
	}

	const std::vector<Entity>& GetEntities() const { return Entities; }

	size_t Size() const { return Entities.size(); }

private:
	size_t GetColumnIndex(Entity Entity) const
	{
		auto Iterator = std::find(Entities.begin(), Entities.end(), Entity);
		return static_cast<size_t>(std::distance(Entities.begin(), Iterator));
	}

	std::vector<Entity> Entities;
	std::tuple<Column<Components>...> Columns;
};