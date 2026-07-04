#include "TransitionUtils.h"
#include "SystemContext.h"
#include "SDLUtils.h"
#include "AddEntitiesCommand.h"
#include "PositionComponent.h"
#include "ShapeComponents.h"
#include "RenderComponents.h"
#include "TextComponent.h"
#include "CommandRunner.h"
#include "AudioRequestComponents.h"
#include "ClickableComponent.h"
#include "GlobalConstants.h"
#include "RunUtils.h"
#include "StageDataLoader.h"
#include "ComponentUtils.h"
#include "CollisionUtils.h"
#include "HealthComponent.h"
#include "UIBlinkComponent.h"

namespace
{
	namespace TransitionConstants
	{
		constexpr const char* MainMenuButtonText = "Main Menu";
		constexpr const char* PrepareMessageText = "PRESS SPACE TO START";
		constexpr float PrepareMessageDisplayDuration = 0.75f;
	}

	namespace Tutorial
	{
		namespace Constants
		{
			constexpr const char* Title = "HOW TO PLAY";
			constexpr const char* TextLines[] = {
				"Bounce the ball to break all the bricks.",
				"Move the paddle with A and D or arrow keys.",
				"Amass infinite power with upgrades!",
				"Have fun!"
			};
			constexpr float ButtonFontSize = 24.f;
			constexpr float TitleFontSize = 72.f;
		}

		void AddTutorialText(SystemContext& Context)
		{
			const Vector2D<int> LogicalPresentation = SDLUtils::GetRendererLogicalPresentation(&Context.Renderer);
			const Vector2D<float> TitleRectSize = { LogicalPresentation.X * 1.f, LogicalPresentation.Y * 0.1f };

			AddEntitiesCommand<PositionComponent, RectComponent, UIRenderComponent, TextComponent> AddTitleTextCommand(5);
			AddTitleTextCommand.WithEntry(PositionComponent{ {LogicalPresentation.X * 0.5f, LogicalPresentation.Y * 0.25f} },
				RectComponent{ SDL_FRect{ -TitleRectSize.X * 0.5f, -TitleRectSize.Y * 0.5f, TitleRectSize.X, TitleRectSize.Y } },
				UIRenderComponent{ SDL_FColor{ 1.f, 1.f, 1.f, 1.f } },
				TextComponent{ Constants::Title, GlobalConstants::FontFilePath, Constants::TitleFontSize });
			Context.Commands.Submit(std::move(AddTitleTextCommand));

			for (size_t i = 0; i < std::size(Constants::TextLines); ++i)
			{
				AddTitleTextCommand.WithEntry(PositionComponent{ {LogicalPresentation.X * 0.5f, LogicalPresentation.Y * (0.42f + i * 0.1f)} },
					RectComponent{ SDL_FRect{ -TitleRectSize.X * 0.5f, -TitleRectSize.Y * 0.5f, TitleRectSize.X, TitleRectSize.Y } },
					UIRenderComponent{ SDL_FColor{ 1.f, 1.f, 1.f, 1.f } },
					TextComponent{ Constants::TextLines[i], GlobalConstants::FontFilePath, Constants::ButtonFontSize });
			}

			Context.Commands.Submit(std::move(AddTitleTextCommand));
		}

		void AddTutorialControls(SystemContext& Context)
		{
			const Vector2D<int> LogicalPresentation = SDLUtils::GetRendererLogicalPresentation(&Context.Renderer);
			const Vector2D<float> ButtonSize = { LogicalPresentation.X * 0.25f, LogicalPresentation.Y * 0.1f };

			AddEntitiesCommand<PositionComponent, RectComponent, UIRenderComponent, TextComponent, ClickableComponent> AddButtonCommand(1);
			AddButtonCommand.WithEntry(PositionComponent{ {LogicalPresentation.X * 0.5f, LogicalPresentation.Y * 0.85f} },
				RectComponent{ SDL_FRect{ -ButtonSize.X * 0.5f, -ButtonSize.Y * 0.5f, ButtonSize.X, ButtonSize.Y } },
				UIRenderComponent{ SDL_FColor{ 1.f, 1.f, 1.f, 1.f } },
				TextComponent{ TransitionConstants::MainMenuButtonText, GlobalConstants::FontFilePath, Constants::ButtonFontSize },
				ClickableComponent{ ClickableTag::MainMenuButton });

			Context.Commands.Submit(std::move(AddButtonCommand));
		}
	}

