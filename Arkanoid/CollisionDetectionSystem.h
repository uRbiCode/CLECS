#pragma once
#include "SystemQuery.h"
#include "TransformComponent.h"
#include "CollisionComponent.h"


/* Detects whether two entities with CollisionComponents are colliding and publishes CollisionEvents.
 * Uses AABB and rect-circle checks.
 */
namespace CollisionDetectionSystem
{
	void Update(SystemQuery<Writes<>, Reads<TransformComponent, CollisionComponent>>& Query, const SystemContext& Context, float DeltaTime);
}