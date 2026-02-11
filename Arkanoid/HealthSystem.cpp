#include "HealthSystem.h"
#include "SystemContext.h"
#include "EventBus.h"
#include "CollisionEvent.h"
#include "HealthComponent.h"
#include "EntityAdmin.h"
#include "HealthChangedEvent.h"

namespace
{
	constexpr int DamageOnCollision = 1;

	void HandleCollision(const SystemContext& Context, const Entity& Entity)
	{
		if (!Context.EntityAdmin.HasComponent<HealthComponent>(Entity))
			return;

		auto& Health = Context.EntityAdmin.AccessComponent<HealthComponent>(Entity);
		Health.CurrentHealth -= DamageOnCollision;

		Context.EventBus.Notify(Context, HealthChangedEvent{ Entity, -DamageOnCollision, Health.CurrentHealth });
	}
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

void HealthSystem::OnCollision(const SystemContext& Context, const CollisionEvent& Event) const
{
	HandleCollision(Context, Event.EntityA);
	HandleCollision(Context, Event.EntityB);
}