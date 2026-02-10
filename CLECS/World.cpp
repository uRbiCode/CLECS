#include "World.h"
#include "SystemContext.h"
#include "WorldInitializationData.h"

World::~World()
{
	Shutdown();
}

CLECS::ResultType World::InitializeWorld(WorldInitializationData& Data)
{
	if (!CreateWindow(Data.RendererConfig))
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Failed to create window");
		return CLECS::ResultType::Failure;
	}

	if (!CreateRenderer())
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Failed to create renderer");
		return CLECS::ResultType::Failure;
	}

	if (!SDL_SetRenderLogicalPresentation(
		Renderer,
		Data.RendererConfig.WindowWidth,
		Data.RendererConfig.WindowHeight,
		SDL_LOGICAL_PRESENTATION_LETTERBOX
	))
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Failed to set logical presentation: %s", SDL_GetError());
		return CLECS::ResultType::Failure;
	}

	InitializeTextureManager();

	EntityAdminPtr = std::move(Data.EntityAdminPtr);
	Systems = std::move(Data.Systems);

	const auto Context = MakeSystemContext();
	for (const auto& System : Systems)
	{
		System->Initialize(Context);
	}

	return CLECS::ResultType::Success;
}

CLECS::ResultType World::Update(float DeltaTime)
{
	Input.BeginFrame();

	SDL_Event Event;
	while (SDL_PollEvent(&Event))
	{
		if (Event.type == SDL_EVENT_QUIT)
			return CLECS::ResultType::Quit;

		Input.ProcessEvent(Event);
	}

	const auto Context = MakeSystemContext();
	for (const auto& CurrentSystem : Systems)
	{
		CurrentSystem->Update(Context, DeltaTime);
	}

	return CLECS::ResultType::Success;
}

void World::Shutdown()
{
	if (Renderer != nullptr)
	{
		SDL_DestroyRenderer(Renderer);
		Renderer = nullptr;
	}

	if (Window != nullptr)
	{
		SDL_DestroyWindow(Window);
		Window = nullptr;
	}

	if (TextureManagerPtr != nullptr)
	{
		TextureManagerPtr->UnloadAll();
		TextureManagerPtr = nullptr;
	}
}

bool World::CreateWindow(const RendererInitializationData& Data)
{
	Window = SDL_CreateWindow(
		Data.WindowTitle,
		Data.WindowWidth,
		Data.WindowHeight,
		SDL_WINDOW_FULLSCREEN
	);

	if (Window == nullptr)
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Window creation failed: %s", SDL_GetError());
		return false;
	}

	return true;
}

bool World::CreateRenderer()
{
	Renderer = SDL_CreateRenderer(Window, nullptr);

	if (Renderer == nullptr)
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Renderer creation failed: %s", SDL_GetError());
		return false;
	}

	return true;
}

void World::InitializeTextureManager()
{
	TextureManagerPtr = std::make_unique<TextureManager>();
	TextureManagerPtr->Initialize(*Renderer);
}

SystemContext World::MakeSystemContext()
{
	return SystemContext{ *EntityAdminPtr, *Window, *Renderer, Input, EventBus, *TextureManagerPtr };
}