#include "HealthSystem.h"
#include "SystemContext.h"
#include "EventBus.h"
#include "CollisionEvent.h"
#include "HealthComponent.h"
#include "EntityAdmin.h"

namespace
{
	void HandleCollision(const SystemContext& Context, const Entity& Entity)
	{
		if (!Context.EntityAdmin.HasComponent<HealthComponent>(Entity))
			return;

		auto& Health = Context.EntityAdmin.AccessComponent<HealthComponent>(Entity);
		Health.CurrentHealth -= 1;
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
	std::vector<Entity> EntitiesToDestroy;
	Context.EntityAdmin.GetGroup<HealthComponent>().ForEach([&Context, &EntitiesToDestroy](const Entity& CurrentEntity, const HealthComponent& Health)
	{
		if (Health.CurrentHealth <= 0)
		{
			EntitiesToDestroy.push_back(CurrentEntity);
		}
	});

	for (const auto& Entity : EntitiesToDestroy)
	{
		Context.EntityAdmin.DestroyEntity(Entity);
	}
}

void HealthSystem::OnCollision(const SystemContext& Context, const CollisionEvent& Event) const
{
	HandleCollision(Context, Event.EntityA);
	HandleCollision(Context, Event.EntityB);
}