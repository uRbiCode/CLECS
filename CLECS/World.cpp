#include "World.h"
#include "SystemContext.h"
#include "WorldInitializationData.h"
#include "MouseClickEvent.h"
#include "QueryContext.h"

World::~World()
{
	Shutdown();
}

CLECS::ResultType World::InitializeRenderer(RendererInitializationData&& RendererData)
{
	if (!CreateWindow(RendererData))
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "World::InitializeRenderer -> Failed to create window");
		return CLECS::ResultType::Failure;
	}

	if (!CreateRenderer())
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "World::InitializeRenderer -> Failed to create renderer");
		return CLECS::ResultType::Failure;
	}

	if (!SDL_SetRenderLogicalPresentation(Renderer, RendererData.WindowWidth, RendererData.WindowHeight, SDL_LOGICAL_PRESENTATION_LETTERBOX))
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "World::InitializeRenderer -> Failed to set logical presentation: %s", SDL_GetError());
		return CLECS::ResultType::Failure;
	}

	return CLECS::ResultType::Success;
}

CLECS::ResultType World::InitializeWorld(WorldInitializationData&& Data)
{
	InitializeTextureManager();
	InitializeAudioManager();
	InitializeFontManager();

	Archetypes = ArchetypeStorage::Create(std::move(ComponentTypesCollection::Create(std::move(Data.ComponentsData))));
	Systems = SystemsCollection::Create(std::move(Data.SystemsData));

	auto Context = MakeSystemContext();
	for (const StartupSystemDescriptor& System : Data.StartupSystemsData.GetRegisteredSystems())
	{
		System.Initialize(Context);
	}

	FlushCommands();

	for (const StartupSystemDescriptor& LateSystem : Data.StartupSystemsData.GetRegisteredLateSystems())
	{
		LateSystem.Initialize(Context);
	}

	FlushCommands();

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

	SystemContext Context = MakeSystemContext();

	for (const auto& [Phase, StagedSystems] : Systems.GetStagedSystems())
	{
		for (const SystemDescriptor::UpdateFunction& Update : StagedSystems)
		{
			Update(Context, DeltaTime);
		}

		FlushCommands();
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
	}

	if (AudioManagerPtr != nullptr)
	{
		AudioManagerPtr->Shutdown();
	}
	
	if (FontManagerPtr != nullptr)
	{
		FontManagerPtr->UnloadAll();
	}
}

bool World::CreateWindow(const RendererInitializationData& Data)
{
	Window = SDL_CreateWindow(Data.WindowTitle,	Data.WindowWidth, Data.WindowHeight, SDL_WINDOW_FULLSCREEN);

	if (Window == nullptr)
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "World::CreateWindow -> Window creation failed: %s", SDL_GetError());
		return false;
	}

	return true;
}

bool World::CreateRenderer()
{
	Renderer = SDL_CreateRenderer(Window, nullptr);

	if (Renderer == nullptr)
	{
		SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "World::CreateRenderer -> Renderer creation failed: %s", SDL_GetError());
		return false;
	}

	return true;
}

void World::InitializeTextureManager()
{
	TextureManagerPtr = std::make_unique<TextureManager>();
	TextureManagerPtr->Initialize(*Renderer);
}

void World::InitializeAudioManager()
{
	AudioManagerPtr = std::make_unique<AudioManager>();
	AudioManagerPtr->Initialize();
}

void World::InitializeFontManager()
{
	FontManagerPtr = std::make_unique<FontManager>();
	FontManagerPtr->Initialize();
}

SystemContext World::MakeSystemContext()
{
	const Managers Managers{ *TextureManagerPtr, *AudioManagerPtr, *FontManagerPtr };
	return SystemContext{ QueryContext::Create(&Archetypes), Commands, *Window, *Renderer, Input, EventBus, Managers };
}

void World::FlushCommands()
{
	Commands.Flush(Archetypes);
}