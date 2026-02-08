#pragma once
#include "System.h"

class MovementSystem : public System
{
public:
	void Update(const SystemUpdateContext& UpdateContext, float DeltaTime) override;
};