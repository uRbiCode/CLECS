#pragma once

struct TrackedComponent
{
    explicit TrackedComponent(int* Count) : DestructCount(Count) {}
    TrackedComponent(const TrackedComponent&) = delete;
    TrackedComponent& operator=(const TrackedComponent&) = delete;

    ~TrackedComponent()
    {
        if (DestructCount != nullptr)
        {
            ++(*DestructCount);
        }
    }

    TrackedComponent(TrackedComponent&& Other) noexcept : DestructCount(Other.DestructCount)
    {
        Other.DestructCount = nullptr;
    }

    TrackedComponent& operator=(TrackedComponent&& Other) noexcept
    {
        DestructCount = Other.DestructCount;
        Other.DestructCount = nullptr;
        return *this;
    }

    int* DestructCount = nullptr;
};