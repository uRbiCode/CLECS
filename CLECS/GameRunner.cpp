#include "GameRunner.h"
#include "Game.h"
#include "World.h"
#include "SDL3/SDL.h"
#include "WorldInitializationData.h"

namespace
{
	constexpr float MILLISECONDS_TO_SECONDS = 1000.0f;


	float CalculateDeltaTime(Uint64& LastTime)
	{
		const Uint64 CurrentTime = SDL_GetTicks();
		const float DeltaTime = (CurrentTime - LastTime) / MILLISECONDS_TO_SECONDS;
		LastTime = CurrentTime;
		return DeltaTime;
	}
}

int GameRunner::Run(std::unique_ptr<Game> GameInstance)
{
	if (GameInstance == nullptr)
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "No game instance provided");
		return 1;
	}

	if (!InitializeSDL())
		return 1;

	SDL_Window* Window = CreateGameWindow(*GameInstance);
	if (Window == nullptr)
	{
		ShutdownSDL();
		return 1;
	}

	auto WorldInitializationData = WorldInitializationData::Create();
	if (!GameInstance->Initialize(WorldInitializationData))
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Game initialization failed");
		SDL_DestroyWindow(Window);
		ShutdownSDL();
		return 1;
	}

	World GameWorld;

	if (!InitializeWorld(GameWorld, WorldInitializationData))
	{
		SDL_DestroyWindow(Window);
		ShutdownSDL();
		return 1;
	}

	const int ExitCode = RunGameLoop(GameWorld);

	GameInstance->Shutdown();
	SDL_DestroyWindow(Window);
	ShutdownSDL();

	return ExitCode;
}

bool GameRunner::InitializeSDL()
{
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "SDL initialization failed: %s", SDL_GetError());
		return false;
	}
	return true;
}

void GameRunner::ShutdownSDL()
{
	SDL_Quit();
}

SDL_Window* GameRunner::CreateGameWindow(const Game& GameInstance)
{
	SDL_Window* Window = SDL_CreateWindow(
		GameInstance.GetWindowTitle(),
		GameInstance.GetWindowWidth(),
		GameInstance.GetWindowHeight(),
		SDL_WINDOW_RESIZABLE
	);

	if (Window == nullptr)
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Window creation failed: %s", SDL_GetError());
	}

	return Window;
}

bool GameRunner::InitializeWorld(World& GameWorld, WorldInitializationData& Data)
{
	const CLECS::ResultType Result = GameWorld.InitializeWorld(Data);
	if (Result != CLECS::ResultType::Success)
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "World failed to initialize");
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
			Running = false;
			return 0;
		}

		if (UpdateResult == CLECS::ResultType::Failure)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "World update failed");
			return 1;
		}
	}

	return 0;
}