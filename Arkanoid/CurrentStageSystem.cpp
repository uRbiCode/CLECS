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

void CurrentStageSystem::Initialize(const SystemContext& Context) const
{
	auto& EventBus = Context.EventBus;
	EventBus.Subscribe<HealthChangedEvent>(this, [this](const SystemContext& Context, const HealthChangedEvent& Event)
	{
		OnHealthChanged(Context, Event);
		return;
	});
}

void CurrentStageSystem::OnHealthChanged(const SystemContext& Context, const HealthChangedEvent& Event) const
{
	if (Event.Delta > 0)
		return;

	if (!Context.EntityAdmin.HasComponent<CollisionComponent>(Event.TargetEntity))
		return;

	const auto& Collision = Context.EntityAdmin.GetComponent<CollisionComponent>(Event.TargetEntity);

	if (Collision.Channel == CollisionChannel::Trigger && Event.NewHealth > 0)
	{
		ResetStage(Context);
	}
}

void CurrentStageSystem::ResetStage(const SystemContext& Context) const
{
	const auto CurrentStageData = StageUtils::GetCurrentStageData(Context);
	Context.EntityAdmin.GetGroup<CollisionComponent>().ForEach([&Context, &CurrentStageData](const Entity& TargetEntity, const CollisionComponent& Collision)
	{
		if (Collision.Channel == CollisionChannel::Player)
		{
			auto& Transform = Context.EntityAdmin.AccessComponent<TransformComponent>(TargetEntity);
			Transform.Position = CurrentStageData.PlayerData.PositionSize.Position;
		}
		else if (Collision.Channel == CollisionChannel::Ball)
		{
			auto& Transform = Context.EntityAdmin.AccessComponent<TransformComponent>(TargetEntity);
			Transform.Position = CurrentStageData.BallData.Position;
			auto& Velocity = Context.EntityAdmin.AccessComponent<VelocityComponent>(TargetEntity);
			Velocity.Velocity = CurrentStageData.BallData.Velocity;
		}
	});
}