#pragma once
#include "System.h"

struct SDL_FRect;
struct TransformComponent;
struct RectComponent;
struct CircleComponent;

/* Detects whether two entities with CollisionComponents are colliding and publishes CollisionEvents.
 * Uses AABB and rect-circle checks.
 */
class CollisionDetectionSystem : public System
{
public:
	void Update(const SystemContext& Context, float DeltaTime) const override;

private:
	bool CheckAABB(const SDL_FRect& A, const SDL_FRect& B) const;
	bool CheckCircleRect(const TransformComponent& CircleTransform, const CircleComponent& Circle, const TransformComponent& RectTransform, const RectComponent& Rect) const;
	bool CheckCircleCircle(const TransformComponent& TransformA, const CircleComponent& CircleA, const TransformComponent& TransformB, const CircleComponent& CircleB) const;
};