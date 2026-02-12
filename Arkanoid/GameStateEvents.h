#pragma once
#include "GameStateComponent.h"

// Sent at the beginning of a GameState
struct GameStateBeginEvent
{
	GameState BeginningState = GameState::Invalid;
};

// Sent at the end of a GameState
struct GameStateEndEvent
{
	GameState EndingState = GameState::Invalid;
};

// Sent to notify that GameState should be changed
struct RequestGameStateChangeEvent
{
	GameState NewState = GameState::Invalid;
};