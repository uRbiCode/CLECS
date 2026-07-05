#pragma once

class ArchetypeStorage;

/* QueryContext provides controlled access to ArchetypeStorage for Systems.
 */
class QueryContext
{
public:
    static QueryContext Create(ArchetypeStorage* Storage);

    ArchetypeStorage* Archetypes = nullptr;
};