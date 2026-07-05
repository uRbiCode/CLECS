#pragma once
#include "SystemPhases.h"
#include <functional>

struct SystemContext;

#pragma warning(push)
#pragma warning(disable : 4820)
/* System is a fundamental concept in CLECS architecture.
 * It represents a piece of logic that operates on entities that have specific components attached to them.
 * Systems are stateless, and they do not store any data themselves. Instead, they operate on the data stored in components.
 */
struct SystemDescriptor
{
	using UpdateFunction = std::function<void(SystemContext&, float)>;

	UpdateFunction Update;
	SystemPhase Phase = SystemPhase::Update;
};
#pragma warning(pop)

struct StartupSystemDescriptor
{
	using InitializeFunction = std::function<void(SystemContext&)>;

	InitializeFunction Initialize;
};