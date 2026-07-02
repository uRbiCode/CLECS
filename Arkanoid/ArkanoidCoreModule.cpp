#include "ArkanoidCoreModule.h"
#include "StartupSystemsInitializationData.h"
#include "GameStartSystem.h"

void ArkanoidCoreModule::RegisterStartupSystems(StartupSystemsInitializationData& Data)
{
	Data.RegisterSystem(GameStartSystem::StartArkanoidGame);
}