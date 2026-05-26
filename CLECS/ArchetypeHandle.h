#pragma once
#include "Archetype.h"
#include "Entity.h"
#include <typeindex>
#include <vector>

/* Handle that provides access to specific subset of Archtypes data
 */
template<typename... Components>
class ArchetypeHandle
{
public:
    ArchetypeHandle(ArchetypeBase& InArchetype) : Archetype(&InArchetype) {}

    template<typename T>
    T* AccessComponents()
    {
        return static_cast<T*>(Archetype->AccessColumnData(std::type_index(typeid(T))));
    }

    template<typename T>
    const T* GetComponents() const
    {
        return static_cast<const T*>(Archetype->GetColumnData(std::type_index(typeid(T))));
    }

    const std::vector<Entity>& GetEntities() const { return Archetype->GetEntities(); }
    size_t Size() const { return Archetype->Size(); }

private:
    ArchetypeBase* Archetype;
};