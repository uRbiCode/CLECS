#pragma once
#include <concepts>

struct SystemContext;

/* System is a fundamental concept in CLECS architecture. 
 * It represents a piece of logic that operates on entities that have specific components attached to them.
 */
class System
{
public:
	virtual ~System() = default;

	virtual void Initialize(const SystemContext& Context) const {}
	virtual void Update(const SystemContext& Context, float DeltaTime) const = 0;
};

template<class T>
concept SystemType = std::derived_from<T, System>;