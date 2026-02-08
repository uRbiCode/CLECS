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
	EntityManager.AddComponent<TransformComponent>(Entity, 640.0f, 360.0f);
	EntityManager.AddComponent<ShapeComponent>(Entity, ShapeComponent::ShapeType::Circle, SDL_FRect{ -50.0f, -50.0f, 100.0f, 100.0f }, SDL_FColor{ 1.0f, 0.f, 0.f, 1.0f }, true, true);

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