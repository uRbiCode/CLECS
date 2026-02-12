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

namespace
{
	constexpr float ScreenWidth = 640.f;
	constexpr float ScreenHeight = 480.f;
}

bool Arkanoid::Initialize(WorldInitializationData& Data)
{
	AddGameStateComponent(Data.AccessEntityAdmin());

	Data.AddSystem<AudioSystem>();
	Data.AddSystem<PlayerInputSystem>();
	Data.AddSystem<CollisionDetectionSystem>();
	Data.AddSystem<CollisionKinematicResolverSystem>();
	Data.AddSystem<HealthSystem>();
	Data.AddSystem<MovementSystem>();
	Data.AddSystem<CurrentStageSystem>();
	Data.AddSystem<UpgradeControllerSystem>();
	Data.AddSystem<MainMenuControllerSystem>();
	Data.AddSystem<RunControllerSystem>();
	Data.AddSystem<TutorialControllerSystem>();
	Data.AddSystem<SummaryControllerSystem>();
	Data.AddSystem<GameStateSystem>();
	Data.AddSystem<HealthIndicatorSystem>();
	Data.AddSystem<StageInfoSystem>();
	Data.AddSystem<RenderSystem>();

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
	auto GameStateEntity = Admin.CreateEntity();
	Admin.AddComponent<GameStateComponent>(GameStateEntity, GameState::MainMenu);
}