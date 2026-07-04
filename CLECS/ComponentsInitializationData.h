#pragma once
#include <unordered_set>
#include <typeindex>

/* Part of WorldInitializationData.
 * Allows Modules to provide Component types to be registered.
 */
struct ComponentsInitializationData
{
	template<typename Component>
	void RegisterComponent()
	{
		Components.emplace(typeid(Component));
	}

	const std::unordered_set<std::type_index>& GetRegisteredComponents() const { return Components; }

private:
	std::unordered_set<std::type_index> Components;
};