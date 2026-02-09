#pragma once
#include "CoreTypes.h"
#include "System.h"
#include "EntityAdmin.h"
#include "InputState.h"
#include "EventBus.h"

#include <vector>
#include <memory>

class WorldInitializationData;
struct SDL_Window;
struct SDL_Renderer;
struct RendererInitializationData;

using SystemCollection = std::vector<std::unique_ptr<System>>;

/* World is the heart of CLECS architecture.
 * It coordinates systems and provides access to entity management.
 * It owns the SDL window and renderer resources.
 */
class World
{
public:
	World() = default;
	~World();
	World(const World&) = delete;
	World& operator=(const World&) = delete;

	CLECS::ResultType InitializeWorld(WorldInitializationData& Data);
	CLECS::ResultType Update(float DeltaTime);
	void Shutdown();

private:
	bool CreateWindow(const RendererInitializationData& Data);
	bool CreateRenderer();
	SystemContext MakeSystemContext();

	std::unique_ptr<EntityAdmin> EntityAdminPtr;
	SystemCollection Systems;
	InputState Input;
	EventBus EventBus;
	SDL_Window* Window = nullptr;
	SDL_Renderer* Renderer = nullptr;
};