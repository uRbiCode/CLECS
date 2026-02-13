#pragma once
#include "System.h"
#include "MathTypes.h"

struct CollisionEvent;
class EntityAdmin;
struct Entity;

/* Resolves CollisionEvents in a kinematic sense, so adjusts velocities and positions.
 * Handles collisions between rects and circles.
 */
class CollisionKinematicResolverSystem : public System
{
public:
	void Initialize(const SystemContext& Context) const override;

private:
	// Event reaction.
	void OnCollision(const SystemContext& Context, const CollisionEvent& Event) const;

	// Collision resolves.
	void ResolveRegularCollision(EntityAdmin& Admin, const Entity& EntityToMove, const Vector2D<float>& Separation) const;
	void ResolveCollisionWithDirection(EntityAdmin& Admin, const Entity& Ball, const Entity& Player, const Vector2D<float>& Separation) const;
};