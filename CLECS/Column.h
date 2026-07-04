#pragma once
#include <vector>
#include <cstddef>
#include <cstring>
#include <utility>

template<typename T>
concept ComponentType = (sizeof(T) > 0) && (!std::is_void_v<T>);

struct ColumnDescription
{
    template<ComponentType T>
    static ColumnDescription Make()
    {
        ColumnDescription Description;
        Description.ElementSize = sizeof(T);
        Description.MoveConstruct = [](void* Dest, void* Src) { new (Dest) T(std::move(*static_cast<T*>(Src))); };
        Description.Destruct = [](void* Ptr) { static_cast<T*>(Ptr)->~T(); };
        return Description;
    }

    size_t ElementSize = 0;
    void (*MoveConstruct)(void* Dest, void* Src) = nullptr;
    void (*Destruct)(void* Ptr) = nullptr;
};

struct Column
{
    Column(ColumnDescription&& InDescription) : Description(std::move(InDescription)) {}

    size_t Size() const { return Data.size() / Description.ElementSize; }

    const ColumnDescription& GetDescription() const { return Description; }

	template<ComponentType T>
    T* AccessData()
    {
        return static_cast<T*>(AccessRawData());
	}

	template<ComponentType T>
    const T* GetData() const
    {
        return static_cast<const T*>(GetRawData());
	}

    void Reserve(size_t Count)
    {
        Data.reserve(Count * Description.ElementSize);
    }

    template<ComponentType T, typename... Args>
    void EmplaceBack(Args&&... Arguments)
    {
        const size_t Offset = Data.size();
        Data.resize(Offset + Description.ElementSize);
        new (Data.data() + Offset) T(std::forward<Args>(Arguments)...);
    }

    // The caller is responsible for SwapRemoving SrcRow from Src afterward. 
    void MoveAppendFrom(Column& Src, size_t SrcRow)
    {
        const size_t NewIdx = Size();
        Data.resize(Data.size() + Description.ElementSize);
        Description.MoveConstruct(At(NewIdx), Src.At(SrcRow));
    }

    void SwapRemove(size_t Row)
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

private:
    void* AccessRawData() { return Data.data(); }
    const void* GetRawData() const { return Data.data(); }
    void* At(size_t Row) { return Data.data() + Row * Description.ElementSize; }
    const void* At(size_t Row) const { return Data.data() + Row * Description.ElementSize; }

    ColumnDescription Description;
    std::vector<uint8_t> Data;
};