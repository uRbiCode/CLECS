#include "Arkanoid.h"
#include "PlayerInputSystem.h"
#include "MovementSystem.h"
#include "RenderSystem.h"
#include "WorldInitializationData.h"
#include "EntityAdmin.h"
#include "CollisionDetectionSystem.h"
#include "CollisionKinematicResolverSystem.h"
#include "HealthSystem.h"
#include "CurrentStageSystem.h"
#include "RunControllerSystem.h"
#include "HealthIndicatorSystem.h"
#include "GameStateSystem.h"
#include "GameStateComponent.h"
#include "MainMenuControllerSystem.h"
#include "AudioSystem.h"
#include "SummaryControllerSystem.h"
#include "StageInfoSystem.h"
#include "UpgradeControllerSystem.h"
#include "TutorialControllerSystem.h"
#include "PlayerPrepareSystem.h"

namespace
{
	constexpr float ScreenWidth = 640.f;
	constexpr float ScreenHeight = 480.f;
}

bool Arkanoid::Initialize(WorldInitializationData& Data)
{
	AddGameStateComponent(Data.AccessEntityAdmin());

	Data.AddSystem({ AudioSystem::Initialize, AudioSystem::Update });
	Data.AddSystem({ PlayerInputSystem::Initialize, PlayerInputSystem::Update });
	Data.AddSystem({ nullptr, CollisionDetectionSystem::Update });
	Data.AddSystem({ CollisionKinematicResolverSystem::Initialize, nullptr });
	Data.AddSystem({ HealthSystem::Initialize, HealthSystem::Update });
	Data.AddSystem({ nullptr, MovementSystem::Update });
	Data.AddSystem({ CurrentStageSystem::Initialize, nullptr });
	Data.AddSystem({ UpgradeControllerSystem::Initialize, nullptr });
	Data.AddSystem({ PlayerPrepareSystem::Initialize, PlayerPrepareSystem::Update });
	Data.AddSystem({ MainMenuControllerSystem::Initialize, nullptr });
	Data.AddSystem({ RunControllerSystem::Initialize, nullptr });
	Data.AddSystem({ TutorialControllerSystem::Initialize, nullptr });
	Data.AddSystem({ SummaryControllerSystem::Initialize, nullptr });
	Data.AddSystem({ GameStateSystem::Initialize, nullptr });
	Data.AddSystem({ HealthIndicatorSystem::Initialize, nullptr });
	Data.AddSystem({ StageInfoSystem::Initialize, nullptr });
	Data.AddSystem({ nullptr, RenderSystem::Update });

	return true;
}

RendererInitializationData Arkanoid::GetRendererConfig() const
{
	RendererInitializationData RendererConfig;
	RendererConfig.WindowTitle = "Arkanoid";
	RendererConfig.WindowWidth = static_cast<int>(ScreenWidth);
	RendererConfig.WindowHeight = static_cast<int>(ScreenHeight);
	return RendererConfig;
}

void Arkanoid::AddGameStateComponent(EntityAdmin& Admin) const
{
	const auto GameStateEntity = Admin.CreateEntity();
	Admin.AddComponent<GameStateComponent>(GameStateEntity, GameState::MainMenu);
}