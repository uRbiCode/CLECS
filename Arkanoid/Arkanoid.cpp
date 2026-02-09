#include "Arkanoid.h"
#include "PlayerInputSystem.h"
#include "MovementSystem.h"
#include "RenderSystem.h"
#include "WorldInitializationData.h"
#include "EntityAdmin.h"
#include "TransformComponent.h"
#include "ShapeComponent.h"
#include "PlayerControllerComponent.h"
#include "VelocityComponent.h"
#include "CollisionComponent.h"
#include "CollisionDetectionSystem.h"
#include "CollisionResolverSystem.h"
#include "HealthSystem.h"
#include "HealthComponent.h"

namespace
{
	constexpr float WallThickness = 20.f;
	constexpr float ScreenWidth = 1280.f;
	constexpr float ScreenHeight = 720.f;

	void CreateWall(EntityAdmin& Admin, const Vector2D<float>& Position, const Vector2D<float>& Size)
	{
		auto Wall = Admin.CreateEntity();
		Admin.AddComponent<TransformComponent>(Wall, Position);
		Admin.AddComponent<ShapeComponent>(Wall, ShapeComponent::ShapeType::Rectangle,
			SDL_FRect{ -Size.X * 0.5f, -Size.Y * 0.5f, Size.X, Size.Y },
			SDL_FColor{ 0.3f, 0.3f, 0.3f, 1.f }, true, true);
		Admin.AddComponent<CollisionComponent>(Wall, CollisionChannel::Static);
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

	auto PlayerEntity = EntityAdmin.CreateEntity();
	EntityAdmin.AddComponent<TransformComponent>(PlayerEntity, Vector2D<float>{ 640.f, 650.f });
	EntityAdmin.AddComponent<ShapeComponent>(PlayerEntity, ShapeComponent::ShapeType::Rectangle, 
		SDL_FRect{ -60.f, -10.f, 120.f, 20.f }, 
		SDL_FColor{ 0.7f, 0.7f, 0.7f, 1.f }, true, true);
	EntityAdmin.AddComponent<VelocityComponent>(PlayerEntity);
	auto& PlayerCollisionComponent = EntityAdmin.AddComponent<CollisionComponent>(PlayerEntity, CollisionChannel::Player);
	EntityAdmin.AddComponent<PlayerControllerComponent>(PlayerEntity);

	auto BallEntity = EntityAdmin.CreateEntity();
	EntityAdmin.AddComponent<TransformComponent>(BallEntity, Vector2D<float>{ 640.f, 500.f });
	EntityAdmin.AddComponent<ShapeComponent>(BallEntity, ShapeComponent::ShapeType::Circle, 
		SDL_FRect{ -10.f, -10.f, 20.f, 20.f }, 
		SDL_FColor{ 0.f, 0.f, 1.f, 1.f }, true, true);
	auto& BallVelocityComponent = EntityAdmin.AddComponent<VelocityComponent>(BallEntity);
	BallVelocityComponent.Velocity = { 150.f, -200.f };
	auto& BallCollisionComponent = EntityAdmin.AddComponent<CollisionComponent>(BallEntity, CollisionChannel::Ball);

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
			auto Brick = EntityAdmin.CreateEntity();
			
			float BrickX = GridStartX + Col * (BrickWidth + BrickSpacing) + BrickWidth * 0.5f;
			float BrickY = GridStartY + Row * (BrickHeight + BrickSpacing) + BrickHeight * 0.5f;
			
			EntityAdmin.AddComponent<TransformComponent>(Brick, Vector2D<float>{ BrickX, BrickY });
			EntityAdmin.AddComponent<ShapeComponent>(Brick, ShapeComponent::ShapeType::Rectangle, 
				SDL_FRect{ -BrickWidth * 0.5f, -BrickHeight * 0.5f, BrickWidth, BrickHeight }, 
				BrickColors[Row], true, true);
			EntityAdmin.AddComponent<CollisionComponent>(Brick, CollisionChannel::Brick);
			EntityAdmin.AddComponent<HealthComponent>(Brick, 1);
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