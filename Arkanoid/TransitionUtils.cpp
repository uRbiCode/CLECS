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
#include "TextureManager.h"
#include "TextureComponent.h"

namespace
{
	namespace TransitionConstants
	{
		constexpr const char* MainMenuButtonText = "Main Menu";

		constexpr const char* PrepareMessageText = "PRESS SPACE TO START";
		constexpr float PrepareMessageDisplayDuration = 0.75f;

		constexpr float TitleFontSize = 72.f;
	}

	namespace Common
	{
		void AddMainMenuButton(SystemContext& Context)
		{
			const Vector2D<float> LogicalPresentation = SDLUtils::GetRendererLogicalPresentation(&Context.Renderer);
			const Vector2D<float> ButtonSize = { LogicalPresentation.X * 0.25f, LogicalPresentation.Y * 0.1f };

			AddEntitiesCommand<PositionComponent, RectComponent, UIRenderComponent, TextComponent, ClickableComponent> AddButtonCommand(1);
			AddButtonCommand.WithEntry(PositionComponent{ {LogicalPresentation.X * 0.5f, LogicalPresentation.Y * 0.85f} },
				RectComponent{ SDL_FRect{ -ButtonSize.X * 0.5f, -ButtonSize.Y * 0.5f, ButtonSize.X, ButtonSize.Y } },
				UIRenderComponent{ SDL_FColor{ 1.f, 1.f, 1.f, 1.f } },
				TextComponent{ TransitionConstants::MainMenuButtonText, GlobalConstants::FontFilePath, 24.f },
				ClickableComponent{ ClickableTag::MainMenuButton });

			Context.Commands.Submit(std::move(AddButtonCommand));
		}
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
			constexpr size_t TextLinesCount = std::size(TextLines);
			constexpr float ButtonFontSize = 24.f;
		}


#pragma warning(push)
#pragma warning(disable : 5045)
		void AddTutorialText(SystemContext& Context)
		{
			const Vector2D<float> LogicalPresentation = SDLUtils::GetRendererLogicalPresentation(&Context.Renderer);
			const Vector2D<float> TitleRectSize = { LogicalPresentation.X * 1.f, LogicalPresentation.Y * 0.1f };

			AddEntitiesCommand<PositionComponent, UIRenderComponent, TextComponent> AddTextCommand(5);
			AddTextCommand.WithEntry(PositionComponent{ {LogicalPresentation.X * 0.15f, LogicalPresentation.Y * 0.15f} },
				UIRenderComponent{ SDL_FColor{ 1.f, 1.f, 1.f, 1.f } },
				TextComponent{ Constants::Title, GlobalConstants::FontFilePath, TransitionConstants::TitleFontSize });
			Context.Commands.Submit(std::move(AddTextCommand));

			const auto IndexToXOffset = [](size_t Index) -> float
			{
				switch (Index)
				{
					case 0:
						return 0.09f;
					case 1:
						return 0.06f;
					case 2:
						return 0.16f;
					case 3:
						return 0.41f;
					default:
						return 0.f;
				}
			};

			for (size_t i = 0; i < Constants::TextLinesCount; ++i)
			{
				AddTextCommand.WithEntry(PositionComponent{ {LogicalPresentation.X * IndexToXOffset(i), LogicalPresentation.Y * (0.42f + static_cast<float>(i) * 0.07f)} },
					UIRenderComponent{ SDL_FColor{ 1.f, 1.f, 1.f, 1.f } },
					TextComponent{ Constants::TextLines[i], GlobalConstants::FontFilePath, Constants::ButtonFontSize });
			}

			Context.Commands.Submit(std::move(AddTextCommand));
		}
#pragma warning(pop)
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
			constexpr size_t ButtonsTextsCount = std::size(ButtonsTexts);
			constexpr const char* MainMenuMusicName = "main_menu_loop";
		}

		void PlayBackgroundMusic(SystemContext& Context)
		{
			AddEntitiesCommand<MusicRequestComponent> AddMusicRequestCommand(1);
			AddMusicRequestCommand.WithEntry(MusicRequestComponent{ MainMenu::Constants::MainMenuMusicName, 0.5f });
			Context.Commands.Submit(std::move(AddMusicRequestCommand));
		}

		void AddTitleText(SystemContext& Context)
		{
			const Vector2D<float> LogicalPresentation = SDLUtils::GetRendererLogicalPresentation(&Context.Renderer);

			AddEntitiesCommand<PositionComponent, UIRenderComponent, TextComponent> AddTitleTextCommand(1);
			AddTitleTextCommand.WithEntry(PositionComponent{ {LogicalPresentation.X * 0.2f, LogicalPresentation.Y * 0.15f} },
				UIRenderComponent{ SDL_FColor{ 1.f, 1.f, 1.f, 1.f } },
				TextComponent{ MainMenu::Constants::TitleText, GlobalConstants::FontFilePath, TransitionConstants::TitleFontSize });

			Context.Commands.Submit(std::move(AddTitleTextCommand));
		}

