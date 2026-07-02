#include "InputSystem.h"
#include "SystemContext.h"
#include "InputState.h"
#include "Query.h"
#include "PlayerMoveSpeedComponent.h"

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