#pragma once
#include "System.h"
#include <vector>

/* Part of WorldInitializationData.
 * Allows Modules to provide Startup Systems to be registered.
 * Startup Systems are executed only once. 
 * Their goal should be to provide necessary setup for the main Systems within the same Module.
 */
struct StartupSystemsInitializationData
{
	void RegisterSystem(void(*Initialize)(const SystemContext&))
	{
		StartupSystemDescriptor Descriptor;
		Descriptor.Initialize = Initialize;
		Descriptors.emplace_back(std::move(Descriptor));
	}

	const std::vector<StartupSystemDescriptor>& GetRegisteredSystems() const { return Descriptors; }

private:
	std::vector<StartupSystemDescriptor> Descriptors;
};