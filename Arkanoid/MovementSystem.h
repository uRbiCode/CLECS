#pragma once

struct SystemContext;

/* Responsible for updating the movement of entities.
 * So basically applies velocity to position.
 */
namespace MovementSystem
{
	void ResolveMovementChanges(SystemContext& Context, float DeltaTime);
	void ResolveMovement(SystemContext& Context, float DeltaTime);
}