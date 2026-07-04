#pragma once

class ArchetypeStorage;

/* QueryContext provides controlled access to ArchetypeStorage for Systems.
 */
class QueryContext
{
public:
    static QueryContext Create(ArchetypeStorage* Storage)
    {
        QueryContext Context{};
		Context.Archetypes = Storage;
        return Context;
	}

    // TODO: TRY TO PRIVATE ARCHETYPESTORAGE AND DO SOME FRIEND MAGIC OR SOMETHING
    ArchetypeStorage* Archetypes = nullptr;
};