#include "ArkanoidInputModule.h"
#include "SystemsInitializationData.h"
#include "ComponentsInitializationData.h"
#include "InputSystem.h"
#include "ClickableComponent.h"

void ArkanoidInputModule::RegisterComponentTypes(ComponentsInitializationData& Data)
{
	Data.RegisterComponent<ClickableComponent>();
}

void ArkanoidInputModule::RegisterSystems(SystemsInitializationData& Data)
{
	Data.RegisterSystem(InputSystem::TranslateRawInput, SystemPhase::Input);
}