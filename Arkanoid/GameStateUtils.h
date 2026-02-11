#pragma once

struct SystemContext;
enum class GameState;

namespace GameStateUtils
{
	void RequestStateChange(const SystemContext& Context, GameState NewState);
	GameState GetCurrentGameState(const SystemContext& Context);
};