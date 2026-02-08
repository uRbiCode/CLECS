#include "MovementSystem.h"
#include "SystemUpdateContext.h"
#include "EntityManager.h"
#include "Components.h"
#include "InputState.h"
#include <SDL3/SDL.h>

void MovementSystem::Update(const SystemUpdateContext& UpdateContext, float DeltaTime)
{
	auto& Manager = UpdateContext.EntityManager;
	const auto& Input = UpdateContext.Input;

	// Handle player-controlled entities
	auto PlayerGroup = Manager.GetGroup<TransformComponent, VelocityComponent, PlayerControllerComponent>();
	PlayerGroup.ForEach([&Input, DeltaTime](Entity CurrentEntity, TransformComponent& Transform, VelocityComponent& Velocity, PlayerControllerComponent& Controller)
	{
		// Reset velocity
		Velocity.Velocity = { 0.f, 0.f };

		// Check WASD input
		if (Input.IsKeyDown(SDLK_W))
		{
			Velocity.Velocity.Y -= Controller.MoveSpeed;
		}
		if (Input.IsKeyDown(SDLK_S))
		{
			Velocity.Velocity.Y += Controller.MoveSpeed;
		}
		if (Input.IsKeyDown(SDLK_A))
		{
			Velocity.Velocity.X -= Controller.MoveSpeed;
		}
		if (Input.IsKeyDown(SDLK_D))
		{
			Velocity.Velocity.X += Controller.MoveSpeed;
		}

		// Normalize diagonal movement
		const float VelocityMagnitude = std::sqrt(Velocity.Velocity.X * Velocity.Velocity.X + Velocity.Velocity.Y * Velocity.Velocity.Y);
		if (VelocityMagnitude > Controller.MoveSpeed)
		{
			Velocity.Velocity.X = (Velocity.Velocity.X / VelocityMagnitude) * Controller.MoveSpeed;
			Velocity.Velocity.Y = (Velocity.Velocity.Y / VelocityMagnitude) * Controller.MoveSpeed;
		}

		Transform.Position.X += Velocity.Velocity.X * DeltaTime;
		Transform.Position.Y += Velocity.Velocity.Y * DeltaTime;
	});
}