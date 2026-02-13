#pragma once
#include "GameStateComponent.h"

/* Sent at the beginning of each GameState.
 * Used by Systems to determine if they shoould initialize state controlled by them.
 */
struct GameStateBeginEvent
{
	GameState BeginningState = GameState::Invalid;
};

/* Sent at the end of each GameState.
 * Used by Systems to determine if they shoould clean up state controlled by them.
 */
struct GameStateEndEvent
{
	GameState EndingState = GameState::Invalid;
};

/* Sent before switching to a new GameState.
 * GameStateSystem listens subscribes to it and performs the change.
 */
struct RequestGameStateChangeEvent
{
	GameState NewState = GameState::Invalid;
};