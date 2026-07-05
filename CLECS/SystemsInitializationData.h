#pragma once
#include "System.h"
#include <vector>

/* Part of WorldInitializationData.
 * Allows Modules to provide Systems to be registered.
 */
struct SystemsInitializationData
{
	void RegisterSystem(void(*Update)(SystemContext&, float), SystemPhase Phase);

	std::vector<SystemDescriptor>& AccessRegisteredSystems();

private:
	std::vector<SystemDescriptor> Systems;
};