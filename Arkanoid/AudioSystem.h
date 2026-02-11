#pragma once
#include "System.h"

struct CollisionEvent;

// Simple audio system that can be used to play sound
class AudioSystem : public System
{
public:
	void Initialize(const SystemContext& Context) const override;

private:
	void OnCollision(const SystemContext& Context, const CollisionEvent& Event) const;
};