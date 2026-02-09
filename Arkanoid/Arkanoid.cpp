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
#include "CollisionSystem.h"

bool Arkanoid::Initialize(WorldInitializationData& Data)
{
	Data.AddSystem<PlayerInputSystem>();
	Data.AddSystem<CollisionSystem>();
	Data.AddSystem<MovementSystem>();
	Data.AddSystem<RenderSystem>();

	auto& EntityAdmin = Data.AccessEntityAdmin();

	// Wall dimensions
	constexpr float WallThickness = 20.f;
	constexpr float ScreenWidth = 1280.f;
	constexpr float ScreenHeight = 720.f;

	// Top Wall
	auto TopWall = EntityAdmin.CreateEntity();
	EntityAdmin.AddComponent<TransformComponent>(TopWall, Vector2D<float>{ ScreenWidth * 0.5f, WallThickness * 0.5f });
	EntityAdmin.AddComponent<ShapeComponent>(TopWall, ShapeComponent::ShapeType::Rectangle, 
		SDL_FRect{ -ScreenWidth * 0.5f, -WallThickness * 0.5f, ScreenWidth, WallThickness }, 
		SDL_FColor{ 0.5f, 0.5f, 0.5f, 1.f }, true, true);
	EntityAdmin.AddComponent<CollisionComponent>(TopWall, CollisionChannel::Static);

	// Left Wall
	auto LeftWall = EntityAdmin.CreateEntity();
	EntityAdmin.AddComponent<TransformComponent>(LeftWall, Vector2D<float>{ WallThickness * 0.5f, ScreenHeight * 0.5f });
	EntityAdmin.AddComponent<ShapeComponent>(LeftWall, ShapeComponent::ShapeType::Rectangle, 
		SDL_FRect{ -WallThickness * 0.5f, -ScreenHeight * 0.5f, WallThickness, ScreenHeight }, 
		SDL_FColor{ 0.5f, 0.5f, 0.5f, 1.f }, true, true);
	EntityAdmin.AddComponent<CollisionComponent>(LeftWall, CollisionChannel::Static);

	// Right Wall
	auto RightWall = EntityAdmin.CreateEntity();
	EntityAdmin.AddComponent<TransformComponent>(RightWall, Vector2D<float>{ ScreenWidth - WallThickness * 0.5f, ScreenHeight * 0.5f });
	EntityAdmin.AddComponent<ShapeComponent>(RightWall, ShapeComponent::ShapeType::Rectangle, 
		SDL_FRect{ -WallThickness * 0.5f, -ScreenHeight * 0.5f, WallThickness, ScreenHeight }, 
		SDL_FColor{ 0.5f, 0.5f, 0.5f, 1.f }, true, true);
	EntityAdmin.AddComponent<CollisionComponent>(RightWall, CollisionChannel::Static);

	// Player Paddle
	auto PlayerEntity = EntityAdmin.CreateEntity();
	EntityAdmin.AddComponent<TransformComponent>(PlayerEntity, Vector2D<float>{ 640.f, 650.f });
	EntityAdmin.AddComponent<ShapeComponent>(PlayerEntity, ShapeComponent::ShapeType::Rectangle, 
		SDL_FRect{ -60.f, -10.f, 120.f, 20.f }, 
		SDL_FColor{ 1.f, 0.f, 0.f, 1.f }, true, true);
	EntityAdmin.AddComponent<VelocityComponent>(PlayerEntity);
	auto& PlayerCollisionComponent = EntityAdmin.AddComponent<CollisionComponent>(PlayerEntity, CollisionChannel::Player);
	EntityAdmin.AddComponent<PlayerControllerComponent>(PlayerEntity);

	// Ball
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
			EntityAdmin.AddComponent<CollisionComponent>(Brick, CollisionChannel::Static);
		}
	}

	return true;
}

RendererInitializationData Arkanoid::GetRendererConfig() const
{
	RendererInitializationData RendererConfig;
	RendererConfig.WindowTitle = "Arkanoid";
	RendererConfig.WindowWidth = 1280;
	RendererConfig.WindowHeight = 720;
	return RendererConfig;
}