#pragma once
#include "System.h"

/* Responsible for updating the movement of entities.
 * So basically applies velocity to transform.
 */
class MovementSystem : public System
{
public:
	void Update(const SystemContext& Context, float DeltaTime) const override;
};