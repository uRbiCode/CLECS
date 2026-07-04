#include "ArkanoidUIBlinkModule.h"
#include "UIBlinkComponent.h"
#include "UIBlinkSystem.h"
#include "ComponentsInitializationData.h"
#include "SystemsInitializationData.h"

void ArkanoidUIBlinkModule::RegisterComponentTypes(ComponentsInitializationData& Data)
{
	Data.RegisterComponent<UIBlinkComponent>();
}

void ArkanoidUIBlinkModule::RegisterSystems(SystemsInitializationData& Data)
{
	Data.RegisterSystem(UIBlinkSystem::Update, SystemPhase::Update);
}