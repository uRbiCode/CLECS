#include "HealthSystem.h"
#include "SystemContext.h"
#include "Query.h"
#include "HealthComponent.h"
#include "CommandRunner.h"
#include "ComponentUtils.h"
#include "TransitionComponents.h"
#include "RenderComponents.h"
#include "GlobalConstants.h"
#include "CollisionUtils.h"
#include "RenderComponents.h"
#include "ShapeComponents.h"
#include "PositionComponent.h"

void HealthSystem::RemoveDeadEntities(SystemContext& Context, float DeltaTime)
{
	const Query<WritesList<>, ReadsList<HealthComponent>, ExcludeList<>> HealthQuery(Context.QueryContext);
	RemoveEntitiesCommand RemoveDeadEntitiesCommand(0);
	HealthQuery.ForEach([&](Entity Entity, const HealthComponent& Health)
	{
		if (Health.CurrentHealth > 0)
			return;

		RemoveDeadEntitiesCommand.WithEntry(Entity);
	});

	if (!RemoveDeadEntitiesCommand.GetEntries().empty())
	{
		Context.Commands.Submit(std::move(RemoveDeadEntitiesCommand));
	}
}

void HealthSystem::CleanupHealthDeltaComponents(SystemContext& Context, float DeltaTime)
{
	ComponentUtils::RemoveAllComponentsTyped<HealthDeltaComponent>(Context);
}

void HealthSystem::ApplyHealthChanges(SystemContext& Context, float DeltaTime)
{
	const Query<WritesList<HealthComponent>, ReadsList<HealthDeltaComponent>, ExcludeList<>> HealthQuery(Context.QueryContext);
	HealthQuery.ForEach([&](Entity Entity, HealthComponent& Health, const HealthDeltaComponent& HealthDelta)
	{
		Health.CurrentHealth += HealthDelta.Delta;
	});
}

void HealthSystem::UpdatePersistentHealth(SystemContext& Context, float DeltaTime)
{
	int NewHealth = GlobalConstants::InitialPlayerHealth;
	const Query<WritesList<HealthComponent>, ReadsList<BackgroundRenderComponent>, ExcludeList<>> HealthQuery(Context.QueryContext);
	HealthQuery.ForEach([&](Entity Entity, HealthComponent& Health, const BackgroundRenderComponent& BackgroundRender)
	{
		NewHealth = Health.CurrentHealth;
	});

	const TriggerQuery Triggers(Context.QueryContext);
	Triggers.ForEach([&](Entity Entity, const PositionComponent& Position, const RectComponent& Rect, const HealthComponent& Health)
	{
		NewHealth = Health.CurrentHealth;
	});

	const Query<WritesList<>, ReadsList<SummaryTransitionComponent>, ExcludeList<>> SummaryQuery(Context.QueryContext);
	if (SummaryQuery.Size() > 0)
	{
		NewHealth = GlobalConstants::InitialPlayerHealth;
	}
	
	HealthQuery.ForEach([&](Entity Entity, HealthComponent& Health, const BackgroundRenderComponent& BackgroundRender)
	{
		Health.CurrentHealth = NewHealth;
	});
}