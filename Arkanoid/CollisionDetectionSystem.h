#pragma once
#include "System.h"
#include "MathTypes.h"

struct TransformComponent;
struct ShapeComponent;

class CollisionDetectionSystem : public System
{
public:
	void Update(const SystemContext& Context, float DeltaTime) const override;
};