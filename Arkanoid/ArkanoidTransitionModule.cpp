#include "ArkanoidTransitionModule.h"
#include "SystemsInitializationData.h"
#include "TransitionSystem.h"
#include "TransitionComponents.h"
#include "ComponentsInitializationData.h"

void ArkanoidTransitionModule::RegisterComponentTypes(ComponentsInitializationData& Data)
{
	Data.RegisterComponent<SummaryTransitionComponent>();
	Data.RegisterComponent<UpgradesTransitionComponent>();
}

void ArkanoidTransitionModule::RegisterSystems(SystemsInitializationData& Data)
{
	Data.RegisterSystem(TransitionSystem::UpdateDynamicTransitions, SystemPhase::EarlyUpdate);
	Data.RegisterSystem(TransitionSystem::UpdateClickableTransitions, SystemPhase::Update);
	Data.RegisterSystem(TransitionSystem::UpdateTransitionsFromRun, SystemPhase::LateUpdate);
}