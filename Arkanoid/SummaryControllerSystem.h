#pragma once
#include "System.h"
#include <string>

struct ClickableUsedEvent;

/* Repsonsible for controlling the Summary game state.
 * Summary is either Victory or Defeat. From this system's perspective, they differ only by displayed text.
 */
class SummaryControllerSystem : public System
{
public:
	void Initialize(const SystemContext& Context) const override;

private:
	// Event responses.
	void OnClickableUsed(const SystemContext& Context, const ClickableUsedEvent& Event) const;

	// Summary management.
	void AddSummaryText(const SystemContext& Context, const std::string& Text) const;
	void AddMainMenuButton(const SystemContext& Context) const;
	void InitializeVictory(const SystemContext& Context) const;
	void InitializeDefeat(const SystemContext& Context) const;
	void CleanupSummary(const SystemContext& Context) const;
	void SubscribeToClickableUsedEvent(const SystemContext& Context) const;
};