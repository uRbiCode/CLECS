#include "MovementSystem.h"
#include "SystemContext.h"
#include "EntityAdmin.h"
#include "VelocityComponent.h"
#include "TransformComponent.h"

void MovementSystem::Update(const SystemContext& Context, float DeltaTime) const
{
	Context.EntityAdmin.GetGroup<TransformComponent, VelocityComponent>().ForEach([DeltaTime](const Entity& Entity, TransformComponent& Transform, const VelocityComponent& Velocity)
	{
		Transform.Position.X += Velocity.Velocity.X * DeltaTime;
		Transform.Position.Y += Velocity.Velocity.Y * DeltaTime;
	});
}