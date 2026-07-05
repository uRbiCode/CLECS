#include "RemoveEntitiesCommand.h"

RemoveEntitiesCommand::RemoveEntitiesCommand(size_t EntityCount)
{
	Entries.reserve(EntityCount);
}

RemoveEntitiesCommand& RemoveEntitiesCommand::WithEntry(Entity EntityToRemove)
{
	Entries.push_back(EntityToRemove);
	return *this;
}

const std::vector<Entity>& RemoveEntitiesCommand::GetEntries() const
{
	return Entries;
}
