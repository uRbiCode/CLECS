#include "World.h"
#include "SDL3/SDL.h"
#include "SystemUpdateContext.h"

CLECS::ResultType World::Update(float DeltaTime)
{
	// Process SDL events
	SDL_Event Event;
	while (SDL_PollEvent(&Event))
	{
		if (Event.type == SDL_EVENT_QUIT)
			return CLECS::ResultType::Quit;
	}

	const auto UpdateContext = SystemUpdateContext{ *EntityManagerPtr };
	for (const auto& CurrentSystem : Systems)
	{
		CurrentSystem->Update(UpdateContext, DeltaTime);
	}

	return CLECS::ResultType::Success;
}

CLECS::ResultType World::InitializeWorld()
{
	EntityManagerPtr = std::make_unique<EntityManager>();
	return CLECS::ResultType::Success;
}