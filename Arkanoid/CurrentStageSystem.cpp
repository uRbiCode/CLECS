#include "CurrentStageSystem.h"
#include "SystemContext.h"
#include "HealthChangedEvent.h"
#include "EventBus.h"
#include "EntityAdmin.h"
#include "CollisionComponent.h"
#include "TransformComponent.h"
#include "VelocityComponent.h"
#include "HealthComponent.h"
#include "StageUtils.h"
#include "StageDataComponent.h"
#include "StageEndEvent.h"
#include "RunStateComponent.h"
#include "CollisionEvent.h"
#include "UpgradeUtils.h"

namespace
{
	void NotifyStageEnd(const SystemContext& Context, bool Victory)
	{
		Context.EventBus.Notify(Context, StageEndEvent{ Victory });
	}

	bool AreAllBricksDestroyed(const SystemContext& Context)
	{
		bool AllDestroyed = true;
		Context.EntityAdmin.GetGroup<CollisionComponent, HealthComponent>().ForEach([&AllDestroyed](const Entity& Entity, const CollisionComponent& Collision, const HealthComponent& Health)
		{
			AllDestroyed &= (Collision.Channel != CollisionChannel::Brick || Health.CurrentHealth <= 0);
		});
		return AllDestroyed;
	}

	void ResetStage(const SystemContext& Context)
	{
		const auto CurrentStageData = StageUtils::GetCurrentStageData(Context);
		Context.EntityAdmin.GetGroup<CollisionComponent>().ForEach([&Context, &CurrentStageData](const Entity& Entity, const CollisionComponent& Collision)
		{
			if (Collision.Channel == CollisionChannel::Player)
			{
				auto& Transform = Context.EntityAdmin.AccessComponent<TransformComponent>(Entity);
				Transform.Position = CurrentStageData.PlayerData.PositionSize.Position;
			}
			else if (Collision.Channel == CollisionChannel::Ball)
			{
				auto& Transform = Context.EntityAdmin.AccessComponent<TransformComponent>(Entity);
				Transform.Position = CurrentStageData.BallData.Position;
				auto& Velocity = Context.EntityAdmin.AccessComponent<VelocityComponent>(Entity);
				Velocity.Velocity = CurrentStageData.BallData.Velocity * UpgradeUtils::GetBallVelocityModifier(Context);
			}
		});
	}

	void OnHealthChanged(const SystemContext& Context, const HealthChangedEvent& Event)
	{
		if (Context.EntityAdmin.HasComponent<RunStateComponent>(Event.Entity))
		{
			if (Event.NewHealth > 0)
			{
				ResetStage(Context);
			}
			else
			{
				NotifyStageEnd(Context, false);
			}
			return;
		}

		if (!Context.EntityAdmin.HasComponent<CollisionComponent>(Event.Entity))
			return;

		const auto& Collision = Context.EntityAdmin.GetComponent<CollisionComponent>(Event.Entity);
		if (Collision.Channel == CollisionChannel::Brick && Event.NewHealth <= 0 && AreAllBricksDestroyed(Context))
		{
			NotifyStageEnd(Context, true);
		}
	}

	void OnCollision(const SystemContext& Context, const CollisionEvent& Event)
	{
		auto TryIncreaseBallSpeed = [&Context](const Entity& Entity)
		{
			if (!Context.EntityAdmin.HasComponent<CollisionComponent>(Entity))
				return;

			if (Context.EntityAdmin.GetComponent<CollisionComponent>(Entity).Channel != CollisionChannel::Ball)
				return;

			if (!Context.EntityAdmin.HasComponent<VelocityComponent>(Entity))
				return;

			auto& Velocity = Context.EntityAdmin.AccessComponent<VelocityComponent>(Entity);
			Velocity.Velocity *= 1.01f;
		};

		TryIncreaseBallSpeed(Event.EntityA);
		TryIncreaseBallSpeed(Event.EntityB);
	}
}

void CurrentStageSystem::Initialize(const SystemContext& Context)
{
	const void* const Id = reinterpret_cast<const void*>(&Initialize);

	Context.EventBus.Subscribe<HealthChangedEvent>(Id, [](const SystemContext& Context, const HealthChangedEvent& Event)
	{
		OnHealthChanged(Context, Event);
	});

	Context.EventBus.Subscribe<CollisionEvent>(Id, [](const SystemContext& Context, const CollisionEvent& Event)
	{
		OnCollision(Context, Event);
	});
}