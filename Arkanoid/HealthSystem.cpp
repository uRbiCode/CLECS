#include "HealthSystem.h"
#include "SystemContext.h"
#include "EventBus.h"
#include "CollisionEvent.h"
#include "EntityAdmin.h"
#include "CollisionComponent.h"
#include "RunStateComponent.h"
#include "HealthUtils.h"
#include <cassert>

namespace
{
	constexpr int DamageOnCollision = 1;

	void DealDamage(const SystemContext& Context, const Entity& Entity, int Damage)
	{
		HealthUtils::ApplyHealthChange(Context, Entity, -Damage);
	}

	void ResolveTriggerHit(const SystemContext& Context)
	{
		const auto RunStateGroup = Context.EntityAdmin.GetGroup<RunStateComponent, HealthComponent>();
		assert(RunStateGroup.Size() == 1 && "Expected exactly one RunStateComponent with HealthComponent in the world");
		if (RunStateGroup.Empty())
			return;

		DealDamage(Context, RunStateGroup[0], DamageOnCollision);
	}

	bool WasTriggerHit(const SystemContext& Context, const Entity& Entity)
	{
		if (!Context.EntityAdmin.HasComponent<CollisionComponent>(Entity))
			return false;

		return Context.EntityAdmin.GetComponent<CollisionComponent>(Entity).Channel == CollisionChannel::Trigger;
	}

	void HandleCollision(const SystemContext& Context, const Entity& Entity)
	{
		if (WasTriggerHit(Context, Entity))
		{
			ResolveTriggerHit(Context);
			return;
		}

		DealDamage(Context, Entity, DamageOnCollision);
	}

	void OnCollision(const SystemContext& Context, const CollisionEvent& Event)
	{
		HandleCollision(Context, Event.EntityA);
		HandleCollision(Context, Event.EntityB);
	}
}

void HealthSystem::Initialize(const SystemContext& Context)
{
	const void* const Id = reinterpret_cast<const void*>(&Initialize);

	Context.EventBus.Subscribe<CollisionEvent>(Id, [](const SystemContext& Context, const CollisionEvent& Event)
	{
		OnCollision(Context, Event);
	});
}

void HealthSystem::Update(SystemQuery<Writes<HealthComponent>, Reads<HealthComponent>>&, const SystemContext& Context, float DeltaTime)
{
	Context.EntityAdmin.GetGroup<HealthComponent>().ForEach([&Context](const Entity& Entity, const HealthComponent& Health)
	{
		if (Health.CurrentHealth > 0)
			return;

		Context.EntityAdmin.DestroyEntity(Entity);
	});
}