#pragma once
#include "System.h"
#include "MathTypes.h"

struct TransformComponent;
struct ShapeComponent;

class CollisionSystem : public System
{
public:
	void Update(const SystemUpdateContext& UpdateContext, float DeltaTime) override;
};