#pragma warning(push)
#pragma warning(disable : 5045)
		void AddMainMenuControls(SystemContext& Context)
		{
			const Vector2D<float> LogicalPresentation = SDLUtils::GetRendererLogicalPresentation(&Context.Renderer);
			const Vector2D<float> ButtonSize = { LogicalPresentation.X * 0.4f, LogicalPresentation.Y * 0.1f };
			AddEntitiesCommand<PositionComponent, RectComponent, UIRenderComponent, ClickableComponent, TextComponent> AddButtonsCommand(3);

			const auto IndexToClickableTag = [](size_t Index) -> ClickableTag
			{
				switch (Index)
				{
					case 0:
						return ClickableTag::PlayButton;
					case 1:
						return ClickableTag::TutorialButton;
					case 2:
						return ClickableTag::QuitButton;
					default:
						return ClickableTag::Invalid;
				}
			};

			for (size_t i = 0; i < Constants::ButtonsTextsCount; ++i)
			{
				AddButtonsCommand.WithEntry(PositionComponent{ {LogicalPresentation.X * 0.5f, LogicalPresentation.Y * (0.5f + static_cast<float>(i) * 0.15f)} },
					RectComponent{ SDL_FRect{ -ButtonSize.X * 0.5f, -ButtonSize.Y * 0.5f, ButtonSize.X, ButtonSize.Y } },
					UIRenderComponent{ SDL_FColor{ 1.f, 1.f, 1.f, 1.f } },
					ClickableComponent{ IndexToClickableTag(i) },
					TextComponent{ MainMenu::Constants::ButtonsTexts[i], GlobalConstants::FontFilePath, 32.f });
			}

			Context.Commands.Submit(std::move(AddButtonsCommand));
		}
