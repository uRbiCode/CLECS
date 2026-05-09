#pragma once

struct SystemContext;

/* Responsible for controlling the MainMenu game state.
 * Adds UI elements and listens to clicks on the buttons.
 * Can exit the game too!
 */
namespace MainMenuControllerSystem
{
	void Initialize(const SystemContext& Context);
}