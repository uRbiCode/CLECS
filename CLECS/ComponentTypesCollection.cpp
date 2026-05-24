#include "ComponentTypesCollection.h"
#include "ComponentsInitializationData.h"

ComponentTypesCollection ComponentTypesCollection::Create(const ComponentsInitializationData& Data)
{
	ComponentTypesCollection Collection;
	Collection.ComponentTypesIds.clear();
	Collection.ComponentTypesIds.reserve(Data.GetRegisteredComponents().size());

	ComponentTypeId NextComponentTypeId = 0;
	for (const std::type_index ComponentType : Data.GetRegisteredComponents())
	{
		Collection.ComponentTypesIds.insert({ ComponentType, NextComponentTypeId++ });
	}
	return Collection;
}