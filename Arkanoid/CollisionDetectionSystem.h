#pragma once

/* Detects whether two entities with CollisionComponents are colliding and publishes CollisionEvents.
 * Uses AABB and rect-circle checks.
 */
namespace CollisionDetectionSystem
{
	void Update(const SystemContext& Context, float DeltaTime);
}