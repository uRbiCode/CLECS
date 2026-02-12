#pragma once
#include "System.h"

struct ClickableUsedEvent;

// Manages How to play tutorial
class TutorialControllerSystem : public System
{
public:
	void Initialize(const SystemContext& Context) const override;

private:
	void AddTutorialText(const SystemContext& Context) const;
	void AddMainMenuButton(const SystemContext& Context) const;
	void CleanupTutorial(const SystemContext& Context) const;
	void OnClickableUsed(const SystemContext& Context, const ClickableUsedEvent& Event) const;
	void SubscribeToClickableUsedEvent(const SystemContext& Context) const;
};