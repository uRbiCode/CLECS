#include "ExampleGame.h"
#include "RendererInitializationData.h"

bool ExampleGame::Initialize(WorldInitializationData& Data)
{
	// TODO: Add your systems here
	// GameWorld.AddSystem<YourSystem>();

	// TODO: Create initial entities
	// EntityManager* EntityMgr = GameWorld.GetEntityManager();
	// EntityMgr->CreateEntity();

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