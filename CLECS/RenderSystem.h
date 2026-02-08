#pragma once
#include "System.h"

// RenderSystem renders all entities with Transform and Shape components
class RenderSystem : public System
{
public:
	void Update(const SystemUpdateContext& UpdateContext) override;
};