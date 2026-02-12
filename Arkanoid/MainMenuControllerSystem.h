#pragma once
#include "System.h"

struct ClickableUsedEvent;

// Controls MainMenu GameState
class MainMenuControllerSystem : public System
{
public:
	void Initialize(const SystemContext& Context) const override;

private:
	void InitializeMainMenu(const SystemContext& Context) const;
	void AddTitleText(const SystemContext& Context) const;
	void AddButtons(const SystemContext& Context) const;
	void CleanupMainMenu(const SystemContext& Context) const;
	void OnClickableUsed(const SystemContext& Context, const ClickableUsedEvent& Event) const;
	void QuitGame() const;
};