	namespace MainMenu
	{
		namespace Constants
		{
			constexpr const char* TitleText = "ROGUEANOID";
			constexpr const char* ButtonsTexts[] = {
				"Play",
				"How to Play",
				"Quit"
			};
			constexpr float TitleFontSize = 72.f;

			constexpr const char* MainMenuMusicName = "main_menu_loop";
		}

		void PlayBackgroundMusic(SystemContext& Context)
		{
			AddEntitiesCommand<MusicRequestComponent> AddMusicRequestCommand(1);
			AddMusicRequestCommand.WithEntry(MusicRequestComponent{ MainMenu::Constants::MainMenuMusicName, 0.5f });
		}

		void AddTitleText(SystemContext& Context)
		{

			const Vector2D<int> LogicalPresentation = SDLUtils::GetRendererLogicalPresentation(&Context.Renderer);
			const Vector2D<float> RectSize = { LogicalPresentation.X * 1.f, LogicalPresentation.Y * 0.1f };

			AddEntitiesCommand<PositionComponent, RectComponent, UIRenderComponent, TextComponent> AddTitleTextCommand(1);
			AddTitleTextCommand.WithEntry(PositionComponent{ {LogicalPresentation.X * 0.5f, LogicalPresentation.Y * 0.25f} },
				RectComponent{ SDL_FRect{ -RectSize.X * 0.5f, -RectSize.Y * 0.5f, RectSize.X, RectSize.Y } },
				UIRenderComponent{ SDL_FColor{ 1.f, 1.f, 1.f, 1.f } },
				TextComponent{ MainMenu::Constants::TitleText, GlobalConstants::FontFilePath, MainMenu::Constants::TitleFontSize });

			Context.Commands.Submit(std::move(AddTitleTextCommand));
		}

		void AddMainMenuControls(SystemContext& Context)
		{
			const Vector2D<int> LogicalPresentation = SDLUtils::GetRendererLogicalPresentation(&Context.Renderer);
			const Vector2D<float> ButtonSize = { LogicalPresentation.X * 0.4f, LogicalPresentation.Y * 0.1f };
			AddEntitiesCommand<PositionComponent, RectComponent, UIRenderComponent, TextComponent> AddButtonsCommand(3);

			for (size_t i = 0; i < std::size(MainMenu::Constants::ButtonsTexts); ++i)
			{
				AddButtonsCommand.WithEntry(PositionComponent{ {LogicalPresentation.X * 0.5f, LogicalPresentation.Y * (0.5f + i * 0.15f)} },
					RectComponent{ SDL_FRect{ -ButtonSize.X * 0.5f, -ButtonSize.Y * 0.5f, ButtonSize.X, ButtonSize.Y } },
					UIRenderComponent{ SDL_FColor{ 1.f, 1.f, 1.f, 1.f } },
					TextComponent{ MainMenu::Constants::ButtonsTexts[i], GlobalConstants::FontFilePath, 32.f });
			}

			Context.Commands.Submit(std::move(AddButtonsCommand));
		}
	}

	namespace Run
	{
		namespace Constants
		{
			constexpr const char* StageText = "STAGE ";
		}

		std::string BuildStageText(SystemContext& Context)
		{
			const int StageNumber = RunUtils::GetCurrentStageNumber(Context);
			return Constants::StageText + std::to_string(StageNumber);
		}

		void StopBackgroundMusic(SystemContext& Context)
		{
			AddEntitiesCommand<StopMusicComponent> AddStopMusicCommand(1);
			AddStopMusicCommand.WithEntry(StopMusicComponent{});
			Context.Commands.Submit(std::move(AddStopMusicCommand));
		}

