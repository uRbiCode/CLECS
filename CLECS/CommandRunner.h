#pragma once
#include "ArchetypeStorage.h"
#include "AddEntitiesCommand.h"
#include "RemoveEntitiesCommand.h"
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

private:
	friend class World;

	std::vector<std::function<void(ArchetypeStorage&)>> PendingCommands;
};