#pragma once
#include "System.h"
#include <cstdint>

enum class GameState : uint8_t;

/* Tracks top-level game state. Relies on present GameStateComponent.
 * Performs state transisitions via sending appropirate events.
 * Also adds background render entity to the world, as upon system initialization Textures are loaded.
 */
class GameStateSystem : public System
{
public:
	void Initialize(const SystemContext& Context) const override;

public:
	// GameState management.
	void InitializeCurrentState(const SystemContext& Context) const;
	void EndCurrentState(const SystemContext& Context) const;
	void ChangeGameState(const SystemContext& Context, GameState NewState) const;

	// Background.
	void AddBackgroundRenderEntity(const SystemContext& Context) const;
};