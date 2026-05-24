#pragma once
#include <tuple>
#include <vector>
#include <type_traits>

template<typename... Components>
class AddEntitiesCommand
{
public:
	AddEntitiesCommand(size_t EntityCount) { Entries.reserve(EntityCount); }

	template<typename... Args>
	requires (sizeof...(Components) == sizeof...(Args) && (std::is_same_v<std::remove_cvref_t<Args>, Components> && ...))
	AddEntitiesCommand& WithEntry(Args&&... ComponentData)
	{
		Entries.emplace_back(std::decay_t<Args>(std::forward<Args>(ComponentData))...);
		return *this;
	}

	std::vector<std::tuple<Components...>>& AccessEntries() const { return Entries; }

private:
	std::vector<std::tuple<Components...>> Entries;
};