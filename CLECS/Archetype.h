#pragma once
#include "Column.h"
#include "Entity.h"
#include "ComponentTypesCollection.h"
#include <vector>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <ranges>

class Archetype
{
public:
	template<typename... Components>
	static Archetype MakeArchetype(const ComponentTypesCollection& Types)
	{
		struct Entry
		{
			ComponentTypeId Id;
			ColumnDescription Description;
		};

		std::array<Entry, sizeof...(Components)> Entries
		{ 
            {{ Types.GetComponentTypeId<Components>(), ColumnDescription::Make<Components>() }...}
		};

		std::ranges::sort(Entries, {}, &Entry::Id);
		constexpr size_t ComponentsSize = sizeof...(Components);

		Archetype Arch;
		Arch.ComponentTypes.reserve(ComponentsSize);
		Arch.Columns.reserve(ComponentsSize);
		Arch.ColumnIndexCache.reserve(ComponentsSize);

		for (Entry& Ent : Entries)
		{
			Arch.ComponentTypes.push_back(Ent.Id);

			Column Col(std::move(Ent.Description));
			Arch.Columns.emplace_back(std::move(Col));
		}

		for (size_t i = 0; i < Arch.ComponentTypes.size(); ++i)
		{
			Arch.ColumnIndexCache[Arch.ComponentTypes[i]] = i;
		}

		return Arch;
	}
    size_t GetColumnIndex(ComponentTypeId CompId) const
    {
        auto It = ColumnIndexCache.find(CompId);
        return It != ColumnIndexCache.end() ? It->second : SIZE_MAX;
    }

    bool HasColumn(ComponentTypeId CompId) const
    {
        return ColumnIndexCache.find(CompId) != ColumnIndexCache.end();
    }

    template<ComponentType T>
    T* AccessColumn(size_t ColumnIndex)
    {
        return Columns[ColumnIndex].AccessData<T>();
    }

    template<typename... Components>
    void EmplaceTypedRow(const ComponentTypesCollection& TypeMap, Entity E, Components&&... Values)
    {
        Entities.push_back(E);
        (EmplaceInColumn(TypeMap.GetComponentTypeId<Components>(), std::forward<Components>(Values)), ...);
    }

    template<ComponentType T>
    void EmplaceInColumn(ComponentTypeId CompId, T&& Value)
    {
        const size_t Idx = GetColumnIndex(CompId);
        Columns[Idx].EmplaceBack<T>(std::forward<T>(Value));
    }

    template<ComponentType T>
    const T* GetColumn(size_t ColumnIndex) const
    {
        return Columns[ColumnIndex].GetData<T>();
    }

    void Reserve(size_t Count)
    {
        Entities.reserve(Count);
        for (Column& Col : Columns)
        {
            Col.Reserve(Count);
        }
    }

    void SwapRemoveRow(Entity E)
    {
        const size_t Row = GetEntityRow(E);

        for (Column& Col : Columns)
        {
            Col.SwapRemove(Row);
        }

		std::swap(Entities[Row], Entities.back());
        Entities.pop_back();
    }

    size_t Size() const { return Entities.size(); }
    const std::vector<Entity>& GetEntities() const { return Entities; }

private:
    size_t GetEntityRow(Entity E) const
    {
        auto It = std::find(Entities.begin(), Entities.end(), E);
        return static_cast<size_t>(std::distance(Entities.begin(), It));
    }

    std::vector<ComponentTypeId> ComponentTypes;
    std::vector<Column> Columns;
    std::vector<Entity> Entities;

    std::unordered_map<ComponentTypeId, size_t> ColumnIndexCache;
};