#include "HealthSystem.h"
#include "SystemContext.h"
#include "EventBus.h"
#include "CollisionEvent.h"
#include "HealthComponent.h"
#include "EntityAdmin.h"
#include "HealthChangedEvent.h"
#include "CollisionComponent.h"
#include "RunStateComponent.h"
#include <cassert>

namespace
{
	constexpr int DamageOnCollision = 1;
}

void HealthSystem::Initialize(const SystemContext& Context) const
{
	auto& EventBus = Context.EventBus;
	EventBus.Subscribe<CollisionEvent>(this, [this](const SystemContext& Context, const CollisionEvent& Event)
	{
		OnCollision(Context, Event);
		return;
	});
}

void HealthSystem::Update(const SystemContext& Context, float DeltaTime) const
{
	Context.EntityAdmin.GetGroup<HealthComponent>().ForEach([&Context](const Entity& Entity, const HealthComponent& Health)
	{
		if (Health.CurrentHealth <= 0)
		{
			Context.EntityAdmin.DestroyEntity(Entity);
		}
	});
}

bool HealthSystem::WasTriggerHit(const SystemContext& Context, const Entity& Entity) const
{
	if (!Context.EntityAdmin.HasComponent<CollisionComponent>(Entity))
		return false;

	return Context.EntityAdmin.GetComponent<CollisionComponent>(Entity).Channel == CollisionChannel::Trigger;
}

void HealthSystem::OnCollision(const SystemContext& Context, const CollisionEvent& Event) const
{
	HandleCollision(Context, Event.EntityA);
	HandleCollision(Context, Event.EntityB);
}

void HealthSystem::HandleCollision(const SystemContext& Context, const Entity& Entity) const
{
	if (WasTriggerHit(Context, Entity))
	{
		ResolveTriggerHit(Context);
		return;
	}

	DealDamage(Context, Entity, DamageOnCollision);
}

void HealthSystem::ResolveTriggerHit(const SystemContext& Context) const
{
	const auto RunStateGroup = Context.EntityAdmin.GetGroup<RunStateComponent, HealthComponent>();
	assert(RunStateGroup.Size() == 1 && "Expected exactly one RunStateComponent with HealthComponent in the world");
	if (RunStateGroup.Empty())
		return;

	DealDamage(Context, RunStateGroup[0], DamageOnCollision);
}

void HealthSystem::DealDamage(const SystemContext& Context, const Entity& Entity, int Damage) const
{
	if (!Context.EntityAdmin.HasComponent<HealthComponent>(Entity))
		return;

	auto& Health = Context.EntityAdmin.AccessComponent<HealthComponent>(Entity);
	Health.CurrentHealth -= DamageOnCollision;

	Context.EventBus.Notify(Context, HealthChangedEvent{ Entity, -Damage, Health.CurrentHealth });
}