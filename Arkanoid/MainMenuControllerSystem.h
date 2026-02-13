#pragma once
#include "System.h"

struct ClickableUsedEvent;

/* Responsible for controlling the MainMenu game state.
 * Adds UI elements and listens to clicks on the buttons.
 * Can exit the game too!
 */
class MainMenuControllerSystem : public System
{
public:
	void Initialize(const SystemContext& Context) const override;

private:
	// Event responses
	void OnClickableUsed(const SystemContext& Context, const ClickableUsedEvent& Event) const;

	// MainMenu management
	void InitializeMainMenu(const SystemContext& Context) const;
	void AddTitleText(const SystemContext& Context) const;
	void AddButtons(const SystemContext& Context) const;
	void CleanupMainMenu(const SystemContext& Context) const;
	void QuitGame() const;
};