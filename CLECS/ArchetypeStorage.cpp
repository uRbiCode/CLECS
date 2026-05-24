#include "ArchetypeStorage.h"

ArchetypeStorage ArchetypeStorage::Create(ComponentTypesCollection&& Data)
{
	ArchetypeStorage Storage = ArchetypeStorage();
	Storage.ComponentTypes = std::move(Data);
	Storage.ComponentsToArchetypes.reserve(Storage.ComponentTypes.GetSize());
	for (const auto& [ComponentType, ComponentTypeId] : Storage.ComponentTypes.GetRegisteredComponents())
	{
		Storage.ComponentsToArchetypes.insert({ ComponentTypeId, {} });
	}
	return Storage;
}