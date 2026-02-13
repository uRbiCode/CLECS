#pragma once
#include "System.h"

struct HealthChangedEvent;

/* Responsible for displaying remaining player lives. These hearts at the bottom left.
 * Reacts to events if to display the health or not and updates number of hearts.
 */
class HealthIndicatorSystem : public System
{
public:
	void Initialize(const SystemContext& Context) const override;

private:
	// Event responses
	void OnRunBegin(const SystemContext& Context) const;
	void OnHealthChanged(const SystemContext& Context, const HealthChangedEvent& Event) const;

	// Health indicator management
	void AddHealthIndicators(const SystemContext& Context, int Count) const;
	void RemoveHealthIndicators(const SystemContext& Context, int Count) const;
	void CleanupHealthIndicators(const SystemContext& Context) const;
};