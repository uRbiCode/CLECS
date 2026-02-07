#pragma once
#include "CoreTypes.h"
#include "Component.h"
#include <vector>

/* Entity is a fundamental concept in CLECS architecture. It represents a unique identifier that can have multiple components attached to it.
 * Entity itself does not contain any data or behavior. It is simply a way to group components together.
 */
struct Entity
{
	int EntityId = INVALID_ID;
};
