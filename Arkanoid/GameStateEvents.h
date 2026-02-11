#pragma once
#include "GameStateComponent.h"

struct GameStateBeginEvent
{
	GameState BeginningState = GameState::Invalid;
};

struct GameStateEndEvent
{
	GameState EndingState = GameState::Invalid;
};

struct RequestGameStateChangeEvent
{
	GameState NewState = GameState::Invalid;
};