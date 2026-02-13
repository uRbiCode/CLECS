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

void CurrentStageSystem::Initialize(const SystemContext& Context) const
{
	auto& EventBus = Context.EventBus;
	EventBus.Subscribe<HealthChangedEvent>(this, [this](const SystemContext& Context, const HealthChangedEvent& Event)
	{
		OnHealthChanged(Context, Event);
		return;
	});

	EventBus.Subscribe<CollisionEvent>(this, [this](const SystemContext& Context, const CollisionEvent& Event)
	{
		OnCollision(Context, Event);
		return;
	});
}

void CurrentStageSystem::OnHealthChanged(const SystemContext& Context, const HealthChangedEvent& Event) const
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

void CurrentStageSystem::ResetStage(const SystemContext& Context) const
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

bool CurrentStageSystem::AreAllBricksDestroyed(const SystemContext& Context) const
{
	bool AllDestroyed = true;
	Context.EntityAdmin.GetGroup<CollisionComponent, HealthComponent>().ForEach([&AllDestroyed](const Entity& Entity, const CollisionComponent& Collision, const HealthComponent& Health)
	{
		AllDestroyed &= (Collision.Channel != CollisionChannel::Brick || Health.CurrentHealth <= 0);
	});
	return AllDestroyed;
}

void CurrentStageSystem::NotifyStageEnd(const SystemContext& Context, bool Victory) const
{
	Context.EventBus.Notify(Context, StageEndEvent{ Victory });
}

void CurrentStageSystem::OnCollision(const SystemContext& Context, const CollisionEvent& Event) const
{
	auto TryIncreaseBallSpeed = [this, &Context](const Entity& Entity)
	{
		if (!Context.EntityAdmin.HasComponent<CollisionComponent>(Entity))
			return;

		if (Context.EntityAdmin.GetComponent<CollisionComponent>(Entity).Channel != CollisionChannel::Ball)
			return;

		if (!Context.EntityAdmin.HasComponent<VelocityComponent>(Entity))
			return;

		//Increase ball speed with each collision
		auto& Velocity = Context.EntityAdmin.AccessComponent<VelocityComponent>(Entity);
		Velocity.Velocity *= 1.01f;
	};

	TryIncreaseBallSpeed(Event.EntityA);
	TryIncreaseBallSpeed(Event.EntityB);
}