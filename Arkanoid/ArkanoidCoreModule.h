#pragma once
#include "ModuleBase.h"

class ArkanoidCoreModule : public ModuleBase
{
public:
	void RegisterComponentTypes(ComponentsInitializationData& Data) {}
	void RegisterStartupSystems(StartupSystemsInitializationData& Data) override;
	void RegisterSystems(SystemsInitializationData& Data) {}
};