#pragma once
#include "SystemQuery.h"
#include "VelocityComponent.h"
#include "TransformComponent.h"

/* Responsible for updating the movement of entities.
 * So basically applies velocity to transform.
 */
namespace MovementSystem
{
	void Update(SystemQuery<Writes<TransformComponent>, Reads<VelocityComponent>>& Query, const SystemContext& Context, float DeltaTime);
}