#include "ArkanoidModule.h"
#include "ComponentsInitializationData.h"
#include "StartupSystemsInitializationData.h"
#include "SystemsInitializationData.h"
#include "VelocityComponent.h"
#include "UpgradeComponent.h"
#include "StageDataComponent.h"
#include "ClickableComponent.h"
#include "CollisionComponent.h"
#include "GameStateComponent.h"
#include "HealthComponent.h"
#include "PlayerControllerComponent.h"
#include "PlayerPrepareComponent.h"
#include "RunStateComponent.h"
#include "PlayerInputSystem.h"
#include "CollisionDetectionSystem.h"
#include "CollisionKinematicResolverSystem.h"
#include "HealthSystem.h"
#include "CurrentStageSystem.h"
#include "MovementSystem.h"
#include "UpgradeControllerSystem.h"
#include "PlayerPrepareSystem.h"
#include "MainMenuControllerSystem.h"
#include "RunControllerSystem.h"
#include "TutorialControllerSystem.h"
#include "SummaryControllerSystem.h"
#include "GameStateSystem.h"
#include "HealthIndicatorSystem.h"
#include "StageInfoSystem.h"

void ArkanoidModule::RegisterComponentTypes(ComponentsInitializationData& Data)
{
	Data.RegisterComponent<VelocityComponent>();
	Data.RegisterComponent<AvailableUpgradesComponent>();
	Data.RegisterComponent<OwnedUpgradesComponent>();
	Data.RegisterComponent<UpgradeComponent>();
	Data.RegisterComponent<StageDataComponent>();
	Data.RegisterComponent<ClickableComponent>();
	Data.RegisterComponent<CollisionComponent>();
	Data.RegisterComponent<GameStateComponent>();
	Data.RegisterComponent<HealthComponent>();
	Data.RegisterComponent<PlayerControllerComponent>();
	Data.RegisterComponent<PlayerPrepareComponent>();
	Data.RegisterComponent<RunStateComponent>();
}

void ArkanoidModule::RegisterStartupSystems(StartupSystemsInitializationData& Data)
{
	Data.RegisterSystem(PlayerInputSystem::Initialize);
	Data.RegisterSystem(CollisionKinematicResolverSystem::Initialize);
	Data.RegisterSystem(HealthSystem::Initialize);
	Data.RegisterSystem(CurrentStageSystem::Initialize);
	Data.RegisterSystem(UpgradeControllerSystem::Initialize);
	Data.RegisterSystem(MainMenuControllerSystem::Initialize);
	Data.RegisterSystem(PlayerPrepareSystem::Initialize);
	Data.RegisterSystem(RunControllerSystem::Initialize);
	Data.RegisterSystem(TutorialControllerSystem::Initialize);
	Data.RegisterSystem(SummaryControllerSystem::Initialize);
	Data.RegisterSystem(GameStateSystem::Initialize);
	Data.RegisterSystem(HealthIndicatorSystem::Initialize);
	Data.RegisterSystem(StageInfoSystem::Initialize);
}

void ArkanoidModule::RegisterSystems(SystemsInitializationData& Data)
{
	Data.RegisterSystem(PlayerInputSystem::Update, SystemPhase::Update);
	Data.RegisterSystem(CollisionDetectionSystem::Update, SystemPhase::Update);
	Data.RegisterSystem(HealthSystem::Update, SystemPhase::Update);
	Data.RegisterSystem(MovementSystem::Update, SystemPhase::Update);
	Data.RegisterSystem(PlayerPrepareSystem::Update, SystemPhase::Update);
}