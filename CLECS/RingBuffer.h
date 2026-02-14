#pragma once

#include <array>
#include <optional>

/* Circular buffer implementation for CLECS.
 * It uses fixed capacity and can't be resized.
 */
template<typename T, size_t Capacity>
class RingBuffer 
{
public:
    RingBuffer() : Head(0), Tail(0) {}

    virtual bool Push(const T& Value) 
    {
        const size_t NextTail = Increment(Tail);

        if (NextTail == Head) 
            return false;

        Buffer[Tail] = Value;
        Tail = NextTail;
        return true;
    }

    virtual bool Push(T&& Value) 
    {
        const size_t NextTail = Increment(Tail);

        if (NextTail == Head)
            return false;

        Buffer[Tail] = std::move(Value);
        Tail = NextTail;
        return true;
    }

    std::optional<T> Pop() 
    {
        if (Head == Tail)
            return std::nullopt;

        T Value = std::move(Buffer[Head]);
        Head = Increment(Head);
        return Value;
    }

    bool IsEmpty() const { return Head == Tail; }

    bool IsFull() const { return Increment(Tail) == Head; }

    size_t Size() const 
    {
        if (Tail >= Head)
            return Tail - Head;

        return Capacity - (Head - Tail);
    }

    constexpr size_t GetCapacity() const { return Capacity; }

    void Clear() 
    {
        Head = 0;
        Tail = 0;
    }

protected:
    constexpr size_t Increment(size_t Index) const { return (Index + 1) % Capacity; }

    std::array<T, Capacity> Buffer;
    size_t Head = 0;
    size_t Tail = 0;
};