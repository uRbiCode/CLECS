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

namespace
{
	constexpr float ScreenWidth = 640.f;
	constexpr float ScreenHeight = 480.f;
}

bool Arkanoid::Initialize(WorldInitializationData& Data)
{
	AddGameStateComponent(Data.AccessEntityAdmin());

	Data.AddSystem<PlayerInputSystem>();
	Data.AddSystem<HealthIndicatorSystem>();
	Data.AddSystem<CollisionDetectionSystem>();
	Data.AddSystem<CollisionKinematicResolverSystem>();
	Data.AddSystem<HealthSystem>();
	Data.AddSystem<MovementSystem>();
	Data.AddSystem<CurrentStageSystem>();
	Data.AddSystem<RunControllerSystem>();
	Data.AddSystem<GameStateSystem>();
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

	// TODO: change, for now start on run
	Admin.AddComponent<GameStateComponent>(GameStateEntity, GameState::Run);
}