#pragma once
#include "System.h"

class PlayerInputSystem : public System
{
public:
	void Update(const SystemUpdateContext& UpdateContext, float DeltaTime) override;
};