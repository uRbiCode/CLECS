#pragma once

enum class GameState
{
	MainMenu,
	Run
};

struct GameStateComponent
{
	GameState CurrentState = GameState::MainMenu;
};