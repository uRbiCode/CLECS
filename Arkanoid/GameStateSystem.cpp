#include "GameStateSystem.h"
#include "SystemContext.h"
#include "EntityAdmin.h"
#include "GameStateComponent.h"
#include "EventBus.h"
#include "GameStateEvents.h"
#include "TextureManager.h"
#include "TransformComponent.h"
#include "ShapeComponents.h"
#include "RenderConstants.h"
#include "RenderComponent.h"
#include "TextureComponent.h"
#include "SDLUtils.h"
#include "GameStateUtils.h"
#include <cassert>

namespace
{
	constexpr const char* BackgroundTexturePath = "../Assets/Textures/Background_Tiles.png";

	void InitializeCurrentState(const SystemContext& Context)
	{
		const auto CurrentState = GameStateUtils::GetCurrentGameState(Context);
		if (CurrentState == GameState::Invalid)
			return;

		Context.EventBus.Notify(Context, GameStateBeginEvent{ CurrentState });
	}

	void EndCurrentState(const SystemContext& Context)
	{
		const auto CurrentState = GameStateUtils::GetCurrentGameState(Context);
		if (CurrentState == GameState::Invalid)
			return;

		Context.EventBus.Notify(Context, GameStateEndEvent{ CurrentState });
	}

	void ChangeGameState(const SystemContext& Context, GameState NewState)
	{
		EndCurrentState(Context);

		auto& Admin = Context.EntityAdmin;
		const auto GameStateGroup = Admin.GetGroup<GameStateComponent>();
		assert(GameStateGroup.Size() == 1 && "Expected exactly one GameStateComponent in the world");
		if (GameStateGroup.Empty())
			return;

		Admin.AccessComponent<GameStateComponent>(GameStateGroup[0]).CurrentState = NewState;
		InitializeCurrentState(Context);
	}

	void AddBackgroundRenderEntity(const SystemContext& Context)
	{
		auto& Admin = Context.EntityAdmin;

		const auto RendererLogicalPresentation = SDLUtils::GetRendererLogicalPresentation(&Context.Renderer);

		const auto BackgroundEntity = Admin.CreateEntity();
		Admin.AddComponent<TransformComponent>(BackgroundEntity);
		Admin.AddComponent<RectComponent>(BackgroundEntity, SDL_FRect{
			0.f,
			0.f,
			static_cast<float>(RendererLogicalPresentation.X),
			static_cast<float>(RendererLogicalPresentation.Y)
		});
		Admin.AddComponent<RenderComponent>(BackgroundEntity, RenderConstants::BackgroundLayer);

		const auto Texture = Context.Managers.TextureManager.LoadTexture(BackgroundTexturePath);
		if (Texture == nullptr)
			return;

		Admin.AddComponent<TextureComponent>(BackgroundEntity, TextureComponent{ Texture, { 33.f, 23.f, 25.f, 20.f } });
	}
}

void GameStateSystem::Initialize(const SystemContext& Context)
{
	const void* const Id = reinterpret_cast<const void*>(&Initialize);

	Context.EventBus.Subscribe<RequestGameStateChangeEvent>(Id, [](const SystemContext& Context, const RequestGameStateChangeEvent& Event)
	{
		ChangeGameState(Context, Event.NewState);
	});

	const auto GameStateEntity = Context.EntityAdmin.CreateEntity();
	Context.EntityAdmin.AddComponent<GameStateComponent>(GameStateEntity, GameState::MainMenu);

	AddBackgroundRenderEntity(Context);
	InitializeCurrentState(Context);
}