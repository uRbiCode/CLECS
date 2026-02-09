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

	auto PlayerEntity = EntityAdmin.CreateEntity();
	EntityAdmin.AddComponent<TransformComponent>(PlayerEntity, Vector2D<float>{ 640.f, 360.f });
	EntityAdmin.AddComponent<ShapeComponent>(PlayerEntity, ShapeComponent::ShapeType::Rectangle, SDL_FRect{ -50.f, -50.f, 100.f, 10.f }, SDL_FColor{ 1.f, 0.f, 0.f, 1.f }, true, true);
	EntityAdmin.AddComponent<VelocityComponent>(PlayerEntity);

	auto& PlayerCollisionComponent = EntityAdmin.AddComponent<CollisionComponent>(PlayerEntity, CollisionChannel::Player);
	EntityAdmin.AddComponent<PlayerControllerComponent>(PlayerEntity);

	auto StaticEntity = EntityAdmin.CreateEntity();
	EntityAdmin.AddComponent<TransformComponent>(StaticEntity, Vector2D<float>{ 640.f, 100.f });
	EntityAdmin.AddComponent<ShapeComponent>(StaticEntity, ShapeComponent::ShapeType::Rectangle, SDL_FRect{ -100.f, -20.f, 200.f, 40.f }, SDL_FColor{ 0.f, 1.f, 0.f, 1.f }, true, true);
	EntityAdmin.AddComponent<CollisionComponent>(StaticEntity, CollisionChannel::Static);

	auto StaticEntity2 = EntityAdmin.CreateEntity();
	EntityAdmin.AddComponent<TransformComponent>(StaticEntity2, Vector2D<float>{ 640.f, 600.f });
	EntityAdmin.AddComponent<ShapeComponent>(StaticEntity2, ShapeComponent::ShapeType::Rectangle, SDL_FRect{ -100.f, -20.f, 200.f, 40.f }, SDL_FColor{ 0.f, 1.f, 0.f, 1.f }, true, true);
	EntityAdmin.AddComponent<CollisionComponent>(StaticEntity2, CollisionChannel::Static);

	auto BallEntity = EntityAdmin.CreateEntity();
	EntityAdmin.AddComponent<TransformComponent>(BallEntity, Vector2D<float>{ 640.f, 400.f });
	EntityAdmin.AddComponent<ShapeComponent>(BallEntity, ShapeComponent::ShapeType::Circle, SDL_FRect{ -25.f, -25.f, 50.f, 50.f }, SDL_FColor{ 0.f, 0.f, 1.f, 1.f }, true, true);
	auto& BallVelocityComponent = EntityAdmin.AddComponent<VelocityComponent>(BallEntity);
	BallVelocityComponent.Velocity = { 0.f, -50.f };
	auto& BallCollisionComponent = EntityAdmin.AddComponent<CollisionComponent>(BallEntity, CollisionChannel::Ball);

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