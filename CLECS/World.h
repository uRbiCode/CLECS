#pragma once
#include "CoreTypes.h"
#include "SystemsCollection.h"
#include "InputState.h"
#include "EventBus.h"
#include "TextureManager.h"
#include "AudioManager.h"
#include "FontManager.h"
#include "ArchetypeStorage.h"
#include "CommandRunner.h"
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

	CLECS::ResultType InitializeRenderer(RendererInitializationData&& RendererData);
	CLECS::ResultType InitializeWorld(WorldInitializationData&& Data);
	CLECS::ResultType Update(float DeltaTime);

	void Shutdown();

private:
	bool CreateWindow(const RendererInitializationData& Data);
	bool CreateRenderer();
	void InitializeTextureManager();
	void InitializeAudioManager();
	void InitializeFontManager();
	SystemContext MakeSystemContext();

	void FlushCommands();

	SystemsCollection Systems;
	ArchetypeStorage Archetypes;
	CommandRunner Commands;

	InputState Input;
	EventBus EventBus;

	std::unique_ptr<TextureManager> TextureManagerPtr;
	std::unique_ptr<AudioManager> AudioManagerPtr;
	std::unique_ptr<FontManager> FontManagerPtr;

	SDL_Window* Window = nullptr;
	SDL_Renderer* Renderer = nullptr;
};