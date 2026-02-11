#include "StageUtils.h"
#include "RunStateComponent.h"
#include "EntityAdmin.h"
#include "SystemContext.h"
#include "StageDataComponent.h"
#include <optional>
#include "HealthComponent.h"
#include "CollisionComponent.h"

StageData StageUtils::GetCurrentStageData(const SystemContext& Context)
{
	StageData Data;

	std::optional<int> CurrentStageId = std::nullopt;

	auto& Admin = Context.EntityAdmin;
	Admin.GetGroup<RunStateComponent>().ForEach([&Context, &CurrentStageId](const Entity& StageEntity, const RunStateComponent& RunState)
	{
		CurrentStageId = RunState.CurrentStage;
	});

	if (!CurrentStageId.has_value())
	{
		SDL_LogError(SDL_LOG_CATEGORY_ASSERT, "StageUtils::GetCurrentStageData -> RunStateComponent is missing");
		return Data;
	}

	Admin.GetGroup<StageDataComponent>().ForEach([&Context, &Data, StageId = CurrentStageId.value()](const Entity& StageEntity, const StageDataComponent& StageDataComponent)
	{
		if (StageId >= static_cast<int>(StageDataComponent.Stages.size()))
		{
			SDL_LogError(SDL_LOG_CATEGORY_ASSERT, "StageUtils::GetCurrentStageData -> Invalid stage index %d", StageId);
			return;
		}

		Data = StageDataComponent.Stages[StageId];
	});

	return Data;
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
