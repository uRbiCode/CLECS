#include "MainMenuControllerSystem.h"
#include "GameStateEvents.h"
#include "SystemContext.h"
#include "EventBus.h"
#include "EntityAdmin.h"
#include "ClickableComponent.h"
#include "SDLUtils.h"
#include "TransformComponent.h"
#include "ShapeComponents.h"
#include "RenderComponent.h"
#include "RenderConstants.h"
#include "ClickableUsedEvent.h"
#include "GameStateUtils.h"

void MainMenuControllerSystem::Initialize(const SystemContext& Context) const
{
	Context.EventBus.Subscribe<GameStateBeginEvent>(this, [this](const SystemContext& Context, const GameStateBeginEvent& Event)
	{
		if (Event.BeginningState != GameState::MainMenu)
			return;

		InitializeMainMenu(Context);
	});

	Context.EventBus.Subscribe<GameStateEndEvent>(this, [this](const SystemContext& Context, const GameStateEndEvent& Event)
	{
		if (Event.EndingState != GameState::MainMenu)
			return;

		CleanupMainMenu(Context);
	});

	Context.EventBus.Subscribe<ClickableUsedEvent>(this, [this](const SystemContext& Context, const ClickableUsedEvent& Event)
	{
		OnClickableUsed(Context, Event);
	});
}

void MainMenuControllerSystem::InitializeMainMenu(const SystemContext& Context) const
{
	auto LogicalPresentation = SDLUtils::GetRendererLogicalPresentation(&Context.Renderer);

	const Vector2D<float> ButtonSize = { LogicalPresentation.X * 0.25f, LogicalPresentation.Y * 0.1f };

	auto& Admin = Context.EntityAdmin;
	auto PlayButtonEntity = Admin.CreateEntity();
	Admin.AddComponent<ClickableComponent>(PlayButtonEntity, ClickableTag::PlayButton);
	Admin.AddComponent<TransformComponent>(PlayButtonEntity, Vector2D<float>{ LogicalPresentation.X * 0.5f, LogicalPresentation.Y * 0.45f });
	Admin.AddComponent<RectComponent>(PlayButtonEntity, SDL_FRect{ -ButtonSize.X * 0.5f, -ButtonSize.Y * 0.5f, ButtonSize.X, ButtonSize.Y });
	Admin.AddComponent<ShapeFillComponent>(PlayButtonEntity, false);
	Admin.AddComponent<RenderComponent>(PlayButtonEntity, RenderConstants::UILayer);

	auto QuitButtonEntity = Admin.CreateEntity();
	Admin.AddComponent<ClickableComponent>(QuitButtonEntity, ClickableTag::QuitButton);
	Admin.AddComponent<TransformComponent>(QuitButtonEntity, Vector2D<float>{ LogicalPresentation.X * 0.5f, LogicalPresentation.Y * 0.6f });
	Admin.AddComponent<RectComponent>(QuitButtonEntity, SDL_FRect{ -ButtonSize.X * 0.5f, -ButtonSize.Y * 0.5f, ButtonSize.X, ButtonSize.Y });
	Admin.AddComponent<ShapeFillComponent>(QuitButtonEntity, false);
	Admin.AddComponent<RenderComponent>(QuitButtonEntity, RenderConstants::UILayer);
}

void MainMenuControllerSystem::CleanupMainMenu(const SystemContext& Context) const
{
	Context.EntityAdmin.GetGroup<ClickableComponent>().ForEach([&](const Entity& Entity, const ClickableComponent& ClickableComp) 
	{
		Context.EntityAdmin.DestroyEntity(Entity);
	});
}

void MainMenuControllerSystem::OnClickableUsed(const SystemContext& Context, const ClickableUsedEvent& Event) const
{
	switch (Event.UsedClickableTag)
	{
		case ClickableTag::PlayButton:
			GameStateUtils::RequestStateChange(Context, GameState::Run);
			break;
		case ClickableTag::QuitButton:
			QuitGame();
			break;
		case ClickableTag::Invalid:
		default:
			break;
	}
}

void MainMenuControllerSystem::QuitGame() const
{
	SDL_Event event;
	event.type = SDL_EVENT_QUIT;
	SDL_PushEvent(&event);
}