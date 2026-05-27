#pragma once
#include "SystemContext.h"
#include <vector>

/* Part of WorldInitializationData.
 * Allows Modules to provide Systems to be registered.
 */
struct SystemsInitializationData
{
	void RegisterSystem(
		void(*Initialize)(const SystemContext&),
		void(*Update)(SystemContext&, float))
	{
		SystemDescriptor Descriptor;
		Descriptor.Initialize = Initialize;
		Descriptor.Update = Update;
		Descriptors.emplace_back(std::move(Descriptor));
	}

	// TODO: REMOVE AFTER GAMEPLAY OVERHAUL
	void RegisterSystem(void(*Initialize)(SystemContext&))
	{
		SystemDescriptor Descriptor;
		Descriptor.Initialize = Initialize;
		Descriptors.emplace_back(std::move(Descriptor));
	}

	std::vector<SystemDescriptor>& AccessRegisteredSystems() { return Descriptors; }

private:
	std::vector<SystemDescriptor> Descriptors;
};