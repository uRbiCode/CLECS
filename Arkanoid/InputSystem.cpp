#include "InputSystem.h"
#include "SystemContext.h"
#include "InputState.h"
#include "Query.h"
#include "PlayerMoveSpeedComponent.h"
#include "ClickableComponent.h"
#include "SDLUtils.h"
#include "PositionComponent.h"
#include "ShapeComponents.h"
#include "CollisionUtils.h"
#include "ComponentUtils.h"
#include "TransisitonUtils.h"
#include "RenderComponents.h"

namespace PlayerMoveConstants
{
	constexpr float PlayerMoveLeftMultiplier = -1.f;
	constexpr float PlayerMoveRightMultiplier = 1.f;
}

namespace
{
	float TranslateInputToPlayerMoveSpeedInputMultiplier(const InputState& Input)
	{
		float PlayerMoveSpeedInputMultiplier = 0.f;
		if (Input.IsKeyDown(SDLK_A) || Input.IsKeyDown(SDLK_LEFT))
		{
			PlayerMoveSpeedInputMultiplier += PlayerMoveConstants::PlayerMoveLeftMultiplier;
		}
		if (Input.IsKeyDown(SDLK_D) || Input.IsKeyDown(SDLK_RIGHT))
		{
			PlayerMoveSpeedInputMultiplier += PlayerMoveConstants::PlayerMoveRightMultiplier;
		}
		return PlayerMoveSpeedInputMultiplier;
	}

	void OnQuitButtonClicked()
	{
		SDL_Event Event{};
		Event.type = SDL_EVENT_QUIT;
		SDL_PushEvent(&Event);
	}

	void OnTutorialButtonClicked(SystemContext& Context)
	{
		ComponentUtils::RemoveAllEntitiesWithComponent<UIRenderComponent>(Context);
		TransitionUtils::InitializeTutorial(Context);
	}

	void OnMainMenuButtonClicked(SystemContext& Context)
	{
		ComponentUtils::RemoveAllEntitiesWithComponent<UIRenderComponent>(Context);
		TransitionUtils::InitializeMainMenu(Context);
	}
}

void InputSystem::UpdateClickables(SystemContext& Context, float DeltaTime)
{
	if (!Context.Input.IsMouseButtonJustPressed(SDL_BUTTON_LEFT))
		return;

	const Vector2D<float> MousePosition = SDLUtils::TranslateCoordinatesFromWindowToLogical(&Context.Renderer, &Context.Window, Context.Input.GetMousePosition());
	const Query<WritesList<>, ReadsList<PositionComponent, RectComponent, ClickableComponent>, ExcludeList<>> ClickableQuery(Context.QueryContext);
	ClickableQuery.ForEach([&](Entity Entity, const PositionComponent& Position, const RectComponent& Rect, const ClickableComponent& Clickable)
	{
		const SDL_FRect MouseRect = SDL_FRect{ MousePosition.X, MousePosition.Y, 0.f, 0.f };
		if (!CollisionUtils::CheckAABB(MouseRect,{ CollisionUtils::GetWorldAABB(Position, Rect)}))
			return;

		switch (Clickable.Tag)
		{
			case ClickableTag::PlayButton:
				//
				break;
			case ClickableTag::QuitButton:
				OnQuitButtonClicked();
				break;
			case ClickableTag::MainMenuButton:
				OnMainMenuButtonClicked(Context);
				break;
			case ClickableTag::TutorialButton:
				OnTutorialButtonClicked(Context);
				break;
			case ClickableTag::Upgrade:
				//
				break;
			case ClickableTag::Invalid:
			default:
				SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "RunControllerSystem::AdvanceToNextStage -> No more stages found. Victory!");
				break;
		}
	});
}

void InputSystem::TranslateRawInput(SystemContext& Context, float DeltaTime)
{
	const float PlayerMoveSpeedInputMultiplier = TranslateInputToPlayerMoveSpeedInputMultiplier(Context.Input);
	const Query<WritesList<PlayerMoveSpeedComponent>, ReadsList<>, ExcludeList<>> PlayerMoveQuery(Context.QueryContext);
	PlayerMoveQuery.ForEach([PlayerMoveSpeedInputMultiplier](Entity Entity, PlayerMoveSpeedComponent& MoveSpeed)
	{
		MoveSpeed.MoveSpeedInputMultiplier = PlayerMoveSpeedInputMultiplier;
	});
}