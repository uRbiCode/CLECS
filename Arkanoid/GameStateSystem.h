#pragma once
#include "System.h"

enum class GameState;

// Tracks top-level game state. Relies on present GameStateComponent
class GameStateSystem : public System
{
public:
	void Initialize(const SystemContext& Context) const override;

public:
	void InitializeCurrentState(const SystemContext& Context) const;
	void EndCurrentState(const SystemContext& Context) const;
	void ChangeGameState(const SystemContext& Context, GameState NewState) const;
	void AddBackgroundRenderEntity(const SystemContext& Context) const;
};