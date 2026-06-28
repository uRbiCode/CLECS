#pragma once
#include "System.h"
#include <vector>

/* Part of WorldInitializationData.
 * Allows Modules to provide Systems to be registered.
 */
struct SystemsInitializationData
{
	void RegisterSystem(void(*Update)(SystemContext&, float), SystemPhase Phase)
	{
		SystemDescriptor Descriptor;
		Descriptor.Update = Update;
		Descriptor.Phase = Phase;
		Systems.emplace_back(std::move(Descriptor));
	}

	std::vector<SystemDescriptor>& AccessRegisteredSystems() { return Systems; }

private:
	std::vector<SystemDescriptor> Systems;
};