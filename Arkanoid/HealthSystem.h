#pragma once
#include "System.h"

struct CollisionEvent;
struct Entity;

/* Responsible for performing health changes. 
 * It's update method is the place where entities with <= 0 health are destroyed.
 */
// 
class HealthSystem : public System
{
public:
	void Initialize(const SystemContext& Context) const override;
	void Update(const SystemContext& Context, float DeltaTime) const override;

private:
	// Event responses
	void OnCollision(const SystemContext& Context, const CollisionEvent& Event) const;

	// Health management
	bool WasTriggerHit(const SystemContext& Context, const Entity& Entity) const;
	void HandleCollision(const SystemContext& Context, const Entity& Entity) const;
	void ResolveTriggerHit(const SystemContext& Context) const;
	void DealDamage(const SystemContext& Context, const Entity& Entity, int Damage) const;
};