#include "ModulesInitializationData.h"

const std::vector<std::unique_ptr<ModuleBase>>& ModulesInitializationData::GetRegisteredModules() const
{
	return Modules;
}