#include "Arkanoid.h"
#include "PlayerInputSystem.h"
#include "MovementSystem.h"
#include "RenderSystem.h"
#include "WorldInitializationData.h"
#include "EntityAdmin.h"
#include "TransformComponent.h"
#include "ShapeComponents.h"
#include "PlayerControllerComponent.h"
#include "VelocityComponent.h"
#include "CollisionComponent.h"
#include "CollisionDetectionSystem.h"
#include "CollisionKinematicResolverSystem.h"
#include "HealthSystem.h"
#include "RenderComponent.h"
#include "CurrentStageResetComponent.h"
#include "CurrentStageSystem.h"
#include "RunStateComponent.h"
#include "RunControllerSystem.h"
#include "StageDataComponent.h"
#include <TextureComponent.h>

namespace
{
	constexpr float WallThickness = 20.f;
	constexpr float ScreenWidth = 640.f;
	constexpr float ScreenHeight = 480.f;
	constexpr int StagesCount = 3;

	constexpr Vector2D<float> BallInitialPosition = { ScreenWidth * 0.5f, ScreenHeight * 0.5f };
	constexpr Vector2D<float> BallInitialVelocity = { 0.f, 250.f };
	constexpr Vector2D<float> PlayerInitialPosition = { ScreenWidth * 0.5f, ScreenHeight * 0.9f };

	void AddPlayer(EntityAdmin& Admin)
	{
		auto PlayerEntity = Admin.CreateEntity();
		Admin.AddComponent<TransformComponent>(PlayerEntity, PlayerInitialPosition);
		Admin.AddComponent<RectComponent>(PlayerEntity, SDL_FRect{ -60.f, -10.f, 80.f, 20.f });
		Admin.AddComponent<RenderComponent>(PlayerEntity);
		Admin.AddComponent<VelocityComponent>(PlayerEntity);
		auto& PlayerCollisionComponent = Admin.AddComponent<CollisionComponent>(PlayerEntity, CollisionChannel::Player);
		PlayerCollisionComponent.ResponseTable[ChannelToIndex(CollisionChannel::Ball)] = CollisionResponse::Ignore;
		PlayerCollisionComponent.ResponseTable[ChannelToIndex(CollisionChannel::Trigger)] = CollisionResponse::Ignore;
		Admin.AddComponent<PlayerControllerComponent>(PlayerEntity);
	}

	void AddBall(EntityAdmin& Admin)
	{
		auto BallEntity = Admin.CreateEntity();
		Admin.AddComponent<TransformComponent>(BallEntity, BallInitialPosition);
		Admin.AddComponent<CircleComponent>(BallEntity, 10.f);
		Admin.AddComponent<ColorComponent>(BallEntity, SDL_FColor{ 0.f, 0.f, 1.f, 1.f });
		Admin.AddComponent<RenderComponent>(BallEntity);
		auto& BallVelocityComponent = Admin.AddComponent<VelocityComponent>(BallEntity);
		BallVelocityComponent.Velocity = BallInitialVelocity;
		auto& BallCollisionComponent = Admin.AddComponent<CollisionComponent>(BallEntity, CollisionChannel::Ball);
		BallCollisionComponent.ResponseTable[ChannelToIndex(CollisionChannel::Trigger)] = CollisionResponse::Ignore;
	}
	void AddCurrentStageResetComponent(EntityAdmin& Admin, const Vector2D<float>& BallInitialPosition, const Vector2D<float>& BallInitialVelocity, const Vector2D<float>& PlayerInitialPosition)
	{
		auto ResetEntity = Admin.CreateEntity();
		Admin.AddComponent<CurrentStageResetComponent>(ResetEntity, BallInitialPosition, BallInitialVelocity, PlayerInitialPosition);
	}

	void AddRunStateComponent(EntityAdmin& Admin)
	{
		auto RunStateEntity = Admin.CreateEntity();
		Admin.AddComponent<RunStateComponent>(RunStateEntity);
	}

	void AddStageDataComponents(EntityAdmin& Admin)
	{
		// Create brick grid
		constexpr int BrickRows = 5;
		constexpr int BrickColumns = 5;
		constexpr float BrickWidth = 64.f;
		constexpr float BrickHeight = 32.f;
		constexpr float BrickSpacing = 8.f;
		constexpr float GridStartX = (ScreenWidth - (BrickColumns * (BrickWidth + BrickSpacing))) * 0.5f;
		constexpr float GridStartY = WallThickness + BrickSpacing;

		auto StageDataEntity = Admin.CreateEntity();
		auto& StageDataEntityComponent = Admin.AddComponent<StageDataComponent>(StageDataEntity);

		for (int i = 0; i < StagesCount; ++i)
		{
			StageData NewStageData;
			NewStageData.PlayerSpawnPosition = PlayerInitialPosition;
			NewStageData.BallSpawnPosition = BallInitialPosition;
			NewStageData.BallInitialVelocity = BallInitialVelocity;

			NewStageData.Walls.push_back(WallData{ Vector2D<float>{ ScreenWidth * 0.5f, WallThickness * 0.5f }, Vector2D<float>{ ScreenWidth, WallThickness } });
			NewStageData.Walls.push_back(WallData{ Vector2D<float>{ WallThickness * 0.5f, ScreenHeight * 0.5f }, Vector2D<float>{ WallThickness, ScreenHeight } });
			NewStageData.Walls.push_back(WallData{ Vector2D<float>{ ScreenWidth - WallThickness * 0.5f, ScreenHeight * 0.5f }, Vector2D<float>{ WallThickness, ScreenHeight } });

			NewStageData.Trigger = TriggerData{ Vector2D<float>{ ScreenWidth * 0.5f, ScreenHeight - WallThickness * 0.5f }, Vector2D<float>{ ScreenWidth, WallThickness } };

			for (int Row = 0; Row < BrickRows; ++Row)
			{
				for (int Col = 0; Col < BrickColumns; ++Col)
				{
					BrickData NewBrick;
					NewBrick.Position = Vector2D<float>{ GridStartX + Col * (BrickWidth + BrickSpacing) + BrickWidth * 0.5f, GridStartY + Row * (BrickHeight + BrickSpacing) + BrickHeight * 0.5f };
					NewBrick.Size = Vector2D<float>{ BrickWidth, BrickHeight };
					NewBrick.Health = 1;
					NewBrick.TexturePath = "../Assets/Textures/bricks.png";
					NewBrick.TextureSourceRect = SDL_FRect{ 0.f, 23.f + static_cast<float>(16.f * Row), 32.f, 8.f};
					NewStageData.Bricks.push_back(NewBrick);
				}
			}
			
			StageDataEntityComponent.Stages.push_back(NewStageData);
		}
	}
}

bool Arkanoid::Initialize(WorldInitializationData& Data)
{
	Data.AddSystem<PlayerInputSystem>();
	Data.AddSystem<CollisionDetectionSystem>();
	Data.AddSystem<CollisionKinematicResolverSystem>();
	Data.AddSystem<HealthSystem>();
	Data.AddSystem<MovementSystem>();
	Data.AddSystem<CurrentStageSystem>();
	Data.AddSystem<RunControllerSystem>();
	Data.AddSystem<RenderSystem>();

	auto& EntityAdmin = Data.AccessEntityAdmin();

	AddRunStateComponent(EntityAdmin);
	AddCurrentStageResetComponent(EntityAdmin, BallInitialPosition, BallInitialVelocity, PlayerInitialPosition);

	AddPlayer(EntityAdmin);

	AddBall(EntityAdmin);
	
	AddStageDataComponents(EntityAdmin);

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