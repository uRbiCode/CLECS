#pragma once
#include "CoreTypes.h"

class SystemUpdateContext;

/* System is a fundamental concept in CLECS architecture. 
 * It represents a piece of logic that operates on entities that have specific components attached to them.
 */
class System
{
public:
	virtual ~System() = default;

	virtual void Update(const SystemUpdateContext& UpdateContext, float DeltaTime) = 0;
};

template<class T>
concept SystemType = std::derived_from<T, System>;