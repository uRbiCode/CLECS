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
#include "CollisionResolverSystem.h"
#include "HealthSystem.h"
#include "HealthComponent.h"
#include "RenderComponent.h"

namespace
{
	constexpr float WallThickness = 20.f;
	constexpr float ScreenWidth = 1280.f;
	constexpr float ScreenHeight = 720.f;

	void CreateWall(EntityAdmin& Admin, const Vector2D<float>& Position, const Vector2D<float>& Size)
	{
		auto Wall = Admin.CreateEntity();
		Admin.AddComponent<TransformComponent>(Wall, Position);
		Admin.AddComponent<RectComponent>(Wall,	SDL_FRect{ -Size.X * 0.5f, -Size.Y * 0.5f, Size.X, Size.Y });
		Admin.AddComponent<ColorComponent>(Wall, SDL_FColor{ 0.3f, 0.3f, 0.3f, 1.f });
		Admin.AddComponent<RenderComponent>(Wall);
		auto& WallCollisionComponent = Admin.AddComponent<CollisionComponent>(Wall, CollisionChannel::Static);
		WallCollisionComponent.ResponseTable[ChannelToIndex(CollisionChannel::Static)] = CollisionResponse::Ignore;
	}

	void CreatePlayer(EntityAdmin& Admin)
	{
		auto PlayerEntity = Admin.CreateEntity();
		Admin.AddComponent<TransformComponent>(PlayerEntity, Vector2D<float>{ 640.f, 650.f });
		Admin.AddComponent<RectComponent>(PlayerEntity, SDL_FRect{ -60.f, -10.f, 120.f, 20.f });
		Admin.AddComponent<ColorComponent>(PlayerEntity, SDL_FColor{ 0.7f, 0.7f, 0.7f, 1.f });
		Admin.AddComponent<RenderComponent>(PlayerEntity);
		Admin.AddComponent<VelocityComponent>(PlayerEntity);
		auto& PlayerCollisionComponent = Admin.AddComponent<CollisionComponent>(PlayerEntity, CollisionChannel::Player);
		PlayerCollisionComponent.ResponseTable[ChannelToIndex(CollisionChannel::Ball)] = CollisionResponse::Ignore;
		Admin.AddComponent<PlayerControllerComponent>(PlayerEntity);
	}

	void CreateBall(EntityAdmin& Admin)
	{
		auto BallEntity = Admin.CreateEntity();
		Admin.AddComponent<TransformComponent>(BallEntity, Vector2D<float>{ 640.f, 300.f });
		Admin.AddComponent<CircleComponent>(BallEntity, 10.f);
		Admin.AddComponent<ColorComponent>(BallEntity, SDL_FColor{ 0.f, 0.f, 1.f, 1.f });
		Admin.AddComponent<RenderComponent>(BallEntity);
		auto& BallVelocityComponent = Admin.AddComponent<VelocityComponent>(BallEntity);
		BallVelocityComponent.Velocity = { 0.f, 300.f };
		auto& BallCollisionComponent = Admin.AddComponent<CollisionComponent>(BallEntity, CollisionChannel::Ball);
	}

	void CreateBrick(EntityAdmin& Admin, const Vector2D<float>& Position, const Vector2D<float>& Size, const SDL_FColor& Color)
	{
		auto BrickEntity = Admin.CreateEntity();
		Admin.AddComponent<TransformComponent>(BrickEntity, Position);
		Admin.AddComponent<RectComponent>(BrickEntity, SDL_FRect{ -Size.X * 0.5f, -Size.Y * 0.5f, Size.X, Size.Y });
		Admin.AddComponent<ColorComponent>(BrickEntity, Color);
		Admin.AddComponent<RenderComponent>(BrickEntity);
		auto& BrickCollisionComponent = Admin.AddComponent<CollisionComponent>(BrickEntity, CollisionChannel::Brick);
		BrickCollisionComponent.ResponseTable[ChannelToIndex(CollisionChannel::Ball)] = CollisionResponse::Ignore;
		Admin.AddComponent<HealthComponent>(BrickEntity, 1);
	}
}

bool Arkanoid::Initialize(WorldInitializationData& Data)
{
	Data.AddSystem<PlayerInputSystem>();
	Data.AddSystem<CollisionDetectionSystem>();
	Data.AddSystem<CollisionResolverSystem>();
	Data.AddSystem<HealthSystem>();
	Data.AddSystem<MovementSystem>();
	Data.AddSystem<RenderSystem>();

	auto& EntityAdmin = Data.AccessEntityAdmin();

	CreateWall(EntityAdmin, Vector2D<float>{ ScreenWidth * 0.5f, WallThickness * 0.5f }, Vector2D<float>{ ScreenWidth, WallThickness });
	CreateWall(EntityAdmin, Vector2D<float>{ WallThickness * 0.5f, ScreenHeight * 0.5f }, Vector2D<float>{ WallThickness, ScreenHeight });
	CreateWall(EntityAdmin, Vector2D<float>{ ScreenWidth - WallThickness * 0.5f, ScreenHeight * 0.5f }, Vector2D<float>{ WallThickness, ScreenHeight });

	CreatePlayer(EntityAdmin);

	CreateBall(EntityAdmin);

	// Create brick grid
	constexpr int BrickRows = 5;
	constexpr int BrickColumns = 10;
	constexpr float BrickWidth = 100.f;
	constexpr float BrickHeight = 30.f;
	constexpr float BrickSpacing = 5.f;
	constexpr float GridStartX = (ScreenWidth - (BrickColumns * (BrickWidth + BrickSpacing))) * 0.5f;
	constexpr float GridStartY = 100.f;

	SDL_FColor BrickColors[] = {
		{ 1.f, 0.f, 0.f, 1.f },     // Red
		{ 1.f, 0.5f, 0.f, 1.f },    // Orange
		{ 1.f, 1.f, 0.f, 1.f },     // Yellow
		{ 0.f, 1.f, 0.f, 1.f },     // Green
		{ 0.f, 0.5f, 1.f, 1.f }     // Blue
	};

	for (int Row = 0; Row < BrickRows; ++Row)
	{
		for (int Col = 0; Col < BrickColumns; ++Col)
		{
			CreateBrick(EntityAdmin,
				Vector2D<float>{ GridStartX + Col * (BrickWidth + BrickSpacing) + BrickWidth * 0.5f, GridStartY + Row * (BrickHeight + BrickSpacing) + BrickHeight * 0.5f },
				Vector2D<float>{ BrickWidth, BrickHeight },
				BrickColors[Row % std::size(BrickColors)]);
		}
	}

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