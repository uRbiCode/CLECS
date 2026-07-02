#include "TransisitonUtils.h"
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

namespace
{
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
				TextComponent{ GlobalConstants::MainMenuButtonText, GlobalConstants::FontFilePath, Constants::ButtonFontSize },
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
		void AddTitleText(SystemContext& Context)
		{

			const auto LogicalPresentation = SDLUtils::GetRendererLogicalPresentation(&Context.Renderer);
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
			const auto LogicalPresentation = SDLUtils::GetRendererLogicalPresentation(&Context.Renderer);
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
}

void TransitionUtils::InitializeMainMenu(SystemContext& Context)
{
	MainMenu::AddTitleText(Context);
	MainMenu::AddMainMenuControls(Context);
}

void TransitionUtils::PlayBackgroundMusic(SystemContext& Context)
{
	AddEntitiesCommand<MusicRequestComponent> AddMusicRequestCommand(1);
	AddMusicRequestCommand.WithEntry(MusicRequestComponent{ MainMenu::Constants::MainMenuMusicName, 0.5f });
}

void TransitionUtils::InitializeTutorial(SystemContext& Context)
{
	Tutorial::AddTutorialText(Context);
	Tutorial::AddTutorialControls(Context);
}