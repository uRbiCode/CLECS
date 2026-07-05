#pragma once
#include "System.h"
#include <vector>

/* Part of WorldInitializationData.
 * Allows Modules to provide Startup Systems to be registered.
 * Startup Systems are executed only once. 
 * Their goal should be to provide necessary setup for the main Systems within the same Module.
 * For more complicated startup sequences, one can use LateSystems. They are executed after regular ones and CommandsFlush.
 */
struct StartupSystemsInitializationData
{
	void RegisterSystem(void(*Initialize)(SystemContext&));

	void RegisterLateSystem(void(*Initialize)(SystemContext&));

	const std::vector<StartupSystemDescriptor>& GetRegisteredSystems() const;
	const std::vector<StartupSystemDescriptor>& GetRegisteredLateSystems() const;

private:
	std::vector<StartupSystemDescriptor> Descriptors;
	std::vector<StartupSystemDescriptor> LateDescriptors;
};