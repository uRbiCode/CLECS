#pragma once
#include "System.h"

struct HealthChangedEvent;

// Manages currently ongoing stage
class CurrentStageSystem : public System
{
public:
	void Initialize(const SystemContext& Context) const override;

private:
	void OnHealthChanged(const SystemContext& Context, const HealthChangedEvent& Event) const;
	void ResetStage(const SystemContext& Context) const;
};