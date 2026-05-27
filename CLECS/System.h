#pragma once
#include <functional>
#include <typeindex>
#include <vector>

struct SystemContext;

enum class ComponentAccessMode 
{ 
	Read,
	Write 
};

struct ComponentAccess
{
	std::type_index ComponentType;
	ComponentAccessMode AccessMode;
};

/* System is a fundamental concept in CLECS architecture.
 * It represents a piece of logic that operates on entities that have specific components attached to them.
 * Systems are stateless, and they do not store any data themselves. Instead, they operate on the data stored in components.
 */
struct SystemDescriptor
{
	using UpdateFunction = std::function<void(SystemContext&, float)>;
	using InitializeFunction = std::function<void(SystemContext&)>;

	InitializeFunction Initialize;
	UpdateFunction Update;
	std::vector<ComponentAccess> ComponentAccesses;
};