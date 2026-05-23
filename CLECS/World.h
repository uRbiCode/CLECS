#pragma once
#include "CoreTypes.h"
#include "SystemCollection.h"
#include "EntityAdmin.h"
#include "InputState.h"
#include "EventBus.h"
#include "TextureManager.h"
#include "AudioManager.h"
#include "FontManager.h"
#include <memory>

class WorldInitializationData;
struct SDL_Window;
struct SDL_Renderer;
struct RendererInitializationData;
struct SystemContext;

/* World is the heart of CLECS architecture.
 * It owns the SDL and CLECS resources and manages their lifecycle.
 * It coordinates systems and provides access to entity management to them.
 * It translates SDL events to the InputState.
 */
class World
{
public:
	World() = default;
	~World();
	World(const World&) = delete;
	World& operator=(const World&) = delete;

	CLECS::ResultType InitializeWorld(WorldInitializationData&& Data, RendererInitializationData&& RendererData);
	CLECS::ResultType Update(float DeltaTime);
	void Shutdown();

private:
	bool CreateWindow(const RendererInitializationData& Data);
	bool CreateRenderer();
	void InitializeTextureManager();
	void InitializeAudioManager();
	void InitializeFontManager();
	void SendInputEvents(const SystemContext& Context);
	SystemContext MakeSystemContext();

	std::unique_ptr<EntityAdmin> EntityAdminPtr;
	SystemCollection Systems;
	InputState Input;
	EventBus EventBus;

	std::unique_ptr<TextureManager> TextureManagerPtr;
	std::unique_ptr<AudioManager> AudioManagerPtr;
	std::unique_ptr<FontManager> FontManagerPtr;

	SDL_Window* Window = nullptr;
	SDL_Renderer* Renderer = nullptr;
};