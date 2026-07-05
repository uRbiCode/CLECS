#include "StartupSystemsInitializationData.h"

void StartupSystemsInitializationData::RegisterSystem(void(*Initialize)(SystemContext&))
{
	StartupSystemDescriptor Descriptor;
	Descriptor.Initialize = Initialize;
	Descriptors.emplace_back(std::move(Descriptor));
}

void StartupSystemsInitializationData::RegisterLateSystem(void(*Initialize)(SystemContext&))
{
	StartupSystemDescriptor Descriptor;
	Descriptor.Initialize = Initialize;
	LateDescriptors.emplace_back(std::move(Descriptor));
}

const std::vector<StartupSystemDescriptor>& StartupSystemsInitializationData::GetRegisteredSystems() const
{
	return Descriptors;
}

const std::vector<StartupSystemDescriptor>& StartupSystemsInitializationData::GetRegisteredLateSystems() const
{
	return LateDescriptors;
}