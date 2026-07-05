#pragma once
#include "ModuleBase.h"

/* AudioModule provides audio-related components and systems that are a part of CLECS engine.
 * It is registered automatically after Game-specific Modules.
 */
class AudioModule : public ModuleBase
{
public:
	void RegisterComponentTypes(ComponentsInitializationData& Data) override;
	void RegisterStartupSystems(StartupSystemsInitializationData& Data) override;
	void RegisterSystems(SystemsInitializationData& Data) override;
};