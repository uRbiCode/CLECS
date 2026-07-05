#include "GameRunner.h"
#include "Game.h"
#include "World.h"
#include "SDL3/SDL.h"
#include "WorldInitializationData.h"
#include "ModulesInitializationData.h"
#include "TextureManager.h"
#include "AudioManager.h"
#include "FontManager.h"

namespace
{
	constexpr float MILLISECONDS_TO_SECONDS = 1000.f;

	float CalculateDeltaTime(Uint64& LastTime)
	{
		const Uint64 CurrentTime = SDL_GetTicks();
		const float DeltaTime = (static_cast<float>(CurrentTime) - static_cast<float>(LastTime)) / MILLISECONDS_TO_SECONDS;
		LastTime = CurrentTime;
		return DeltaTime;
	}
}

int GameRunner::Run(std::unique_ptr<Game> GameInstance)
{
	if (GameInstance == nullptr)
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "GameRunner::Run -> GameInstance is null");
		return 1;
	}

	if (!InitializeSDL())
		return 1;

	ModulesInitializationData ModulesData = ModulesInitializationData();
	GameInstance->InitializeModules(ModulesData);

	WorldInitializationData WorldData = WorldInitializationData::InitializeWithModules(std::move(ModulesData));

	World GameWorld = World();
	if (!InitializeWorld(GameWorld, std::move(WorldData), std::move(GameInstance->GetRendererConfig())))
	{
		Shutdown(*GameInstance, GameWorld);
		return 1;
	}

	const int ExitCode = RunGameLoop(GameWorld);

	Shutdown(*GameInstance,GameWorld);

	return ExitCode;
}

bool GameRunner::InitializeSDL()
{
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_AUDIO))
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "GameRunner::InitializeSDL -> SDL initialization failed: %s", SDL_GetError());
		return false;
	}
	return true;
}

bool GameRunner::InitializeWorld(World& GameWorld, WorldInitializationData&& WorldData, RendererInitializationData&& RendererData)
{
	CLECS::ResultType Result = GameWorld.InitializeRenderer(std::move(RendererData));
	if (Result != CLECS::ResultType::Success)
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "GameRunner::InitializeWorld -> World failed to initialize renderer");
		return false;
	}

	Result = GameWorld.InitializeWorld(std::move(WorldData));
	if (Result != CLECS::ResultType::Success)
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "GameRunner::InitializeWorld -> World failed to initialize");
		return false;
	}

	return true;
}

int GameRunner::RunGameLoop(World& GameWorld)
{
	bool Running = true;
	Uint64 LastTime = SDL_GetTicks();

	while (Running)
	{
		const float DeltaTime = CalculateDeltaTime(LastTime);

		const CLECS::ResultType UpdateResult = GameWorld.Update(DeltaTime);

		if (UpdateResult == CLECS::ResultType::Quit)
		{
			SDL_Log("GameRunner::RunGameLoop -> Received Quit from Update");
			Running = false;
		}

		else if (UpdateResult == CLECS::ResultType::Failure)
		{
			SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "GameRunner::RunGameLoop -> World update failed");
			return 1;
		}
	}

	return 0;
}

void GameRunner::Shutdown(Game& GameInstance, World& GameWorld)
{
	GameInstance.Shutdown();
	GameWorld.Shutdown();
	SDL_Quit();
}