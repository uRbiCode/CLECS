#pragma once
#include "ModuleBase.h"
class ArkanoidUpgradeModule : public ModuleBase
{
public:
	void RegisterComponentTypes(ComponentsInitializationData& Data) override;
	void RegisterStartupSystems(StartupSystemsInitializationData& Data) override;
	void RegisterSystems(SystemsInitializationData& Data) override;
};