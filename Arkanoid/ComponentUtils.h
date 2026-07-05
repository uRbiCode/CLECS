#pragma once
#include "SystemContext.h"
#include "CommandRunner.h"
#include "Query.h"

namespace ComponentUtils
{
	template<ComponentType T>
	void RemoveAllComponentsTyped(SystemContext& Context)
	{
		const Query<WritesList<>, ReadsList<T>, ExcludeList<>> RemoveQuery(Context.QueryContext);
		if (RemoveQuery.Size() < 1)
			return;

		RemoveComponentsCommand<T> RemoveCommand(RemoveQuery.Size());
		RemoveQuery.ForEach([&](Entity Entity, [[maybe_unused]] const T& Component)
		{
			RemoveCommand.WithEntry(Entity);
		});

		Context.Commands.Submit(std::move(RemoveCommand));
	}

	template<ComponentType T>
	void RemoveAllEntitiesWithComponent(SystemContext& Context)
	{
		const Query<WritesList<>, ReadsList<T>, ExcludeList<>> RemoveQuery(Context.QueryContext);
		if (RemoveQuery.Size() < 1)
			return;

		RemoveEntitiesCommand RemoveCommand(RemoveQuery.Size());
		RemoveQuery.ForEach([&](Entity Entity, [[maybe_unused]] const T& Component)
		{
			RemoveCommand.WithEntry(Entity);
		});

		Context.Commands.Submit(std::move(RemoveCommand));
	}
}