#include "SummaryControllerSystem.h"
#include "SystemContext.h"
#include "EventBus.h"
#include "GameStateEvents.h"
#include "EntityAdmin.h"
#include "TextComponent.h"
#include "SDLUtils.h"
#include "ClickableComponent.h"
#include "TransformComponent.h"
#include "ShapeComponents.h"
#include "RenderComponent.h"
#include "RenderConstants.h"
#include "Constants.h"
#include "ClickableUsedEvent.h"
#include "GameStateUtils.h"
#include <string>

namespace
{
	constexpr const char* VictoryText = "VICTORY";
	constexpr const char* DefeatText = "DEFEAT";

	void CleanupSummary(const SystemContext& Context)
	{
		Context.EntityAdmin.GetGroup<TextComponent>().ForEach([&](const Entity& Entity, const TextComponent& TextComp)
		{
			Context.EntityAdmin.DestroyEntity(Entity);
		});
	}

	void AddSummaryText(const SystemContext& Context, const std::string& Text)
	{
		const auto LogicalPresentation = SDLUtils::GetRendererLogicalPresentation(&Context.Renderer);
		const Vector2D<float> RectSize = { LogicalPresentation.X * 1.f, LogicalPresentation.Y * 0.1f };

		auto& Admin = Context.EntityAdmin;
		const auto SummaryTextEntity = Admin.CreateEntity();
		Admin.AddComponent<TransformComponent>(SummaryTextEntity, Vector2D<float>{ LogicalPresentation.X * 0.5f, LogicalPresentation.Y * 0.25f });
		Admin.AddComponent<RectComponent>(SummaryTextEntity, SDL_FRect{ -RectSize.X * 0.5f, -RectSize.Y * 0.5f, RectSize.X, RectSize.Y });
		Admin.AddComponent<ColorComponent>(SummaryTextEntity, SDL_FColor{ 0.f, 0.f, 0.f, 0.f });
		Admin.AddComponent<TextComponent>(SummaryTextEntity, Text, Constants::FontFilePath, 72);
		Admin.AddComponent<RenderComponent>(SummaryTextEntity, RenderConstants::UILayer);
	}

	void AddMainMenuButton(const SystemContext& Context)
	{
		const auto LogicalPresentation       = SDLUtils::GetRendererLogicalPresentation(&Context.Renderer);
		const Vector2D<float> ButtonSize     = { LogicalPresentation.X * 0.25f, LogicalPresentation.Y * 0.1f };

		auto& Admin = Context.EntityAdmin;
		const auto MainMenuButtonEntity = Admin.CreateEntity();
		Admin.AddComponent<ClickableComponent>(MainMenuButtonEntity, ClickableTag::MainMenuButton);
		Admin.AddComponent<TransformComponent>(MainMenuButtonEntity, Vector2D<float>{ LogicalPresentation.X * 0.5f, LogicalPresentation.Y * 0.75f });
		Admin.AddComponent<RectComponent>(MainMenuButtonEntity, SDL_FRect{ -ButtonSize.X * 0.5f, -ButtonSize.Y * 0.5f, ButtonSize.X, ButtonSize.Y });
		Admin.AddComponent<ShapeFillComponent>(MainMenuButtonEntity, false);
		Admin.AddComponent<RenderComponent>(MainMenuButtonEntity, RenderConstants::UILayer);
		Admin.AddComponent<TextComponent>(MainMenuButtonEntity, Constants::MainMenuButtonText, Constants::FontFilePath, 24);
	}

	void OnClickableUsed(const SystemContext& Context, const ClickableUsedEvent& Event)
	{
		if (Event.UsedClickableTag != ClickableTag::MainMenuButton)
			return;

		const void* const Id = reinterpret_cast<const void*>(&SummaryControllerSystem::Initialize);
		Context.EventBus.Unsubscribe<ClickableUsedEvent>(Id);
		CleanupSummary(Context);
		GameStateUtils::RequestStateChange(Context, GameState::MainMenu);
	}

	void SubscribeToClickableUsedEvent(const SystemContext& Context)
	{
		const void* const Id = reinterpret_cast<const void*>(&SummaryControllerSystem::Initialize);
		Context.EventBus.Subscribe<ClickableUsedEvent>(Id, [](const SystemContext& Context, const ClickableUsedEvent& Event)
		{
			OnClickableUsed(Context, Event);
		});
	}

	void InitializeVictory(const SystemContext& Context)
	{
		AddMainMenuButton(Context);
		AddSummaryText(Context, VictoryText);
	}

	void InitializeDefeat(const SystemContext& Context)
	{
		AddMainMenuButton(Context);
		AddSummaryText(Context, DefeatText);
	}
}

void SummaryControllerSystem::Initialize(const SystemContext& Context)
{
	const void* const Id = reinterpret_cast<const void*>(&Initialize);

	Context.EventBus.Subscribe<GameStateBeginEvent>(Id, [](const SystemContext& Context, const GameStateBeginEvent& Event)
	{
		if (Event.BeginningState == GameState::Victory)
		{
			InitializeVictory(Context);
			SubscribeToClickableUsedEvent(Context);
		}
		else if (Event.BeginningState == GameState::Defeat)
		{
			InitializeDefeat(Context);
			SubscribeToClickableUsedEvent(Context);
		}
	});

	Context.EventBus.Subscribe<GameStateEndEvent>(Id, [](const SystemContext& Context, const GameStateEndEvent& Event)
	{
		if (Event.EndingState == GameState::Victory || Event.EndingState == GameState::Defeat)
		{
			CleanupSummary(Context);
		}
	});
}