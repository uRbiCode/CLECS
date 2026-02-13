#include "MovementSystem.h"
#include "SystemContext.h"
#include "EntityAdmin.h"
#include "VelocityComponent.h"
#include "TransformComponent.h"

void MovementSystem::Update(const SystemContext& Context, float DeltaTime) const
{
	auto& Admin = Context.EntityAdmin;

	auto MovementGroup = Admin.GetGroup<TransformComponent, VelocityComponent>();
	MovementGroup.ForEach([DeltaTime](const Entity& Entity, TransformComponent& Transform, const VelocityComponent& Velocity)
	{
		Transform.Position.X += Velocity.Velocity.X * DeltaTime;
		Transform.Position.Y += Velocity.Velocity.Y * DeltaTime;
	});
}