#pragma once
#include "System.h"

class MovementSystem : public System
{
public:
	void Update(const SystemContext& Context, float DeltaTime) const override;
};