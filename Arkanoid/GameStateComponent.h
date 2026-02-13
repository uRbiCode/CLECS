#pragma once
#include <cstdint>

enum class GameState : uint8_t
{
	Invalid = 0,
	MainMenu,
	Tutorial,
	Run,
	Victory,
	Defeat
};

/* Stores high-level state of the game.
 * Used by Systems to determine if they should initialize state which they control. 
 */
struct GameStateComponent
{
	GameState CurrentState = GameState::Invalid;
};