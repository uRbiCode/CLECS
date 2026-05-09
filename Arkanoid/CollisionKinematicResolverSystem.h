#pragma once

struct SystemContext;

/* Resolves CollisionEvents in a kinematic sense, so adjusts velocities and positions.
 * Handles collisions between rects and circles.
 */
namespace CollisionKinematicResolverSystem
{
	void Initialize(const SystemContext& Context);
}