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

void GameStateSystem::Initialize(const SystemContext& Context) const
{
	Context.EventBus.Subscribe<RequestGameStateChangeEvent>(this, [this](const SystemContext& Context, const RequestGameStateChangeEvent& Event)
	{
		ChangeGameState(Context, Event.NewState);
	});

	AddBackgroundRenderEntity(Context);
	InitializeCurrentState(Context);
}

void GameStateSystem::InitializeCurrentState(const SystemContext& Context) const
{
	const auto CurrentState = GameStateUtils::GetCurrentGameState(Context);
	if (CurrentState == GameState::Invalid)
		return;

	Context.EventBus.Notify(Context, GameStateBeginEvent{ CurrentState });
}

void GameStateSystem::EndCurrentState(const SystemContext& Context) const
{
	const auto CurrentState = GameStateUtils::GetCurrentGameState(Context);
	if (CurrentState == GameState::Invalid)
		return;

	Context.EventBus.Notify(Context, GameStateEndEvent{ CurrentState });
}

void GameStateSystem::ChangeGameState(const SystemContext& Context, GameState NewState) const
{
	EndCurrentState(Context);

	auto& Admin = Context.EntityAdmin;
	Admin.GetGroup<GameStateComponent>().ForEach([&](const Entity& Entity, GameStateComponent& GameStateComp) {
		GameStateComp.CurrentState = NewState;
	});

	InitializeCurrentState(Context);
}

void GameStateSystem::AddBackgroundRenderEntity(const SystemContext& Context) const
{
	auto& Admin = Context.EntityAdmin;
	
	const auto WindowSize = SDLUtils::GetWindowSize(&Context.Window);

	auto BackgroundEntity = Admin.CreateEntity();
	Admin.AddComponent<TransformComponent>(BackgroundEntity);
	Admin.AddComponent<RectComponent>(BackgroundEntity, SDL_FRect{ 
		0.f, 
		0.f, 
		static_cast<float>(WindowSize.X), 
		static_cast<float>(WindowSize.Y)
	});
	Admin.AddComponent<RenderComponent>(BackgroundEntity, RenderConstants::BackgroundLayer);
	
	const auto Texture = Context.Managers.TextureManager.LoadTexture("../Assets/Textures/Background_Tiles.png");
	if (Texture == nullptr)
		return;

	Admin.AddComponent<TextureComponent>(BackgroundEntity, TextureComponent{ Texture, {34.f, 33.f, 60.f, 42.f} });
}