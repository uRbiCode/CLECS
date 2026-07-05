#pragma once
#include "ComponentsInitializationData.h"
#include "SystemsInitializationData.h"
#include "StartupSystemsInitializationData.h"

struct ModulesInitializationData;

/* Allows World to be initialized with a specified configuration and Systems.
 * Forwarded to the Game class, which can fill it with necessary Systems and configure to its need.
 */
class WorldInitializationData
{
public:
	static WorldInitializationData InitializeWithModules(ModulesInitializationData&& ModulesData);

private:
	WorldInitializationData() = default;

	ComponentsInitializationData ComponentsData;
	StartupSystemsInitializationData StartupSystemsData;
	SystemsInitializationData SystemsData;

	friend class World;
};