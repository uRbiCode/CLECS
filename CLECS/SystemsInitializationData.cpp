#include "SystemsInitializationData.h"

void SystemsInitializationData::RegisterSystem(void(*Update)(SystemContext&, float), SystemPhase Phase)
{
	SystemDescriptor Descriptor;
	Descriptor.Update = Update;
	Descriptor.Phase = Phase;
	Systems.emplace_back(std::move(Descriptor));
}

std::vector<SystemDescriptor>& SystemsInitializationData::AccessRegisteredSystems()
{
	return Systems;
}