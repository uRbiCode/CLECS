#pragma once
#include <System.h>

struct CollisionEvent;

class CollisionResolverSystem : public System
{
public:
	void Initialize(const SystemContext& Context) const override;

private:
	void ResolveCollision(const SystemContext& Context, const CollisionEvent& Event) const;
};