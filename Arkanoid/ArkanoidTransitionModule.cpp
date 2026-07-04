#include "ArkanoidTransitionModule.h"
#include "SystemsInitializationData.h"
#include "TransitionSystem.h"

void ArkanoidTransitionModule::RegisterSystems(SystemsInitializationData& Data)
{
	Data.RegisterSystem(TransitionSystem::UpdateTransition, SystemPhase::Update);
}