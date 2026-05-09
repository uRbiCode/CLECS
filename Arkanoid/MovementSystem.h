#pragma once

struct SystemContext;

/* Responsible for updating the movement of entities.
 * So basically applies velocity to transform.
 */
namespace MovementSystem
{
	void Update(const SystemContext& Context, float DeltaTime);
}