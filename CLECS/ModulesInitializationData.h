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
		Modules.insert_or_assign(typeid(Module), std::make_unique<Module>());
	}

	const std::unordered_map<std::type_index, std::unique_ptr<ModuleBase>>& GetRegisteredModules() const { return Modules; }

private:
	std::unordered_map<std::type_index, std::unique_ptr<ModuleBase>> Modules;
};