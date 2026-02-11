#pragma once
#include "System.h"

struct HealthChangedEvent;
struct StageBeginEvent;

// Event-driven controller for displaying remaining player lives
class HealthIndicatorSystem : public System
{
public:
	void Initialize(const SystemContext& Context) const override;

private:
	void OnStageBegin(const SystemContext& Context, const StageBeginEvent& Event) const;
	void AddHealthIndicators(const SystemContext& Context, int Count) const;
	void RemoveHealthIndicators(const SystemContext& Context, int Count) const;
	void OnHealthChanged(const SystemContext& Context, const HealthChangedEvent& Event) const;
};