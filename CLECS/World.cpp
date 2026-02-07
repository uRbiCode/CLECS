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

void World::Update(float deltaTime)
{
	for (const auto& System : Systems)
	{
		System->Update(*this, deltaTime);
	}
}

CLECS::ResultType World::InitializeWorld()
{
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