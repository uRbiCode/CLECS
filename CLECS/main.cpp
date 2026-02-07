#include "SDL3/SDL.h"
#include "SDL3/SDL_main.h"
#include "World.h"

namespace
{
	constexpr float MILLISECONDS_TO_SECONDS = 1000.0f;
	constexpr int WINDOW_WIDTH = 1280;
	constexpr int WINDOW_HEIGHT = 720;

	bool InitializeSDL()
	{
		if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS))
		{
			SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "SDL initialization failed: %s", SDL_GetError());
			return false;
		}
		return true;
	}

	void ShutdownSDL()
	{
		SDL_Quit();
	}

	SDL_Window* CreateGameWindow()
	{
		SDL_Window* Window = SDL_CreateWindow(
			"CLECS",
			WINDOW_WIDTH,
			WINDOW_HEIGHT,
			SDL_WINDOW_RESIZABLE
		);

		if (!Window)
		{
			SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Window creation failed: %s", SDL_GetError());
		}

		return Window;
	}

	bool InitializeWorld(World& GameWorld)
	{
		const CLECS::ResultType Result = GameWorld.InitializeWorld();
		if (Result != CLECS::ResultType::Success)
		{
			SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "World failed to initialize");
			return false;
		}
		return true;
	}

	float CalculateDeltaTime(Uint64& LastTime)
	{
		const Uint64 CurrentTime = SDL_GetTicks();
		const float DeltaTime = (CurrentTime - LastTime) / MILLISECONDS_TO_SECONDS;
		LastTime = CurrentTime;
		return DeltaTime;
	}

	int RunGameLoop(World& GameWorld)
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
}

int main(int argc, char* argv[])
{
	if (!InitializeSDL())
		return 1;

	const auto Window = CreateGameWindow();
	if (Window == nullptr)
	{
		ShutdownSDL();
		return 1;
	}

	World GameWorld;

	if (!InitializeWorld(GameWorld))
	{
		SDL_DestroyWindow(Window);
		ShutdownSDL();
		return 1;
	}

	const auto ExitCode = RunGameLoop(GameWorld);
	
	SDL_DestroyWindow(Window);
	ShutdownSDL();
	
	return ExitCode;
}