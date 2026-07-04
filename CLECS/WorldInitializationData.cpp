#include "WorldInitializationData.h"

WorldInitializationData WorldInitializationData::InitializeWithModules(ModulesInitializationData&& ModulesData)
{
	WorldInitializationData Data;
	for (const auto& ModulePtr : ModulesData.GetRegisteredModules())
	{
		ModulePtr->RegisterComponentTypes(Data.ComponentsData);
		ModulePtr->RegisterStartupSystems(Data.StartupSystemsData);
		ModulePtr->RegisterSystems(Data.SystemsData);
	}
	return Data;
}