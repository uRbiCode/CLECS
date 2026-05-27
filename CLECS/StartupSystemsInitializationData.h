#pragma once
#include "SystemContext.h"
#include "System.h"
#include <vector>

/* Part of WorldInitializationData.
 * Allows Modules to provide Startup Systems to be registered.
 * Startup Systems are exeucted only once. 
 * Their goal should be to provide necessary setup for the main Systems within the same Module.
 */
struct StartupSystemsInitializationData
{
	// TODO: REMOVE AFTER GAMEPLAY OVERHAUL
	void RegisterSystem(void(*Initialize)(SystemContext&))
	{
		SystemDescriptor Descriptor;
		Descriptor.Initialize = Initialize;
		Descriptors.emplace_back(std::move(Descriptor));
	}

	const std::vector<SystemDescriptor>& GetRegisteredSystems() const { return Descriptors; }

private:
	std::vector<SystemDescriptor> Descriptors;
};