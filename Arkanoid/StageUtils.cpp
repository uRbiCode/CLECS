#include "StageUtils.h"
#include "RunStateComponent.h"
#include "EntityAdmin.h"
#include "SystemContext.h"
#include "StageDataComponent.h"
#include <optional>
#include "HealthComponent.h"
#include "CollisionComponent.h"
#include <cassert>

StageData StageUtils::GetCurrentStageData(const SystemContext& Context)
{
	const auto StageDataGroup = Context.EntityAdmin.GetGroup<StageDataComponent>();
	assert(StageDataGroup.Size() == 1 && "Expected exactly one StageDataComponent in the world");

	if (StageDataGroup.Empty())
		return {};

	return Context.EntityAdmin.GetComponent<StageDataComponent>(StageDataGroup[0]).CurrentStageData;
}

int StageUtils::GetCurrentPlayerHealth(const SystemContext& Context)
{
	std::optional<int> Health = std::nullopt;
	Context.EntityAdmin.GetGroup<CollisionComponent, HealthComponent>().ForEach([&Context, &Health](const Entity& Entity, const CollisionComponent& Collision, const HealthComponent& HealthComp)
	{
		if (Collision.Channel == CollisionChannel::Trigger)
		{
			Health = HealthComp.CurrentHealth;
		}
	});

	if (!Health.has_value())
	{
		SDL_LogError(SDL_LOG_CATEGORY_ASSERT, "StageUtils::GetCurrentPlayerHealth -> Trigger entity with HealthComponent is missing");
	}

	return Health.value_or(0);
}
