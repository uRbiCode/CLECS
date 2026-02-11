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
#include <cassert>

void GameStateSystem::Initialize(const SystemContext& Context) const
{
	AddBackgroundRenderEntity(Context);
	InitializeCurrentState(Context);
}

void GameStateSystem::InitializeCurrentState(const SystemContext& Context) const
{
	auto& Admin = Context.EntityAdmin;
	auto GameStateGroup = Admin.GetGroup<GameStateComponent>();
	assert(GameStateGroup.Size() == 1 && "Expected exactly one GameStateComponent in the world");

	if (GameStateGroup.Empty())
		return;

	const auto& GameState = GameStateGroup.Get<GameStateComponent>(GameStateGroup[0]).CurrentState;
	Context.EventBus.Notify<GameStateBeginEvent>(Context, GameStateBeginEvent{ GameState });
}

void GameStateSystem::AddBackgroundRenderEntity(const SystemContext& Context) const
{
	auto& Admin = Context.EntityAdmin;
	
	int WindowWidth, WindowHeight;
	SDL_GetWindowSize(&Context.Window, &WindowWidth, &WindowHeight);

	auto BackgroundEntity = Admin.CreateEntity();
	Admin.AddComponent<TransformComponent>(BackgroundEntity);
	Admin.AddComponent<RectComponent>(BackgroundEntity, SDL_FRect{ 
		0.f, 
		0.f, 
		static_cast<float>(WindowWidth), 
		static_cast<float>(WindowHeight)
	});
	Admin.AddComponent<RenderComponent>(BackgroundEntity, RenderConstants::BackgroundLayer);
	
	auto Texture = Context.TextureManager.LoadTexture("../Assets/Textures/Background_Tiles.png");
	if (Texture == nullptr)
		return;

	Admin.AddComponent<TextureComponent>(BackgroundEntity, TextureComponent{ Texture, {34.f, 13.f, 60.f, 42.f} });
}