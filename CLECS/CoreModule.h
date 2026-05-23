#pragma once
#include "ModuleBase.h"

/* CoreModule provides basic components and systems that are a part of CLECS engine.
 * It is registered automatically after Game-specific Modules.
 */
class CoreModule : public ModuleBase
{
public:
	void RegisterComponentTypes(ComponentsInitializationData& Data) override;
	void RegisterStartupSystems(StartupSystemsInitializationData& Data) override {}
	void RegisterSystems(SystemsInitializationData& Data) override;
};