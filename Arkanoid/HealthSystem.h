#pragma once
#include "System.h"

struct CollisionEvent;

// Manages HealthComponents
class HealthSystem : public System
{
public:
	void Initialize(const SystemContext& Context) const override;
	void Update(const SystemContext& Context, float DeltaTime) const override;

private:
	void OnCollision(const SystemContext& Context, const CollisionEvent& Event) const;
};