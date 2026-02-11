#pragma once

enum class GameState
{
	Invalid,
	MainMenu,
	Run
};

struct GameStateComponent
{
	GameState CurrentState = GameState::Invalid;
};