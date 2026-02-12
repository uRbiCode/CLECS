#pragma once
#include <cstdint>

struct SystemContext;
enum class GameState : uint8_t;

namespace GameStateUtils
{
	void RequestStateChange(const SystemContext& Context, GameState NewState);
	GameState GetCurrentGameState(const SystemContext& Context);
};