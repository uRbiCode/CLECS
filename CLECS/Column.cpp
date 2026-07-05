#include "Column.h"

Column::Column(ColumnDescription&& InDescription) : Description(std::move(InDescription))
{
}

size_t Column::Size() const
{
	return Data.size() / Description.ElementSize;
}

const ColumnDescription& Column::GetDescription() const
{
    return Description;
}

void Column::MoveAppendFrom(Column& Source, size_t SourceRow)
{
	const size_t NewIdx = Size();
	Data.resize(Data.size() + Description.ElementSize);
	Description.MoveConstruct(At(NewIdx), Source.At(SourceRow));
}

void Column::SwapRemove(size_t Row)
{
	const size_t LastRow = Size() - 1;
	if (Row != LastRow)
	{
		Description.Destruct(At(Row));
		Description.MoveConstruct(At(Row), At(LastRow));
	}
	Description.Destruct(At(LastRow));
	Data.resize(Data.size() - Description.ElementSize);
}

void* Column::AccessRawData()
{
    return Data.data();
}

const void* Column::GetRawData() const
{
    return Data.data();
}

void* Column::At(size_t Row)
{
	return Data.data() + Row * Description.ElementSize; 
}

const void* Column::At(size_t Row) const
{
    return Data.data() + Row * Description.ElementSize;
}

void Column::Reserve(size_t Count)
{
    Data.reserve(Count * Description.ElementSize);
}