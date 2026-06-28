#pragma once
#include <cstdint>
#include <unordered_map>
#include <typeindex>

using ComponentTypeId = uint32_t;
using ComponentTypesMap = std::unordered_map<std::type_index, ComponentTypeId>;
struct ComponentsInitializationData;

/* Stores Component types with assigned IDs.
 * Every Component type is to be registered here. Failing to do so will result in crashes/undefined behavior.
 */
class ComponentTypesCollection
{
public:
	static ComponentTypesCollection Create(ComponentsInitializationData&& Data);

	template<typename Component>
	ComponentTypeId GetComponentTypeId() const
	{
		return ComponentTypesIds.at(typeid(Component));
	}

	size_t GetSize() const { return ComponentTypesIds.size(); }

	const ComponentTypesMap& GetRegisteredComponents() const { return ComponentTypesIds; }

private:
	ComponentTypesMap ComponentTypesIds;
};