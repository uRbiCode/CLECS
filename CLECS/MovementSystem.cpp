#include "MovementSystem.h"
#include "SystemUpdateContext.h"
#include "EntityManager.h"
#include <SDL3/SDL.h>
#include "VelocityComponent.h"
#include "TransformComponent.h"
#include "MovementUtils.h"

void MovementSystem::Update(const SystemUpdateContext& UpdateContext, float DeltaTime)
{
	auto& Manager = UpdateContext.EntityManager;

	// Apply velocity to all entities with Transform and Velocity
	auto MovementGroup = Manager.GetGroup<TransformComponent, VelocityComponent>();
	MovementGroup.ForEach([DeltaTime](Entity CurrentEntity, TransformComponent& Transform, const VelocityComponent& Velocity)
	{
		Transform.Position = MovementUtils::GetPositionAfterMove(Transform.Position, Velocity.Velocity, DeltaTime);
		Transform.Rotation = MovementUtils::GetRotationAfterMove(Transform.Rotation, Velocity.AngularVelocity, DeltaTime);
	});
}