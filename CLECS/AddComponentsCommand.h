#pragma once
#include "CommandTypes.h"
#include "Entity.h"
#include <vector>
#include <tuple>
#include <type_traits>

/* Command that adds an exact set of Components to already-existing Entities.
 */
template<typename... Components>
class AddComponentsCommand
{
public:
	AddComponentsCommand(size_t EntityCount) { Entries.reserve(EntityCount); }

	template<typename... Args>
	requires ValidCommandArgs<TypeList<Components...>, Args...>
	AddComponentsCommand& WithEntry(Entity E, Args&&... ComponentData)
	{
		Entries.emplace_back(E, std::decay_t<Args>(std::forward<Args>(ComponentData))...);
		return *this;
	}

	std::vector<std::pair<Entity, std::tuple<Components...>>>& AccessEntries() { return Entries; }

private:
	std::vector<std::pair<Entity, std::tuple<Components...>>> Entries;
};