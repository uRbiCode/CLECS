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

	for (size_t i = 0; i < Arch.ComponentTypes.size(); ++i)
	{
		Arch.ColumnIndexCache[Arch.ComponentTypes[i]] = i;
	}

	return Arch;
}

size_t Archetype::GetColumnIndex(ComponentTypeId CompId) const
{
	const auto It = ColumnIndexCache.find(CompId);
	return It != ColumnIndexCache.end() ? It->second : SIZE_MAX;
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

	for (size_t i = 0; i < ComponentTypes.size(); ++i)
	{
		if (!Target.HasComponentType(ComponentTypes[i]))
			continue;

		const size_t TargetColIdx = Target.GetColumnIndex(ComponentTypes[i]);
		Target.Columns[TargetColIdx].MoveAppendFrom(Columns[i], Row);
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