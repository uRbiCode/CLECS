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
	auto Entity = EntityAdmin.CreateEntity();
	EntityAdmin.AddComponent<TransformComponent>(Entity, Vector2D<float>{ 640.f, 360.f });
	EntityAdmin.AddComponent<ShapeComponent>(Entity, ShapeComponent::ShapeType::Rectangle, SDL_FRect{ -50.f, -50.f, 200.f, 50.f }, SDL_FColor{ 1.f, 0.f, 0.f, 1.f }, true, true);
	EntityAdmin.AddComponent<VelocityComponent>(Entity);
	auto& PlayerCollisionComponent = EntityAdmin.AddComponent<CollisionComponent>(Entity, CollisionChannel::Player);
	PlayerCollisionComponent.ResponseTable[ChannelToIndex(CollisionChannel::Static)] = CollisionResponse::Block;
	EntityAdmin.AddComponent<PlayerControllerComponent>(Entity);

	auto StaticEntity = EntityAdmin.CreateEntity();
	EntityAdmin.AddComponent<TransformComponent>(StaticEntity, Vector2D<float>{ 640.f, 500.f });
	EntityAdmin.AddComponent<ShapeComponent>(StaticEntity, ShapeComponent::ShapeType::Rectangle, SDL_FRect{ -100.f, -20.f, 200.f, 40.f }, SDL_FColor{ 0.f, 1.f, 0.f, 1.f }, true, true);
	EntityAdmin.AddComponent<CollisionComponent>(StaticEntity, CollisionChannel::Static);

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