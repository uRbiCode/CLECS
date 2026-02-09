#pragma once
#include "System.h"

// Detects whether two entities with CollisionComponents are colliding and publishes CollisionEvents
class CollisionDetectionSystem : public System
{
public:
	void Update(const SystemContext& Context, float DeltaTime) const override;
};