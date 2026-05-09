#pragma once

struct SystemContext;

/* Tracks top-level game state. Relies on present GameStateComponent.
 * Performs state transitions via sending appropriate events.
 * Also adds background render entity to the world, as upon system initialization Textures are loaded.
 */
namespace GameStateSystem
{
	void Initialize(const SystemContext& Context);
}