#pragma once
#include <concepts>

struct SystemContext;

/* System is a fundamental concept in CLECS architecture. 
 * It represents a piece of logic that operates on entities that have specific components attached to them.
 * Systems are to be stateless, and they should not store any data themselves. Instead, they should operate on the data stored in components.
 */
class System
{
public:
	virtual ~System() = default;

	virtual void Initialize(const SystemContext& Context) const {}
	virtual void Update(const SystemContext& Context, float DeltaTime) const {}
};

template<class T>
concept SystemType = std::derived_from<T, System>;