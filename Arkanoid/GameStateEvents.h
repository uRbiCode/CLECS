#pragma once
#include "GameStateComponent.h"

struct GameStateBeginEvent
{
	GameState BeginningState = GameState::MainMenu;
};

struct GameStateEndedEvent
{
	GameState EndingState = GameState::MainMenu;
};