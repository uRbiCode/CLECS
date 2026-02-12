#pragma once
#include <cstdint>

enum class GameState : uint8_t
{
	Invalid = 0,
	MainMenu,
	Run,
	Victory,
	Defeat
};

struct GameStateComponent
{
	GameState CurrentState = GameState::Invalid;
};