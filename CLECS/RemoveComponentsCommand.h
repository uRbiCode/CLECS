#pragma once
#include "Entity.h"
#include <vector>

/* Command that removes an exact set of Components from Entities
 */
template<typename... Components>
class RemoveComponentsCommand
{
public:
	RemoveComponentsCommand(size_t EntityCount) { Entries.reserve(EntityCount); }

	RemoveComponentsCommand& WithEntry(Entity E)
	{
		Entries.emplace_back(E);
		return *this;
	}

	const std::vector<Entity>& GetEntries() { return Entries; }

private:
	std::vector<Entity> Entries;
};