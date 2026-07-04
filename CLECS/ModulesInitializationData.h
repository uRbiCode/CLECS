#pragma once
#include "ModuleBase.h"
#include <memory>
#include <unordered_map>
#include <typeindex>

struct ModulesInitializationData
{
	template<typename Module>
	requires std::derived_from<Module, ModuleBase>
	void RegisterModule()
	{
		Modules.push_back(std::make_unique<Module>());
	}

	const std::vector<std::unique_ptr<ModuleBase>>& GetRegisteredModules() const { return Modules; }

private:
	std::vector<std::unique_ptr<ModuleBase>> Modules;
};