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
#include "CommandRunner.h"
#include "BeginStageComponent.h"
#include "ComponentUtils.h"

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

	void CleanupClickableUsedComponents(SystemContext& Context)
	{
		const Query<WritesList<>, ReadsList<ClickableUsedComponent>, ExcludeList<>> CleanupClickableUsedQuery(Context.QueryContext);
		if (CleanupClickableUsedQuery.Size() < 1)
			return;

		RemoveComponentsCommand<ClickableUsedComponent> RemoveCommand(CleanupClickableUsedQuery.Size());

		CleanupClickableUsedQuery.ForEach([&RemoveCommand](Entity Entity, const ClickableUsedComponent& ClickableUsed)
		{
			RemoveCommand.WithEntry(Entity);
		});

		Context.Commands.Submit(std::move(RemoveCommand));
	}

	bool ShouldBeginStage(const InputState& Input)
	{
		return Input.IsKeyJustPressed(SDLK_SPACE);
	}

	void CleanupInput(SystemContext& Context)
	{
		ComponentUtils::RemoveAllEntitiesWithComponent<BeginStageComponent>(Context);
	}
}

void InputSystem::UpdateClickables(SystemContext& Context, float DeltaTime)
{
	CleanupClickableUsedComponents(Context);

	if (!Context.Input.IsMouseButtonJustPressed(SDL_BUTTON_LEFT))
		return;

	const Vector2D<float> MousePosition = SDLUtils::TranslateCoordinatesFromWindowToLogical(&Context.Renderer, &Context.Window, Context.Input.GetMousePosition());
	const Query<WritesList<>, ReadsList<PositionComponent, RectComponent, ClickableComponent>, ExcludeList<>> ClickableQuery(Context.QueryContext);
	AddEntitiesCommand<ClickableUsedComponent> AddClickableUsedCommand(0);
	ClickableQuery.ForEach([&](Entity Entity, const PositionComponent& Position, const RectComponent& Rect, const ClickableComponent& Clickable)
	{
		const SDL_FRect MouseRect = SDL_FRect{ MousePosition.X, MousePosition.Y, 0.f, 0.f };
		if (!CollisionUtils::CheckAABB(MouseRect,{ CollisionUtils::GetWorldAABB(Position, Rect)}))
			return;

		AddClickableUsedCommand.WithEntry(ClickableUsedComponent{});
	});

	if (AddClickableUsedCommand.AccessEntries().size() > 0)
	{
		Context.Commands.Submit(std::move(AddClickableUsedCommand));
	}
}

void InputSystem::TranslateRawInput(SystemContext& Context, float DeltaTime)
{
	CleanupInput(Context);

	const float PlayerMoveSpeedInputMultiplier = TranslateInputToPlayerMoveSpeedInputMultiplier(Context.Input);
	const Query<WritesList<PlayerMoveSpeedComponent>, ReadsList<>, ExcludeList<>> PlayerMoveQuery(Context.QueryContext);
	PlayerMoveQuery.ForEach([PlayerMoveSpeedInputMultiplier](Entity Entity, PlayerMoveSpeedComponent& MoveSpeed)
	{
		MoveSpeed.MoveSpeedInputMultiplier = PlayerMoveSpeedInputMultiplier;
	});

	if (!ShouldBeginStage(Context.Input))
		return;

	AddEntitiesCommand<BeginStageComponent> AddBeginStageCommand(1);
	AddBeginStageCommand.WithEntry(BeginStageComponent{});
	Context.Commands.Submit(std::move(AddBeginStageCommand));
}