#pragma warning(pop)
	}

	namespace Run
	{
		namespace Constants
		{
			constexpr const char* StageText = "STAGE ";

			constexpr const char* HealthIndicatorTexturePath = "../Assets/Textures/Hearts.png";
			constexpr const SDL_FRect HealthIndicatorTextureRect = {115.f, 3.f, 11.f, 10.f};
			constexpr float HealthIndicatorSpacing = 10.f;
		}

		std::string BuildStageText(SystemContext& Context)
		{
			const int StageNumber = RunUtils::GetCurrentStageNumber(Context);
			return Constants::StageText + std::to_string(StageNumber);
		}

		SDL_Texture* GetHealthIndicatorTexture(const TextureManager& TextureManager)
		{
			return TextureManager.GetTexture(Constants::HealthIndicatorTexturePath);
		}

		void StopBackgroundMusic(SystemContext& Context)
		{
			AddEntitiesCommand<StopMusicComponent> AddStopMusicCommand(1);
			AddStopMusicCommand.WithEntry(StopMusicComponent{});
			Context.Commands.Submit(std::move(AddStopMusicCommand));
		}

		void InitializeStageInfo(SystemContext& Context)
		{
			const Vector2D<float> LogicalPresentation = SDLUtils::GetRendererLogicalPresentation(&Context.Renderer);
			const Vector2D<float> RectSize = { LogicalPresentation.X * 0.25f, LogicalPresentation.Y * 0.1f };

			AddEntitiesCommand<PositionComponent, UIRenderComponent, TextComponent> AddStageInfoCommand(1);
			AddStageInfoCommand.WithEntry(PositionComponent{ {LogicalPresentation.X * 0.75f, LogicalPresentation.Y * 0.93f} },
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

		void InitializeHealthIndicators(SystemContext& Context)
		{
			int PlayerHealth = GlobalConstants::InitialPlayerHealth;
			const Query<WritesList<>, ReadsList<HealthComponent, BackgroundRenderComponent>, ExcludeList<>> HealthQuery(Context.QueryContext);
			HealthQuery.ForEach([&]([[maybe_unused]] Entity Entity, const HealthComponent& Health, [[maybe_unused]] const BackgroundRenderComponent& BackgroundRender)
			{
				PlayerHealth = Health.CurrentHealth;
			});

			const Vector2D<float> LogicalPresentation = SDLUtils::GetRendererLogicalPresentation(&Context.Renderer);
			const Vector2D<float> RectSize = { LogicalPresentation.X * 0.05f, LogicalPresentation.Y * 0.05f };
			const float PositionY = LogicalPresentation.Y * 0.96f;

			AddEntitiesCommand<PositionComponent, RectComponent, UIRenderComponent, TextureComponent> AddHealthIndicatorsCommand(GlobalConstants::InitialPlayerHealth);
			for (int i = 0; i < PlayerHealth; ++i)
			{
				const float PositionX = LogicalPresentation.X * 0.1f + static_cast<float>(i) * (RectSize.X + Run::Constants::HealthIndicatorSpacing);
				AddHealthIndicatorsCommand.WithEntry(PositionComponent{ {PositionX, PositionY} },
					RectComponent{ SDL_FRect{ -RectSize.X * 0.5f, -RectSize.Y * 0.5f, RectSize.X, RectSize.Y } },
					UIRenderComponent{ SDL_FColor{ 1.f, 1.f, 1.f, 1.f } },
					TextureComponent{ GetHealthIndicatorTexture(Context.TextureManager), Run::Constants::HealthIndicatorTextureRect});
			}

			Context.Commands.Submit(std::move(AddHealthIndicatorsCommand));
		}
	}

	namespace Summary
	{
		void AddSummaryText(SystemContext& Context, const std::string& Message)
		{
			const Vector2D<float> LogicalPresentation = SDLUtils::GetRendererLogicalPresentation(&Context.Renderer);
			const Vector2D<float> RectSize = { LogicalPresentation.X * 1.f, LogicalPresentation.Y * 0.1f };

			AddEntitiesCommand<PositionComponent, UIRenderComponent, TextComponent> AddSummaryTextCommand(1);
			AddSummaryTextCommand.WithEntry(PositionComponent{ {LogicalPresentation.X * 0.31f, LogicalPresentation.Y * 0.15f} },
				UIRenderComponent{ SDL_FColor{ 1.f, 1.f, 1.f, 1.f } },
				TextComponent{ Message, GlobalConstants::FontFilePath, TransitionConstants::TitleFontSize });

			Context.Commands.Submit(std::move(AddSummaryTextCommand));
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
			const Vector2D<float> LogicalPresentation = SDLUtils::GetRendererLogicalPresentation(&Context.Renderer);
			AddEntitiesCommand<PositionComponent, UIRenderComponent, TextComponent> AddUpgradesTextCommand(1);
			AddUpgradesTextCommand.WithEntry(PositionComponent{ {LogicalPresentation.X * 0.12f, LogicalPresentation.Y * 0.15f} },
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
	Common::AddMainMenuButton(Context);
}

void TransitionUtils::TravelToRun(SystemContext& Context)
{
	Run::StopBackgroundMusic(Context);
	Run::InitializeStageInfo(Context);
	Run::InitializeStageEntities(Context);
	InitializeBlinkingMessage(Context);
	Run::InitializeHealthIndicators(Context);
}

void TransitionUtils::TravelToSummary(SystemContext& Context, const std::string& Message)
{
	Summary::AddSummaryText(Context, Message);
	Common::AddMainMenuButton(Context);
}

void TransitionUtils::TravelToUpgrades(SystemContext& Context)
{
	Upgrades::AddUpgradesText(Context);
}

void TransitionUtils::CleanupRunStage(SystemContext& Context)
{
	ComponentUtils::RemoveAllEntitiesWithComponent<GameRenderComponent>(Context);
	ComponentUtils::RemoveAllEntitiesWithComponent<UIRenderComponent>(Context);

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
	const Vector2D<float> LogicalPresentation = SDLUtils::GetRendererLogicalPresentation(&Context.Renderer);
	const Vector2D<float> RectSize = { LogicalPresentation.X * 1.f, LogicalPresentation.Y * 0.1f };

	AddEntitiesCommand<PositionComponent, UIRenderComponent, TextComponent, UIBlinkComponent> AddBlinkingMessageCommand(1);
	AddBlinkingMessageCommand.WithEntry(PositionComponent{ {LogicalPresentation.X * 0.06f, LogicalPresentation.Y * 0.75f} },
		UIRenderComponent{ SDL_FColor{ 1.f, 1.f, 1.f, 1.f } },
		TextComponent{ TransitionConstants::PrepareMessageText, GlobalConstants::FontFilePath, 50.f },
		UIBlinkComponent{ TransitionConstants::PrepareMessageDisplayDuration, TransitionConstants::PrepareMessageDisplayDuration });

	Context.Commands.Submit(std::move(AddBlinkingMessageCommand));
}