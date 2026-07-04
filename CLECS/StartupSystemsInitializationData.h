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
	void RegisterSystem(void(*Initialize)(SystemContext&))
	{
		StartupSystemDescriptor Descriptor;
		Descriptor.Initialize = Initialize;
		Descriptors.emplace_back(std::move(Descriptor));
	}

	void RegisterLateSystem(void(*Initialize)(SystemContext&))
	{
		StartupSystemDescriptor Descriptor;
		Descriptor.Initialize = Initialize;
		LateDescriptors.emplace_back(std::move(Descriptor));
	}

	const std::vector<StartupSystemDescriptor>& GetRegisteredSystems() const { return Descriptors; }
	const std::vector<StartupSystemDescriptor>& GetRegisteredLateSystems() const { return LateDescriptors; }

private:
	std::vector<StartupSystemDescriptor> Descriptors;
	std::vector<StartupSystemDescriptor> LateDescriptors;
};