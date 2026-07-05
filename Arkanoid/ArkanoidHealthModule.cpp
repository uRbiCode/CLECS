#include "ArkanoidHealthModule.h"
#include "ComponentsInitializationData.h"
#include "SystemsInitializationData.h"
#include "HealthComponent.h"
#include "HealthSystem.h"
#include "DamageComponent.h"

void ArkanoidHealthModule::RegisterComponentTypes(ComponentsInitializationData& Data)
{
	Data.RegisterComponent<HealthComponent>();
	Data.RegisterComponent<HealthDeltaComponent>();
	Data.RegisterComponent<DamageComponent>();
}

void ArkanoidHealthModule::RegisterStartupSystems([[maybe_unused]] StartupSystemsInitializationData& Data)
{
}

void ArkanoidHealthModule::RegisterSystems(SystemsInitializationData& Data)
{
	Data.RegisterSystem(HealthSystem::UpdateDisplayedHealth, SystemPhase::EarlyUpdate);
	Data.RegisterSystem(HealthSystem::CleanupHealthDeltaComponents, SystemPhase::EarlyUpdate);
	Data.RegisterSystem(HealthSystem::RemoveDeadEntities, SystemPhase::EarlyUpdate);
	Data.RegisterSystem(HealthSystem::UpdatePersistentHealth, SystemPhase::EarlyUpdate);
	Data.RegisterSystem(HealthSystem::ApplyHealthChanges, SystemPhase::LateUpdate);
}