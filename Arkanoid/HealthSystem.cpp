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
#include "TextureComponent.h"

void HealthSystem::UpdateDisplayedHealth(SystemContext& Context, [[maybe_unused]] float DeltaTime)
{
	int TotalHealthChange = 0;

	const Query<WritesList<>, ReadsList<HealthDeltaComponent>, ExcludeList<BackgroundRenderComponent, GameRenderComponent>> HealthQuery(Context.QueryContext);
	HealthQuery.ForEach([&]([[maybe_unused]] Entity Entity, const HealthDeltaComponent& HealthDelta)
	{
		TotalHealthChange += HealthDelta.Delta;
	});

	if (TotalHealthChange >= 0)
		return;

	const Query<WritesList<>, ReadsList<PositionComponent, RectComponent, UIRenderComponent, TextureComponent>, ExcludeList<BackgroundRenderComponent, GameRenderComponent>> HealthIndicatorQuery(Context.QueryContext);
	std::vector<std::pair<Entity, float>> HealthIndicatorsToRemove;
	HealthIndicatorsToRemove.reserve(HealthIndicatorQuery.Size());
	HealthIndicatorQuery.ForEach([&](Entity Entity, const PositionComponent& Position, [[maybe_unused]] const RectComponent& Rect, [[maybe_unused]] const UIRenderComponent& Render, [[maybe_unused]] const TextureComponent& Texture)
	{
		HealthIndicatorsToRemove.push_back({Entity, Position.Position.X});
	});

	std::ranges::sort(HealthIndicatorsToRemove, [](const std::pair<Entity, float>& A, const std::pair<Entity, float>& B) 
	{
		return A.second > B.second; 
	});

	TotalHealthChange = std::abs(TotalHealthChange);
	RemoveEntitiesCommand RemoveHealthIndicatorsCommand(TotalHealthChange);
	for (int i = 0; i < TotalHealthChange; ++i)
	{
		RemoveHealthIndicatorsCommand.WithEntry(HealthIndicatorsToRemove[i].first);
	}

	Context.Commands.Submit(std::move(RemoveHealthIndicatorsCommand));
}

void HealthSystem::RemoveDeadEntities(SystemContext& Context, [[maybe_unused]] float DeltaTime)
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

void HealthSystem::CleanupHealthDeltaComponents(SystemContext& Context, [[maybe_unused]] float DeltaTime)
{
	ComponentUtils::RemoveAllComponentsTyped<HealthDeltaComponent>(Context);
}

void HealthSystem::ApplyHealthChanges(SystemContext& Context, [[maybe_unused]] float DeltaTime)
{
	const Query<WritesList<HealthComponent>, ReadsList<HealthDeltaComponent>, ExcludeList<>> HealthQuery(Context.QueryContext);
	HealthQuery.ForEach([&]([[maybe_unused]] Entity Entity, HealthComponent& Health, const HealthDeltaComponent& HealthDelta)
	{
		Health.CurrentHealth += HealthDelta.Delta;
	});
}

void HealthSystem::UpdatePersistentHealth(SystemContext& Context, [[maybe_unused]] float DeltaTime)
{
	int NewHealth = GlobalConstants::InitialPlayerHealth;
	const Query<WritesList<HealthComponent>, ReadsList<BackgroundRenderComponent>, ExcludeList<>> HealthQuery(Context.QueryContext);
	HealthQuery.ForEach([&]([[maybe_unused]] Entity Entity, HealthComponent& Health, [[maybe_unused]] const BackgroundRenderComponent& BackgroundRender)
	{
		NewHealth = Health.CurrentHealth;
	});

	const TriggerQuery Triggers(Context.QueryContext);
	Triggers.ForEach([&]([[maybe_unused]] Entity Entity, [[maybe_unused]] const PositionComponent& Position, [[maybe_unused]] const RectComponent& Rect, const HealthComponent& Health)
	{
		NewHealth = Health.CurrentHealth;
	});

	const Query<WritesList<>, ReadsList<SummaryTransitionComponent>, ExcludeList<>> SummaryQuery(Context.QueryContext);
	if (SummaryQuery.Size() > 0)
	{
		NewHealth = GlobalConstants::InitialPlayerHealth;
	}
	
	HealthQuery.ForEach([&]([[maybe_unused]] Entity Entity, HealthComponent& Health, [[maybe_unused]] const BackgroundRenderComponent& BackgroundRender)
	{
		Health.CurrentHealth = NewHealth;
	});
}