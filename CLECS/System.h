#pragma once
#include <functional>
#include <typeindex>

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
	using UpdateFunction = void(*)(const SystemContext&, float);
	using InitializeFunction = void(*)(const SystemContext&);

	InitializeFunction Initialize = nullptr;
	UpdateFunction Update = nullptr;

	std::vector<ComponentAccess> ComponentAccesses;
};