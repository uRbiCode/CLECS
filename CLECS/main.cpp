#include "SDL3/SDL.h"
#include <iostream>

#include "World.h"

int main(int argc, char* argv[])
{
	World GameWorld;
    if (GameWorld.InitializeWorld() != CLECS::ResultType::Success)
    {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "World failed to initialize");
        return 1;
    }

    bool Running = true;
	Uint64 CurrentTime = SDL_GetTicks();
    Uint64 LastTime = CurrentTime;
    
    while (Running)
    {
        CurrentTime = SDL_GetTicks();
        const float DeltaTime = (CurrentTime - LastTime) / 1000.0f;
        LastTime = CurrentTime;

        const CLECS::ResultType UpdateResult = GameWorld.Update(DeltaTime);
        if (UpdateResult == CLECS::ResultType::Quit)
        {
            Running = false;
            return 0;
        }
        else if (UpdateResult == CLECS::ResultType::Failure)
        {
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "World update failed");
            return 1;
        }
    }
}