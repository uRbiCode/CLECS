#pragma once
#include "System.h"

class PlayerInputSystem : public System
{
public:
	void Update(const SystemContext& Context, float DeltaTime) const override;
};