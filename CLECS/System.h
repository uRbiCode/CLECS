#pragma once
#include "CoreTypes.h"

/* System is a fundamental concept in CLECS architecture. It represents a piece of logic that operates on entities that have specific components attached to them.
 * System itself is stateless. It is simply a way to define a set of operations that can be performed on entities that match a certain criteria.
 */
class System
{
public:
	virtual void Update(float deltaTime) = 0;
};
