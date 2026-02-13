#include "TutorialControllerSystem.h"
#include "SystemContext.h"
#include "EventBus.h"
#include "ClickableUsedEvent.h"
#include "EntityAdmin.h"
#include "GameStateEvents.h"
#include "TextComponent.h"
#include <SDLUtils.h>
#include <RenderSystem.h>
#include <ShapeComponents.h>
#include <RenderComponent.h>
#include "Constants.h"
#include <RenderConstants.h>
#include "GameStateUtils.h"
#include "TransformComponent.h"

namespace
{
	constexpr const char* Title = "HOW TO PLAY";
	constexpr const char* TextLine1 = "Bounce the ball to break all the bricks.";
	constexpr const char* TextLine2 = "Move the paddle with A and D or arrow keys.";
	constexpr const char* TextLine3 = "Amass infinite power with upgrades!";
	constexpr const char* TextLine4 = "Have fun!";
}

void TutorialControllerSystem::Initialize(const SystemContext& Context) const
{
	Context.EventBus.Subscribe<GameStateBeginEvent>(this, [this](const SystemContext& Context, const GameStateBeginEvent& Event)
	{
		if (Event.BeginningState == GameState::Tutorial)
		{
			AddTutorialText(Context);
			AddMainMenuButton(Context);
			SubscribeToClickableUsedEvent(Context);
		}
	});

	Context.EventBus.Subscribe<GameStateEndEvent>(this, [this](const SystemContext& Context, const GameStateEndEvent& Event)
	{
		if (Event.EndingState == GameState::Tutorial)
		{
			CleanupTutorial(Context);
		}
	});
}

void TutorialControllerSystem::AddTutorialText(const SystemContext& Context) const
{
	const auto LogicalPresentation = SDLUtils::GetRendererLogicalPresentation(&Context.Renderer);
	const Vector2D<float> TitleRectSize = { LogicalPresentation.X * 1.f, LogicalPresentation.Y * 0.1f };

	auto& Admin = Context.EntityAdmin;
	auto TitleTextEntity = Admin.CreateEntity();
	Admin.AddComponent<TransformComponent>(TitleTextEntity, Vector2D<float>{ LogicalPresentation.X * 0.5f, LogicalPresentation.Y * 0.25f });
	Admin.AddComponent<RectComponent>(TitleTextEntity, SDL_FRect{ -TitleRectSize.X * 0.5f, -TitleRectSize.Y * 0.5f, TitleRectSize.X, TitleRectSize.Y });
	Admin.AddComponent<ColorComponent>(TitleTextEntity, SDL_FColor{0.f, 0.f, 0.f, 0.f});
	Admin.AddComponent<TextComponent>(TitleTextEntity, Title, Constants::FontFilePath, 72);
	Admin.AddComponent<RenderComponent>(TitleTextEntity, RenderConstants::UILayer);

	// Looks awkward but does its job for the centered text visuals. I'm taking the blame
	// Text Line 1
	const Vector2D<float> TextRectSize = { LogicalPresentation.X * 1.f, LogicalPresentation.Y * 0.08f };
	auto TextLine1Entity = Admin.CreateEntity();
	Admin.AddComponent<TransformComponent>(TextLine1Entity, Vector2D<float>{ LogicalPresentation.X * 0.5f, LogicalPresentation.Y * 0.42f });
	Admin.AddComponent<RectComponent>(TextLine1Entity, SDL_FRect{ -TextRectSize.X * 0.5f, -TextRectSize.Y * 0.5f, TextRectSize.X, TextRectSize.Y });
	Admin.AddComponent<ColorComponent>(TextLine1Entity, SDL_FColor{0.f, 0.f, 0.f, 0.f});
	Admin.AddComponent<TextComponent>(TextLine1Entity, TextLine1, Constants::FontFilePath, 24);
	Admin.AddComponent<RenderComponent>(TextLine1Entity, RenderConstants::UILayer);

	// Text Line 2
	auto TextLine2Entity = Admin.CreateEntity();
	Admin.AddComponent<TransformComponent>(TextLine2Entity, Vector2D<float>{ LogicalPresentation.X * 0.5f, LogicalPresentation.Y * 0.52f });
	Admin.AddComponent<RectComponent>(TextLine2Entity, SDL_FRect{ -TextRectSize.X * 0.5f, -TextRectSize.Y * 0.5f, TextRectSize.X, TextRectSize.Y });
	Admin.AddComponent<ColorComponent>(TextLine2Entity, SDL_FColor{0.f, 0.f, 0.f, 0.f});
	Admin.AddComponent<TextComponent>(TextLine2Entity, TextLine2, Constants::FontFilePath, 24);
	Admin.AddComponent<RenderComponent>(TextLine2Entity, RenderConstants::UILayer);

	// Text Line 3
	auto TextLine3Entity = Admin.CreateEntity();
	Admin.AddComponent<TransformComponent>(TextLine3Entity, Vector2D<float>{ LogicalPresentation.X * 0.5f, LogicalPresentation.Y * 0.62f });
	Admin.AddComponent<RectComponent>(TextLine3Entity, SDL_FRect{ -TextRectSize.X * 0.5f, -TextRectSize.Y * 0.5f, TextRectSize.X, TextRectSize.Y });
	Admin.AddComponent<ColorComponent>(TextLine3Entity, SDL_FColor{0.f, 0.f, 0.f, 0.f});
	Admin.AddComponent<TextComponent>(TextLine3Entity, TextLine3, Constants::FontFilePath, 24);
	Admin.AddComponent<RenderComponent>(TextLine3Entity, RenderConstants::UILayer);

	// Text Line 4
	auto TextLine4Entity = Admin.CreateEntity();
	Admin.AddComponent<TransformComponent>(TextLine4Entity, Vector2D<float>{ LogicalPresentation.X * 0.5f, LogicalPresentation.Y * 0.72f });
	Admin.AddComponent<RectComponent>(TextLine4Entity, SDL_FRect{ -TextRectSize.X * 0.5f, -TextRectSize.Y * 0.5f, TextRectSize.X, TextRectSize.Y });
	Admin.AddComponent<ColorComponent>(TextLine4Entity, SDL_FColor{0.f, 0.f, 0.f, 0.f});
	Admin.AddComponent<TextComponent>(TextLine4Entity, TextLine4, Constants::FontFilePath, 24);
	Admin.AddComponent<RenderComponent>(TextLine4Entity, RenderConstants::UILayer);
}

