#pragma once
#include "Column.h"
#include "Entity.h"
#include "ComponentTypesCollection.h"
#include <vector>
#include <unordered_map>
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
            Arch.Columns.emplace_back(Column(std::move(Ent.Description)));
		}

		for (size_t i = 0; i < Arch.ComponentTypes.size(); ++i)
		{
			Arch.ColumnIndexCache[Arch.ComponentTypes[i]] = i;
		}

		return Arch;
	}

    template<ComponentType... NewComponents>
    static Archetype MakeExtended(const Archetype& Existing, const ComponentTypesCollection& Types)
    {
        struct Entry
        {
            ComponentTypeId Id;
            ColumnDescription Description;
        };

        std::vector<Entry> Entries;
        Entries.reserve(Existing.ComponentTypes.size() + sizeof...(NewComponents));

        for (size_t i = 0; i < Existing.ComponentTypes.size(); ++i)
        {
            Entries.push_back({ Existing.ComponentTypes[i], Existing.Columns[i].GetDescription() });
        }

        ([&]()
            {
                const ComponentTypeId Id = Types.GetComponentTypeId<NewComponents>();
                const bool AlreadyPresent = std::ranges::any_of(Entries, [Id](const Entry& E) { return E.Id == Id; });
                if (!AlreadyPresent)
                {
                    Entries.push_back({ Id, ColumnDescription::Make<NewComponents>() });
                }
            }(), ...);

        std::ranges::sort(Entries, {}, &Entry::Id);

        Archetype Arch;
        Arch.ComponentTypes.reserve(Entries.size());
        Arch.Columns.reserve(Entries.size());
        Arch.ColumnIndexCache.reserve(Entries.size());

        for (Entry& Ent : Entries)
        {
            Arch.ComponentTypes.push_back(Ent.Id);
            Arch.Columns.emplace_back(Column(std::move(Ent.Description)));
        }

        for (size_t i = 0; i < Arch.ComponentTypes.size(); ++i)
            Arch.ColumnIndexCache[Arch.ComponentTypes[i]] = i;

        return Arch;
    }

    /* Creates a new empty Archetype whose columns are exactly those in TargetKey.
     * TargetKey must be a sorted subset of Existing's component types.
     */
    static Archetype MakeNarrowed(const Archetype& Existing, const std::vector<ComponentTypeId>& TargetKey)
    {
        Archetype Arch;
        Arch.ComponentTypes.reserve(TargetKey.size());
        Arch.Columns.reserve(TargetKey.size());
        Arch.ColumnIndexCache.reserve(TargetKey.size());

        for (const ComponentTypeId Id : TargetKey)
        {
            const size_t SrcColIdx = Existing.GetColumnIndex(Id);
            ColumnDescription Desc = Existing.Columns[SrcColIdx].GetDescription();
            Arch.ComponentTypes.push_back(Id);
            Arch.Columns.emplace_back(Column(std::move(Desc)));
        }

        for (size_t i = 0; i < Arch.ComponentTypes.size(); ++i)
        {
            Arch.ColumnIndexCache[Arch.ComponentTypes[i]] = i;
        }

        return Arch;
    }

    size_t GetColumnIndex(ComponentTypeId CompId) const
    {
        const auto It = ColumnIndexCache.find(CompId);
        return It != ColumnIndexCache.end() ? It->second : SIZE_MAX;
    }

    bool HasComponentType(ComponentTypeId CompId) const
    {
        return ColumnIndexCache.contains(CompId);
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

    void MigrateRowTo(Entity E, Archetype& Target)
    {
        const size_t Row = GetEntityRow(E);

        Target.Entities.push_back(E);

        for (size_t i = 0; i < ComponentTypes.size(); ++i)
        {
            if (!Target.HasComponentType(ComponentTypes[i]))
				continue;

            const size_t TargetColIdx = Target.GetColumnIndex(ComponentTypes[i]);
            Target.Columns[TargetColIdx].MoveAppendFrom(Columns[i], Row);
        }

        SwapRemoveAt(Row);
    }

    void SwapRemoveRow(Entity E)
    {
        SwapRemoveAt(GetEntityRow(E));
    }

    size_t Size() const { return Entities.size(); }
    const std::vector<Entity>& GetEntities() const { return Entities; }

private:
    void SwapRemoveAt(size_t Row)
    {
        for (Column& Col : Columns)
        {
            Col.SwapRemove(Row);
        }

		std::swap(Entities[Row], Entities.back());
        Entities.pop_back();
    }

    size_t GetEntityRow(Entity E) const
    {
        const auto It = std::find(Entities.begin(), Entities.end(), E);
        return static_cast<size_t>(std::distance(Entities.begin(), It));
    }

    std::vector<ComponentTypeId> ComponentTypes;
    std::vector<Column> Columns;
    std::vector<Entity> Entities;

    std::unordered_map<ComponentTypeId, size_t> ColumnIndexCache;
};