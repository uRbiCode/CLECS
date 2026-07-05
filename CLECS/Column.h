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
    Column(ColumnDescription&& InDescription);

    size_t Size() const;

    const ColumnDescription& GetDescription() const;

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

    void Reserve(size_t Count); 

    template<ComponentType T, typename... Args>
    void EmplaceBack(Args&&... Arguments)
    {
        const size_t Offset = Data.size();
        Data.resize(Offset + Description.ElementSize);
        new (Data.data() + Offset) T(std::forward<Args>(Arguments)...);
    }

    // The caller is responsible for SwapRemoving SourceRow from Source afterward. 
    void MoveAppendFrom(Column& Source, size_t SourceRow);

    void SwapRemove(size_t Row);    

private:
    void* AccessRawData();
    const void* GetRawData() const;
    void* At(size_t Row);
    const void* At(size_t Row) const;

    ColumnDescription Description;
    std::vector<uint8_t> Data;
};