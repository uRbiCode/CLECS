#include "ArkanoidUpgradeModule.h"
#include "ComponentsInitializationData.h"
#include "StartupSystemsInitializationData.h"
#include "UpgradeComponents.h"
#include "UpgradeSystem.h"
#include "SystemsInitializationData.h"

void ArkanoidUpgradeModule::RegisterComponentTypes(ComponentsInitializationData& Data)
{
	Data.RegisterComponent<AvailableUpgradeComponent>();
	Data.RegisterComponent<UpgradeDescriptionComponent>();
	Data.RegisterComponent<PaddleWidthMultiplierUpgradeComponent>();
	Data.RegisterComponent<BallSpeedMultiplierUpgradeComponent>();
	Data.RegisterComponent<BallSizeMultiplierUpgradeComponent>();
	Data.RegisterComponent<HealUpgradeComponent>();
}

void ArkanoidUpgradeModule::RegisterStartupSystems(StartupSystemsInitializationData& Data)
{
	Data.RegisterSystem(UpgradeSystem::SpawnUpgradeEntities);
	Data.RegisterLateSystem(UpgradeSystem::InitializeUpgrades);
}

void ArkanoidUpgradeModule::RegisterSystems(SystemsInitializationData& Data)
{
	Data.RegisterSystem(UpgradeSystem::UpdateOwnedUpgrades, SystemPhase::EarlyUpdate);
	Data.RegisterSystem(UpgradeSystem::UpdateUpgradesChoice, SystemPhase::EarlyUpdate);
	Data.RegisterSystem(UpgradeSystem::ResetUpgrades, SystemPhase::EarlyUpdate);
	Data.RegisterSystem(UpgradeSystem::UpdateImmediateUpgrades, SystemPhase::EarlyUpdate);
}