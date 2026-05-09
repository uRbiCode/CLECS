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
#include "TextComponent.h"
#include "Constants.h"

namespace
{
	constexpr const char* TitleText = "ROGUEANOID";
	constexpr const char* PlayButtonText = "Play";
	constexpr const char* TutorialButtonText = "How to Play";
	constexpr const char* QuitButtonText = "Quit";

	void QuitGame()
	{
		SDL_Event Event;
		Event.type = SDL_EVENT_QUIT;
		SDL_PushEvent(&Event);
	}

	void AddTitleText(const SystemContext& Context)
	{
		const auto LogicalPresentation = SDLUtils::GetRendererLogicalPresentation(&Context.Renderer);
		const Vector2D<float> RectSize = { LogicalPresentation.X * 1.f, LogicalPresentation.Y * 0.1f };
		auto& Admin = Context.EntityAdmin;

		const auto TitleTextEntity = Admin.CreateEntity();
		Admin.AddComponent<TransformComponent>(TitleTextEntity, Vector2D<float>{ LogicalPresentation.X * 0.5f, LogicalPresentation.Y * 0.25f });
		Admin.AddComponent<RectComponent>(TitleTextEntity, SDL_FRect{ -RectSize.X * 0.5f, -RectSize.Y * 0.5f, RectSize.X, RectSize.Y });
		Admin.AddComponent<ColorComponent>(TitleTextEntity, SDL_FColor{ 0.f, 0.f, 0.f, 0.f });
		Admin.AddComponent<TextComponent>(TitleTextEntity, TitleText, Constants::FontFilePath, 72);
		Admin.AddComponent<RenderComponent>(TitleTextEntity, RenderConstants::UILayer);
	}

	void AddButtons(const SystemContext& Context)
	{
		const auto LogicalPresentation = SDLUtils::GetRendererLogicalPresentation(&Context.Renderer);
		const Vector2D<float> ButtonSize = { LogicalPresentation.X * 0.4f, LogicalPresentation.Y * 0.1f };
		auto& Admin = Context.EntityAdmin;

		const auto PlayButtonEntity = Admin.CreateEntity();
		Admin.AddComponent<ClickableComponent>(PlayButtonEntity, ClickableTag::PlayButton);
		Admin.AddComponent<TransformComponent>(PlayButtonEntity, Vector2D<float>{ LogicalPresentation.X * 0.5f, LogicalPresentation.Y * 0.50f });
		Admin.AddComponent<RectComponent>(PlayButtonEntity, SDL_FRect{ -ButtonSize.X * 0.5f, -ButtonSize.Y * 0.5f, ButtonSize.X, ButtonSize.Y });
		Admin.AddComponent<ShapeFillComponent>(PlayButtonEntity, false);
		Admin.AddComponent<RenderComponent>(PlayButtonEntity, RenderConstants::UILayer);
		Admin.AddComponent<TextComponent>(PlayButtonEntity, PlayButtonText, Constants::FontFilePath, 32);

		const auto TutorialButtonEntity = Admin.CreateEntity();
		Admin.AddComponent<ClickableComponent>(TutorialButtonEntity, ClickableTag::TutorialButton);
		Admin.AddComponent<TransformComponent>(TutorialButtonEntity, Vector2D<float>{ LogicalPresentation.X * 0.5f, LogicalPresentation.Y * 0.65f });
		Admin.AddComponent<RectComponent>(TutorialButtonEntity, SDL_FRect{ -ButtonSize.X * 0.5f, -ButtonSize.Y * 0.5f, ButtonSize.X, ButtonSize.Y });
		Admin.AddComponent<ShapeFillComponent>(TutorialButtonEntity, false);
		Admin.AddComponent<RenderComponent>(TutorialButtonEntity, RenderConstants::UILayer);
		Admin.AddComponent<TextComponent>(TutorialButtonEntity, TutorialButtonText, Constants::FontFilePath, 32);

		const auto QuitButtonEntity = Admin.CreateEntity();
		Admin.AddComponent<ClickableComponent>(QuitButtonEntity, ClickableTag::QuitButton);
		Admin.AddComponent<TransformComponent>(QuitButtonEntity, Vector2D<float>{ LogicalPresentation.X * 0.5f, LogicalPresentation.Y * 0.80f });
		Admin.AddComponent<RectComponent>(QuitButtonEntity, SDL_FRect{ -ButtonSize.X * 0.5f, -ButtonSize.Y * 0.5f, ButtonSize.X, ButtonSize.Y });
		Admin.AddComponent<ShapeFillComponent>(QuitButtonEntity, false);
		Admin.AddComponent<RenderComponent>(QuitButtonEntity, RenderConstants::UILayer);
		Admin.AddComponent<TextComponent>(QuitButtonEntity, QuitButtonText, Constants::FontFilePath, 32);
	}

	void CleanupMainMenu(const SystemContext& Context)
	{
		Context.EntityAdmin.GetGroup<TextComponent>().ForEach([&](const Entity& Entity, const TextComponent& TextComp)
			{
				Context.EntityAdmin.DestroyEntity(Entity);
			});
	}

	void InitializeMainMenu(const SystemContext& Context)
	{
		AddTitleText(Context);
		AddButtons(Context);
	}

	void OnClickableUsed(const SystemContext& Context, const ClickableUsedEvent& Event)
	{
		switch (Event.UsedClickableTag)
		{
		case ClickableTag::PlayButton:
			GameStateUtils::RequestStateChange(Context, GameState::Run);
			break;
		case ClickableTag::QuitButton:
			QuitGame();
			break;
		case ClickableTag::TutorialButton:
			GameStateUtils::RequestStateChange(Context, GameState::Tutorial);
			break;
		case ClickableTag::Invalid:
		default:
			break;
		}
	}
}

void MainMenuControllerSystem::Initialize(const SystemContext& Context)
{
	const void* const Id = reinterpret_cast<const void*>(&Initialize);

	Context.EventBus.Subscribe<GameStateBeginEvent>(Id, [](const SystemContext& Context, const GameStateBeginEvent& Event)
		{
			if (Event.BeginningState != GameState::MainMenu)
				return;

			InitializeMainMenu(Context);
		});

	Context.EventBus.Subscribe<GameStateEndEvent>(Id, [](const SystemContext& Context, const GameStateEndEvent& Event)
		{
			if (Event.EndingState != GameState::MainMenu)
				return;

			CleanupMainMenu(Context);
		});

	Context.EventBus.Subscribe<ClickableUsedEvent>(Id, [](const SystemContext& Context, const ClickableUsedEvent& Event)
		{
			OnClickableUsed(Context, Event);
		});
}
