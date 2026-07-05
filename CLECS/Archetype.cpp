#include "Archetype.h"

Archetype Archetype::MakeNarrowed(const Archetype& Existing, const std::vector<ComponentTypeId>& TargetKey)
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

	size_t i = 0;
	for (const ComponentTypeId Id : Arch.ComponentTypes)
	{
		Arch.ColumnIndexCache[Id] = i++;
	}

	return Arch;
}

size_t Archetype::GetColumnIndex(ComponentTypeId CompId) const
{
	const auto It = ColumnIndexCache.find(CompId);
	return It != ColumnIndexCache.end() ? It->second : InvalidColumnIndex;
}

bool Archetype::HasComponentType(ComponentTypeId CompId) const
{
	return ColumnIndexCache.contains(CompId);
}

void Archetype::Reserve(size_t Count)
{
	Entities.reserve(Count);
	for (Column& Col : Columns)
	{
		Col.Reserve(Count);
	}
}

void Archetype::MigrateRowTo(Entity E, Archetype& Target)
{
	const size_t Row = GetEntityRow(E);

	Target.Entities.push_back(E);

	std::vector<Column>::iterator ColIt = Columns.begin();
	for (const ComponentTypeId CompId : ComponentTypes)
	{
		const size_t TargetColumnIndex = Target.GetColumnIndex(CompId);
		if (TargetColumnIndex != InvalidColumnIndex)
		{
			Target.Columns[TargetColumnIndex].MoveAppendFrom(*ColIt, Row);
		}
		++ColIt;
	}

	SwapRemoveAt(Row);
}

void Archetype::SwapRemoveRow(Entity E)
{
	SwapRemoveAt(GetEntityRow(E));
}

void Archetype::SwapRemoveAt(size_t Row)
{
	for (Column& Col : Columns)
	{
		Col.SwapRemove(Row);
	}

	std::swap(Entities[Row], Entities.back());
	Entities.pop_back();
}

size_t Archetype::GetEntityRow(Entity E) const
{
	const auto It = std::find(Entities.begin(), Entities.end(), E);
	return static_cast<size_t>(std::distance(Entities.begin(), It));
}

size_t Archetype::Size() const
{
	return Entities.size();
}

const std::vector<Entity>& Archetype::GetEntities() const
{
	return Entities;
}