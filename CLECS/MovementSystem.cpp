#include "MovementSystem.h"
#include "SystemUpdateContext.h"
#include "EntityManager.h"
#include <SDL3/SDL.h>
#include "VelocityComponent.h"
#include "TransformComponent.h"

void MovementSystem::Update(const SystemUpdateContext& UpdateContext, float DeltaTime)
{
	auto& Manager = UpdateContext.EntityManager;

	// Apply velocity to all entities with Transform and Velocity
	auto MovementGroup = Manager.GetGroup<TransformComponent, VelocityComponent>();
	MovementGroup.ForEach([DeltaTime](Entity CurrentEntity, TransformComponent& Transform, const VelocityComponent& Velocity)
	{
		Transform.Position.X += Velocity.Velocity.X * DeltaTime;
		Transform.Position.Y += Velocity.Velocity.Y * DeltaTime;
		Transform.Rotation += Velocity.AngularVelocity * DeltaTime;
	});
}