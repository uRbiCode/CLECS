#pragma once
#include "System.h"
#include <string>

struct ClickableUsedEvent;

class SummaryControllerSystem : public System
{
public:
	void Initialize(const SystemContext& Context) const override;

private:
	void AddSummaryText(const SystemContext& Context, const std::string& Text) const;
	void AddMainMenuButton(const SystemContext& Context) const;
	void InitializeVictory(const SystemContext& Context) const;
	void InitializeDefeat(const SystemContext& Context) const;
	void CleanupSummary(const SystemContext& Context) const;
	void OnClickableUsed(const SystemContext& Context, const ClickableUsedEvent& Event) const;
};