		void InitializeStageInfo(SystemContext& Context)
		{
			const Vector2D<int> LogicalPresentation = SDLUtils::GetRendererLogicalPresentation(&Context.Renderer);
			const Vector2D<float> RectSize = { LogicalPresentation.X * 0.25f, LogicalPresentation.Y * 0.1f };

			AddEntitiesCommand<PositionComponent, RectComponent, UIRenderComponent, TextComponent> AddStageInfoCommand(1);
			AddStageInfoCommand.WithEntry(PositionComponent{ {LogicalPresentation.X * 0.8f, LogicalPresentation.Y * 0.95f} },
				RectComponent{ SDL_FRect{ -RectSize.X * 0.5f, -RectSize.Y * 0.5f, RectSize.X, RectSize.Y } },
				UIRenderComponent{ SDL_FColor{ 1.f, 1.f, 1.f, 1.f } },
				TextComponent{ Run::BuildStageText(Context), GlobalConstants::FontFilePath, 24.f });

			Context.Commands.Submit(std::move(AddStageInfoCommand));
		}

		void InitializeStageEntities(SystemContext& Context)
		{
			StageData StageData = StageDataLoader::LoadStageDataByNumber(RunUtils::GetCurrentStageNumber(Context));
			RunUtils::SpawnWalls(Context, std::move(StageData.Walls));
			RunUtils::SpawnBricks(Context, std::move(StageData.Bricks));
			RunUtils::SpawnTrigger(Context, std::move(StageData.Trigger));
			RunUtils::SpawnPlayer(Context, std::move(StageData.PlayerData));
			RunUtils::SpawnBall(Context, std::move(StageData.BallData));
		}
	}

	namespace Summary
	{
		void AddSummaryText(SystemContext& Context, const std::string& Message)
		{
			const Vector2D<int> LogicalPresentation = SDLUtils::GetRendererLogicalPresentation(&Context.Renderer);
			const Vector2D<float> RectSize = { LogicalPresentation.X * 1.f, LogicalPresentation.Y * 0.1f };

			AddEntitiesCommand<PositionComponent, RectComponent, UIRenderComponent, TextComponent> AddSummaryTextCommand(1);
			AddSummaryTextCommand.WithEntry(PositionComponent{ {LogicalPresentation.X * 0.5f, LogicalPresentation.Y * 0.25f} },
				RectComponent{ SDL_FRect{ -RectSize.X * 0.5f, -RectSize.Y * 0.5f, RectSize.X, RectSize.Y } },
				UIRenderComponent{ SDL_FColor{ 1.f, 1.f, 1.f, 1.f } },
				TextComponent{ Message, GlobalConstants::FontFilePath, 72.f });

			Context.Commands.Submit(std::move(AddSummaryTextCommand));
		}

		void AddSummaryControls(SystemContext& Context)
		{
			const Vector2D<int> LogicalPresentation = SDLUtils::GetRendererLogicalPresentation(&Context.Renderer);
			const Vector2D<float> ButtonSize = { LogicalPresentation.X * 0.25f, LogicalPresentation.Y * 0.1f };

			AddEntitiesCommand<PositionComponent, RectComponent, UIRenderComponent, TextComponent, ClickableComponent> AddButtonCommand(1);
			AddButtonCommand.WithEntry(PositionComponent{ {LogicalPresentation.X * 0.5f, LogicalPresentation.Y * 0.75f} },
				RectComponent{ SDL_FRect{ -ButtonSize.X * 0.5f, -ButtonSize.Y * 0.5f, ButtonSize.X, ButtonSize.Y } },
				UIRenderComponent{ SDL_FColor{ 1.f, 1.f, 1.f, 1.f } },
				TextComponent{ TransitionConstants::MainMenuButtonText, GlobalConstants::FontFilePath, 24.f },
				ClickableComponent{ ClickableTag::MainMenuButton });

			Context.Commands.Submit(std::move(AddButtonCommand));
		}
	}

