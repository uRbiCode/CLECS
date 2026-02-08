#include "ExampleGame.h"
#include "RenderSystem.h"
#include "WorldInitializationData.h"
#include "EntityManager.h"
#include "Components.h"

bool ExampleGame::Initialize(WorldInitializationData& Data)
{
	Data.AddSystem<RenderSystem>();

	auto& EntityManager = Data.AccessEntityManager();
	auto Entity = EntityManager.CreateEntity();
	EntityManager.AddComponent<TransformComponent>(Entity, Vector2D<float>{ 640.f, 360.f });
	EntityManager.AddComponent<ShapeComponent>(Entity, ShapeComponent::ShapeType::Circle, SDL_FRect{ -50.f, -50.f, 100.f, 100.f }, SDL_FColor{ 1.f, 0.f, 0.f, 1.f }, true, true);

	return true;
}

RendererInitializationData ExampleGame::GetRendererConfig() const
{
	RendererInitializationData RendererConfig;
	RendererConfig.WindowTitle = "CLECS Example Game";
	RendererConfig.WindowWidth = 1280;
	RendererConfig.WindowHeight = 720;
	return RendererConfig;
}