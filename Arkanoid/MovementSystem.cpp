#include "MovementSystem.h"
#include "SystemContext.h"
#include "EntityAdmin.h"
#include "VelocityComponent.h"
#include "TransformComponent.h"
#include "RunStateComponent.h"
#include <cassert>

namespace
{
	bool ShouldUpdateMovement(const SystemContext& Context)
	{
		const auto RunStateGroup = Context.EntityAdmin.GetGroup<RunStateComponent>();
		if (RunStateGroup.Empty())
			return false;

		auto& RunStateComp = Context.EntityAdmin.GetComponent<RunStateComponent>(RunStateGroup[0]);
		return RunStateComp.State == RunState::Stage;
	}
}

void MovementSystem::Update(const SystemContext& Context, float DeltaTime)
{
	if (!ShouldUpdateMovement(Context))
		return;

	Context.EntityAdmin.GetGroup<TransformComponent, VelocityComponent>().ForEach([DeltaTime](const Entity& Entity, TransformComponent& Transform, const VelocityComponent& Velocity)
	{
		Transform.Position.X += Velocity.Velocity.X * DeltaTime;
		Transform.Position.Y += Velocity.Velocity.Y * DeltaTime;
	});
}