	namespace Upgrades
	{
		namespace Constants
		{
			constexpr const char* UpgradeTitleText = "Pick an Upgrade";
		}
		void AddUpgradesText(SystemContext& Context)
		{
			const Vector2D<int> LogicalPresentation = SDLUtils::GetRendererLogicalPresentation(&Context.Renderer);
			const Vector2D<float> RectSize = { LogicalPresentation.X * 1.f, LogicalPresentation.Y * 0.1f };
			AddEntitiesCommand<PositionComponent, RectComponent, UIRenderComponent, TextComponent> AddUpgradesTextCommand(1);
			AddUpgradesTextCommand.WithEntry(PositionComponent{ {LogicalPresentation.X * 0.5f, LogicalPresentation.Y * 0.25f} },
				RectComponent{ SDL_FRect{ -RectSize.X * 0.5f, -RectSize.Y * 0.5f, RectSize.X, RectSize.Y } },
				UIRenderComponent{ SDL_FColor{ 1.f, 1.f, 1.f, 1.f } },
				TextComponent{ Constants::UpgradeTitleText, GlobalConstants::FontFilePath, 60.f });
			Context.Commands.Submit(std::move(AddUpgradesTextCommand));
		}
	}
}

void TransitionUtils::TravelToMainMenu(SystemContext& Context)
{
	MainMenu::AddTitleText(Context);
	MainMenu::AddMainMenuControls(Context);
	MainMenu::PlayBackgroundMusic(Context);
}

void TransitionUtils::TravelToTutorial(SystemContext& Context)
{
	Tutorial::AddTutorialText(Context);
	Tutorial::AddTutorialControls(Context);
}

void TransitionUtils::TravelToRun(SystemContext& Context)
{
	Run::StopBackgroundMusic(Context);
	Run::InitializeStageInfo(Context);
	Run::InitializeStageEntities(Context);
	InitializeBlinkingMessage(Context);
	// Initialize health indicator
}

void TransitionUtils::TravelToSummary(SystemContext& Context, const std::string& Message)
{
	Summary::AddSummaryText(Context, Message);
	Summary::AddSummaryControls(Context);
}

void TransitionUtils::TravelToUpgrades(SystemContext& Context)
{
	Upgrades::AddUpgradesText(Context);
}

void TransitionUtils::CleanupRunStage(SystemContext& Context)
{
	ComponentUtils::RemoveAllEntitiesWithComponent<GameRenderComponent>(Context);

	const TriggerQuery TriggerQuery(Context.QueryContext);
	RemoveEntitiesCommand RemoveTriggerCommand(TriggerQuery.Size());
	TriggerQuery.ForEach([&RemoveTriggerCommand](Entity E, const PositionComponent&, const RectComponent&, const HealthComponent&)
	{
		RemoveTriggerCommand.WithEntry(E);
	});
	
	Context.Commands.Submit(std::move(RemoveTriggerCommand));
}

void TransitionUtils::InitializeBlinkingMessage(SystemContext& Context)
{
	const Vector2D<int> LogicalPresentation = SDLUtils::GetRendererLogicalPresentation(&Context.Renderer);
	const Vector2D<float> RectSize = { LogicalPresentation.X * 1.f, LogicalPresentation.Y * 0.1f };

	AddEntitiesCommand<PositionComponent, RectComponent, UIRenderComponent, TextComponent, UIBlinkComponent> AddBlinkingMessageCommand(1);
	AddBlinkingMessageCommand.WithEntry(PositionComponent{ {LogicalPresentation.X * 0.5f, LogicalPresentation.Y * 0.75f} },
		RectComponent{ SDL_FRect{ -RectSize.X * 0.5f, -RectSize.Y * 0.5f, RectSize.X, RectSize.Y } },
		UIRenderComponent{ SDL_FColor{ 1.f, 1.f, 1.f, 1.f } },
		TextComponent{ TransitionConstants::PrepareMessageText, GlobalConstants::FontFilePath, 50.f },
		UIBlinkComponent{ TransitionConstants::PrepareMessageDisplayDuration, TransitionConstants::PrepareMessageDisplayDuration });

	Context.Commands.Submit(std::move(AddBlinkingMessageCommand));
}