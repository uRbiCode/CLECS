#include "PlayerInputSystem.h"
#include "SystemContext.h"
#include "EntityAdmin.h"
#include "InputState.h"
#include <SDL3/SDL.h>
#include "PlayerControllerComponent.h"
#include "VelocityComponent.h"

void PlayerInputSystem::Update(const SystemContext& Context, float DeltaTime) const
{
	auto& Admin = Context.EntityAdmin;
	const auto& Input = Context.Input;

	auto PlayerGroup = Admin.GetGroup<VelocityComponent, PlayerControllerComponent>();
	PlayerGroup.ForEach([&Input](Entity CurrentEntity, VelocityComponent& Velocity, const PlayerControllerComponent& Controller)
	{
		Velocity.Velocity = { 0.f, 0.f };

		if (Input.IsKeyDown(SDLK_A))
		{
			Velocity.Velocity.X -= Controller.MoveSpeed;
		}
		if (Input.IsKeyDown(SDLK_D))
		{
			Velocity.Velocity.X += Controller.MoveSpeed;
		}
	});
}