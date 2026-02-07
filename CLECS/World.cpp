#include "World.h"
#include "SDL3/SDL.h"
#include "SystemUpdateContext.h"
#include "WorldInitializationData.h"

CLECS::ResultType World::InitializeWorld(WorldInitializationData& Data)
{
	EntityManagerPtr = std::move(Data.EntityManagerPtr);
	Systems = std::move(Data.Systems);
	return CLECS::ResultType::Success;
}

CLECS::ResultType World::Update(float DeltaTime)
{
	// Process SDL events
	SDL_Event Event;
	while (SDL_PollEvent(&Event))
	{
		if (Event.type == SDL_EVENT_QUIT)
			return CLECS::ResultType::Quit;
	}

	const auto UpdateContext = SystemUpdateContext{ *EntityManagerPtr, DeltaTime };
	for (const auto& CurrentSystem : Systems)
	{
		CurrentSystem->Update(UpdateContext);
	}

	return CLECS::ResultType::Success;
}