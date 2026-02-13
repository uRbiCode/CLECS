#pragma once
#include "System.h"

struct ClickableUsedEvent;

/* Repsonsible for controlling the Tutorial game state.
 * Works similarly to SummaryControllerSystem, but manages a different state and differs slightly within initialization and cleanup logic.
 */
class TutorialControllerSystem : public System
{
public:
	void Initialize(const SystemContext& Context) const override;

private:
	// Event responses.
	void OnClickableUsed(const SystemContext& Context, const ClickableUsedEvent& Event) const;

	// Tutorial management.
	void AddTutorialText(const SystemContext& Context) const;
	void AddMainMenuButton(const SystemContext& Context) const;
	void CleanupTutorial(const SystemContext& Context) const;
	void SubscribeToClickableUsedEvent(const SystemContext& Context) const;
};