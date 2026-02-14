#include "GameStateUtils.h"
#include "GameStateEvents.h"
#include "GameStateComponent.h"
#include "SystemContext.h"
#include "EventBus.h"
#include "EntityAdmin.h"
#include <cassert>

void GameStateUtils::RequestStateChange(const SystemContext& Context, GameState NewState)
{
	Context.EventBus.Notify(Context, RequestGameStateChangeEvent{ NewState });
}

GameState GameStateUtils::GetCurrentGameState(const SystemContext& Context)
{
	const auto GameStateGroup = Context.EntityAdmin.GetGroup<GameStateComponent>();
	assert(GameStateGroup.Size() == 1 && "Expected exactly one GameStateComponent in the world");

	if (GameStateGroup.Empty())
		return GameState::Invalid;

	return GameStateGroup.Get<GameStateComponent>(GameStateGroup[0]).CurrentState;
}