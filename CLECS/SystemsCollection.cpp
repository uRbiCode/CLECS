#include "SystemsCollection.h"
#include "SystemsInitializationData.h"

SystemsCollection SystemsCollection::Create(SystemsInitializationData&& Data)
{
	SystemsCollection Systems;
	for (SystemDescriptor& Descriptor : Data.AccessRegisteredSystems())
	{
		Systems.Systems[Descriptor.Phase].emplace_back(std::move(Descriptor.Update));
	}
	return Systems;
}