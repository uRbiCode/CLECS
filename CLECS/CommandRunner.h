#pragma once
#include "ArchetypeStorage.h"
#include <functional>
#include <vector>

/* CommandRunner queues structural world-mutation Commands submitted by Systems during their update pass.
 */
class CommandRunner
{
public:
	template<typename... Components>
	void Submit(AddEntitiesCommand<Components...>&& Command)
	{
		PendingCommands.emplace_back([Cmd = std::move(Command)](ArchetypeStorage& Archetypes) mutable
		{
			Archetypes.EmplaceEntities(std::move(Cmd));
		});
	}

	void Submit(RemoveEntitiesCommand&& Command)
	{
		PendingCommands.emplace_back([Cmd = std::move(Command)](ArchetypeStorage& Archetypes) mutable
		{
			Archetypes.RemoveEntities(std::move(Cmd));
		});
	}

	template<typename... Components>
	void Submit(AddComponentsCommand<Components...>&& Command)
	{
		PendingCommands.emplace_back([Cmd = std::move(Command)](ArchetypeStorage& Archetypes) mutable
		{
			Archetypes.AddComponents(std::move(Cmd));
		});
	}

	template<typename... Components>
	void Submit(RemoveComponentsCommand<Components...>&& Command)
	{
		PendingCommands.emplace_back([Cmd = std::move(Command)](ArchetypeStorage& Archetypes) mutable
		{
			Archetypes.RemoveComponents(std::move(Cmd));
		});
	}

	void Flush(ArchetypeStorage& Archetypes)
	{
		for (auto& Command : PendingCommands)
		{
			Command(Archetypes);
		}
		PendingCommands.clear();
	}

private:
	std::vector<std::function<void(ArchetypeStorage&)>> PendingCommands;
};