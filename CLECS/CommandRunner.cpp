#include "CommandRunner.h"

void CommandRunner::Submit(RemoveEntitiesCommand&& Command)
{
	PendingCommands.emplace_back([Cmd = std::move(Command)](ArchetypeStorage& Archetypes) mutable
	{
		Archetypes.RemoveEntities(std::move(Cmd));
	});
}

void CommandRunner::Flush(ArchetypeStorage& Archetypes)
{
	for (auto& Command : PendingCommands)
	{
		Command(Archetypes);
	}
	PendingCommands.clear();
}