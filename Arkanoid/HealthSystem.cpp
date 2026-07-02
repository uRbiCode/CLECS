#include "HealthSystem.h"
#include "SystemContext.h"
#include "Query.h"
#include "HealthComponent.h"
#include "CommandRunner.h"
#include "ComponentUtils.h"

void HealthSystem::CleanupHealthDeltaComponents(SystemContext& Context, float DeltaTime)
{
	ComponentUtils::RemoveAllComponentsTyped<HealthDeltaComponent>(Context);
}

void HealthSystem::ApplyHealthChanges(SystemContext& Context, float DeltaTime)
{
	Query<WritesList<HealthComponent>, ReadsList<HealthDeltaComponent>, ExcludeList<>> HealthQuery(Context.QueryContext);
	if (HealthQuery.Size() < 1)
		return;

	RemoveEntitiesCommand RemoveDeadEntitiesCommand(0);
	HealthQuery.ForEach([&](Entity Entity, HealthComponent& Health, const HealthDeltaComponent& HealthDelta)
	{
		Health.CurrentHealth += HealthDelta.Delta;
		if (Health.CurrentHealth <= 0)
		{
			RemoveDeadEntitiesCommand.WithEntry(Entity);
		}
	});

	if (!RemoveDeadEntitiesCommand.GetEntries().empty())
	{
		Context.Commands.Submit(std::move(RemoveDeadEntitiesCommand));
	}
}