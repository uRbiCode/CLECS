#include "World.h"
#include "SDL3/SDL.h"

namespace
{
	template<class T>
	concept SystemType = std::derived_from<T, System>;

	template<SystemType T>
	std::unique_ptr<T> CreateSystem()
	{
		return std::make_unique<T>();
	}
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

	// Update systems
	for (const auto& CurrentSystem : Systems)
	{
		CurrentSystem->Update(DeltaTime);
	}

	return CLECS::ResultType::Success;
}

CLECS::ResultType World::InitializeWorld()
{
	Entities = std::make_unique<EntityManager>();

	if (InitializeSystems() != CLECS::ResultType::Success)
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Failed to initialize systems");
		return CLECS::ResultType::Failure;
	}

	return CLECS::ResultType::Success;
}

CLECS::ResultType World::InitializeSystems()
{
	// Systems.push_back(CreateSystem<System>());
	return CLECS::ResultType::Success;
}