void TutorialControllerSystem::AddMainMenuButton(const SystemContext& Context) const
{
	const auto LogicalPresentation = SDLUtils::GetRendererLogicalPresentation(&Context.Renderer);
	const Vector2D<float> ButtonSize = { LogicalPresentation.X * 0.25f, LogicalPresentation.Y * 0.1f };

	auto& Admin = Context.EntityAdmin;
	auto MainMenuButtonEntity = Admin.CreateEntity();
	Admin.AddComponent<ClickableComponent>(MainMenuButtonEntity, ClickableTag::MainMenuButton);
	Admin.AddComponent<TransformComponent>(MainMenuButtonEntity, Vector2D<float>{ LogicalPresentation.X * 0.5f, LogicalPresentation.Y * 0.85f });
	Admin.AddComponent<RectComponent>(MainMenuButtonEntity, SDL_FRect{ -ButtonSize.X * 0.5f, -ButtonSize.Y * 0.5f, ButtonSize.X, ButtonSize.Y });
	Admin.AddComponent<ShapeFillComponent>(MainMenuButtonEntity, false);
	Admin.AddComponent<RenderComponent>(MainMenuButtonEntity, RenderConstants::UILayer);
	Admin.AddComponent<TextComponent>(MainMenuButtonEntity, Constants::MainMenuButtonText, Constants::FontFilePath, 24);
}

void TutorialControllerSystem::CleanupTutorial(const SystemContext& Context) const
{
	Context.EntityAdmin.GetGroup<TextComponent>().ForEach([&](const Entity& Entity, const TextComponent& TextComp)
	{
		Context.EntityAdmin.DestroyEntity(Entity);
	});
}

void TutorialControllerSystem::OnClickableUsed(const SystemContext& Context, const ClickableUsedEvent& Event) const
{
	if (Event.UsedClickableTag != ClickableTag::MainMenuButton)
		return;

	Context.EventBus.Unsubscribe<ClickableUsedEvent>(this);
	CleanupTutorial(Context);
	GameStateUtils::RequestStateChange(Context, GameState::MainMenu);
}

void TutorialControllerSystem::SubscribeToClickableUsedEvent(const SystemContext& Context) const
{
	Context.EventBus.Subscribe<ClickableUsedEvent>(this, [this](const SystemContext& Context, const ClickableUsedEvent& Event)
	{
		OnClickableUsed(Context, Event);
	});
}
