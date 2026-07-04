#pragma once

struct SystemContext;

/* Detects collision between entities.
 * Recognizes Paddle, Ball, Bricks, Trigger, and Wall entities.
 */
namespace CollisionSystem
{
	void CleanupCollisionComponents(SystemContext& Context, float DeltaTime);
	void UpdateBallCollision(SystemContext& Context, float DeltaTime);
	void UpdatePaddleCollision(SystemContext& Context, float DeltaTime);
	void UpdateBallVelocity(SystemContext& Context, float DeltaTime);
}