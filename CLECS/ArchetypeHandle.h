#pragma once
#include "Archetype.h"
#include "ComponentTypesCollection.h"
#include "Entity.h"
#include <array>
#include <vector>
#include <cstddef>

template<typename Candidate, typename... Components>
concept MatchingComponent = (std::same_as<Candidate, Components> || ...);

/* Handle that provides access to specific subset of Archetypes data
 */
template<ComponentType... Components>
class ArchetypeHandle
{
public:
	ArchetypeHandle(Archetype& InArchetype, const ComponentTypesCollection& Types) : CachedArchetype(&InArchetype), ComponentTypes(&Types)
    {
        FillIndices(std::index_sequence_for<Components...>{});
    }

    template<ComponentType T>
    requires MatchingComponent<T, Components...>
    T* AccessComponents()
    {
        return CachedArchetype->AccessColumn<T>(ResolvedIndices[SlotOf<T>()]);
    }

    template<ComponentType T>
    requires MatchingComponent<T, Components...>
    const T* GetComponents() const
    {
        return CachedArchetype->GetColumn<T>(ResolvedIndices[SlotOf<T>()]);
    }

    const std::vector<Entity>& GetEntities() const { return CachedArchetype->GetEntities(); }
    size_t Size() const { return CachedArchetype->Size(); }

	template<ComponentType T>
    bool HasComponentType() const
    {
        return CachedArchetype->HasComponentType(ComponentTypes->GetComponentTypeId<T>());
	}

private:
    template<size_t... Index>
    void FillIndices(std::index_sequence<Index...>)
    {
        ((ResolvedIndices[Index] = CachedArchetype->GetColumnIndex(ComponentTypes->GetComponentTypeId<Components>())), ...);
    }

    // Compile-time index of T within Components...
    template<ComponentType T>
    static consteval size_t SlotOf()
    {
        size_t Slot = 0;
        bool Found = false;
        ((Found || (std::same_as<T, Components> ? (Found = true, true) : (++Slot, false))), ...);
        return Slot;
    }

    Archetype* CachedArchetype = nullptr;
	const ComponentTypesCollection* ComponentTypes = nullptr;
    std::array<size_t, sizeof...(Components)> ResolvedIndices;
};