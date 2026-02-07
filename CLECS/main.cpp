#include "SDL3/SDL.h"
#include "World.h"

namespace
{
	constexpr float MILLISECONDS_TO_SECONDS = 1000.0f;

	bool InitializeApplication(World& GameWorld)
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
	World GameWorld;

	if (!InitializeApplication(GameWorld))
		return 1;

	return RunGameLoop(GameWorld);
}