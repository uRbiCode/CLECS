#pragma once
#include "System.h"

struct CollisionEvent;

// Resolves CollisionEvents in a kinematic sense, so adjusts velocities and positions
class CollisionKinematicResolverSystem : public System
{
public:
	void Initialize(const SystemContext& Context) const override;

private:
	void OnCollision(const SystemContext& Context, const CollisionEvent& Event) const;
};