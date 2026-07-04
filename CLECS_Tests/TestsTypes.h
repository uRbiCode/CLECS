#pragma once

struct TrackedComp
{
    explicit TrackedComp(int* Count) : DestructCount(Count) {}
    TrackedComp(const TrackedComp&) = delete;
    TrackedComp& operator=(const TrackedComp&) = delete;

    ~TrackedComp()
    {
        if (DestructCount != nullptr)
        {
            ++(*DestructCount);
        }
    }

    TrackedComp(TrackedComp&& Other) noexcept : DestructCount(Other.DestructCount)
    {
        Other.DestructCount = nullptr;
    }

    TrackedComp& operator=(TrackedComp&& Other) noexcept
    {
        DestructCount = Other.DestructCount;
        Other.DestructCount = nullptr;
        return *this;
    }

    int* DestructCount = nullptr;
};