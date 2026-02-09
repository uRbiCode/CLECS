#pragma once
#include "System.h"
#include "MathTypes.h"

struct TransformComponent;
struct ShapeComponent;

class CollisionSystem : public System
{
public:
	void Update(const SystemContext& Context, float DeltaTime) const override;
};