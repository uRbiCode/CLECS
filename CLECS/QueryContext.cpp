#include "QueryContext.h"

QueryContext QueryContext::Create(ArchetypeStorage* Storage)
{
	QueryContext Context{};
	Context.Archetypes = Storage;
	return Context;
}