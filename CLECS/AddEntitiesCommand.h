#pragma once
#include "CommandTypes.h"
#include <tuple>
#include <vector>
#include <type_traits>

/* Command creates Enitites with provided Components.
 */
template<typename... Components>
class AddEntitiesCommand
{
public:
	AddEntitiesCommand(size_t EntityCount) { Entries.reserve(EntityCount); }

	template<typename... Args>
	requires ValidCommandArgs<Components..., Args...>
	AddEntitiesCommand& WithEntry(Args&&... ComponentData)
	{
		Entries.emplace_back(std::decay_t<Args>(std::forward<Args>(ComponentData))...);
		return *this;
	}

	const std::vector<std::tuple<Components...>>& AccessEntries() const { return Entries; }

private:
	std::vector<std::tuple<Components...>> Entries;
};