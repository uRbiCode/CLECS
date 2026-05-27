#pragma once

class ArchetypeStorage;
template<typename, typename> class Query;

/* QueryContext provides controlled access to ArchetypeStorage for Systems.
 */
class QueryContext
{
public:
    QueryContext(ArchetypeStorage& Storage) : Archetypes(Storage) {}

private:
    template<typename, typename> friend class Query;

    ArchetypeStorage& Archetypes;
};