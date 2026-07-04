#include "ArkanoidInputModule.h"
#include "SystemsInitializationData.h"
#include "ComponentsInitializationData.h"
#include "InputSystem.h"
#include "ClickableComponent.h"

void ArkanoidInputModule::RegisterComponentTypes(ComponentsInitializationData& Data)
{
	Data.RegisterComponent<ClickableComponent>();
	Data.RegisterComponent<ClickableUsedComponent>();
}

void ArkanoidInputModule::RegisterSystems(SystemsInitializationData& Data)
{
	Data.RegisterSystem(InputSystem::TranslateRawInput, SystemPhase::Input);
	Data.RegisterSystem(InputSystem::UpdateClickables, SystemPhase::Input);
}