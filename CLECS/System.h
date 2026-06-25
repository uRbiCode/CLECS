#pragma once
#include "SystemPhases.h"
#include <functional>

struct SystemContext;

/* System is a fundamental concept in CLECS architecture.
 * It represents a piece of logic that operates on entities that have specific components attached to them.
 * Systems are stateless, and they do not store any data themselves. Instead, they operate on the data stored in components.
 */
struct SystemDescriptor
{
	using UpdateFunction = std::function<void(const SystemContext&, float)>;

	UpdateFunction Update;
	SystemPhase Phase = SystemPhase::Update;
};

struct StartupSystemDescriptor
{
	using InitializeFunction = std::function<void(const SystemContext&)>;

	InitializeFunction Initialize;
};