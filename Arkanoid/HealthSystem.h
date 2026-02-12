#pragma once
#include "System.h"

struct CollisionEvent;
struct Entity;

// Manages HealthComponents
class HealthSystem : public System
{
public:
	void Initialize(const SystemContext& Context) const override;
	void Update(const SystemContext& Context, float DeltaTime) const override;

private:
	bool WasTriggerHit(const SystemContext& Context, const Entity& Entity) const;
	void OnCollision(const SystemContext& Context, const CollisionEvent& Event) const;
	void HandleCollision(const SystemContext& Context, const Entity& Entity) const;
	void ResolveTriggerHit(const SystemContext& Context) const;
	void DealDamage(const SystemContext& Context, const Entity& Entity, int Damage) const;
};