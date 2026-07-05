#include "ArkanoidCoreModule.h"
#include "StartupSystemsInitializationData.h"
#include "GameStartSystem.h"

void ArkanoidCoreModule::RegisterComponentTypes([[maybe_unused]] ComponentsInitializationData& Data)
{
}

void ArkanoidCoreModule::RegisterStartupSystems(StartupSystemsInitializationData& Data)
{
	Data.RegisterSystem(GameStartSystem::StartArkanoidGame);
}

void ArkanoidCoreModule::RegisterSystems([[maybe_unused]] SystemsInitializationData& Data)
{
}