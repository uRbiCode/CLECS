#include "ArkanoidInputModule.h"
#include "SystemsInitializationData.h"
#include "InputSystem.h"

void ArkanoidInputModule::RegisterSystems(SystemsInitializationData& Data)
{
	Data.RegisterSystem(InputSystem::TranslateRawInput, SystemPhase::Input);
}