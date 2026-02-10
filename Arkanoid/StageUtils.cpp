#include "StageUtils.h"
#include "RunStateComponent.h"
#include "EntityAdmin.h"
#include "SystemContext.h"
#include "StageDataComponent.h"
#include <optional>

StageData StageUtils::GetCurrentStageData(const SystemContext& Context)
{
	StageData Data;

	std::optional<int> CurrentStageId = 0;

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
