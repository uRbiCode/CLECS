#pragma once
#include "System.h"

struct HealthChangedEvent;

// Event-driven controller for displaying remaining player lives
class HealthIndicatorSystem : public System
{
public:
	void Initialize(const SystemContext& Context) const override;

private:
	void OnRunBegin(const SystemContext& Context) const;
	void AddHealthIndicators(const SystemContext& Context, int Count) const;
	void RemoveHealthIndicators(const SystemContext& Context, int Count) const;
	void CleanupHealthIndicators(const SystemContext& Context) const;
	void OnHealthChanged(const SystemContext& Context, const HealthChangedEvent& Event) const;
};