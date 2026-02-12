#include "RunControllerSystem.h"
#include "SystemContext.h"
#include "EventBus.h"
#include "RunStateComponent.h"
#include "StageDataComponent.h"
#include "EntityAdmin.h"
#include "CollisionComponent.h"
#include "TransformComponent.h"
#include "ShapeComponents.h"
#include "RenderComponent.h"
#include "TextureComponent.h"
#include "HealthComponent.h"
#include "VelocityComponent.h"
#include "PlayerControllerComponent.h"
#include "HealthChangedEvent.h"
#include "TextureManager.h"
#include "StageBeginEvent.h"
#include "RenderConstants.h"
#include "GameStateEvents.h"
#include "GameStateUtils.h"
#include <cassert>
#include "StageDataLoader.h"
#include "StageUtils.h"

void RunControllerSystem::Initialize(const SystemContext& Context) const
{
	Context.EventBus.Subscribe<HealthChangedEvent>(this, [this](const SystemContext& Context, const HealthChangedEvent& Event)
	{
		OnHealthChanged(Context, Event);
		return;
	});

	Context.EventBus.Subscribe<GameStateBeginEvent>(this, [this](const SystemContext& Context, const GameStateBeginEvent& Event)
	{
		if (Event.BeginningState == GameState::Run)
		{
			BeginRun(Context);
		}
	});
}

void RunControllerSystem::BeginRun(const SystemContext& Context) const
{
	AddRunStateComponent(Context);
	AddStageDataComponent(Context);

	// Move to stage 1
	HandleStageCleared(Context);
}

void RunControllerSystem::CleanupRun(const SystemContext& Context) const
{
	CleanupCurrentStage(Context);
	RemoveRunStateComponent(Context);
	RemoveStageDataComponent(Context);
}

void RunControllerSystem::AddRunStateComponent(const SystemContext& Context) const
{
	auto& Admin = Context.EntityAdmin;
	auto RunStateEntity = Admin.CreateEntity();
	Admin.AddComponent<RunStateComponent>(RunStateEntity);
}

void RunControllerSystem::AddStageDataComponent(const SystemContext& Context) const
{
	auto& Admin = Context.EntityAdmin;

	auto StageDataEntity = Admin.CreateEntity();
	Admin.AddComponent<StageDataComponent>(StageDataEntity);
}

void RunControllerSystem::RemoveRunStateComponent(const SystemContext& Context) const
{
	Context.EntityAdmin.GetGroup<RunStateComponent>().ForEach([&Context](const Entity& Entity, const RunStateComponent& RunState)
	{
		Context.EntityAdmin.DestroyEntity(Entity);
	});
}

void RunControllerSystem::RemoveStageDataComponent(const SystemContext& Context) const
{
	Context.EntityAdmin.GetGroup<StageDataComponent>().ForEach([&Context](const Entity& Entity, const StageDataComponent& StageData)
	{
		Context.EntityAdmin.DestroyEntity(Entity);
	});
}

void RunControllerSystem::OnHealthChanged(const SystemContext& Context, const HealthChangedEvent& Event) const
{
	if (Event.Delta >= 0)
		return;

	if (!Context.EntityAdmin.HasComponent<CollisionComponent>(Event.Entity))
		return;

	const auto& Collision = Context.EntityAdmin.GetComponent<CollisionComponent>(Event.Entity);
	if (Collision.Channel == CollisionChannel::Brick)
	{
		if (AreAllBricksDestroyed(Context))
		{
			HandleStageCleared(Context);
		}
	}
	else if (Collision.Channel == CollisionChannel::Trigger)
	{
		if (HasPlayerLost(Context))
		{
			HandleRunDefeat(Context);
		}
	}
}

void RunControllerSystem::HandleStageCleared(const SystemContext& Context) const
{
	const auto RunStateGroup = Context.EntityAdmin.GetGroup<RunStateComponent>();
	assert(RunStateGroup.Size() == 1 && "Expected exactly one RunStateComponent in the world");
	if (RunStateGroup.Empty())
		return;

	auto& RunStateComp = Context.EntityAdmin.AccessComponent<RunStateComponent>(RunStateGroup[0]);
	if (AdvanceToNextStage(Context, RunStateComp.CurrentStage))
	{
		Context.EventBus.Notify(Context, StageBeginEvent{ RunStateComp.CurrentStage });
		RunStateComp.CurrentStage++;
	}
	else
	{
		HandleRunVictory(Context);
	}
}

void RunControllerSystem::HandleRunVictory(const SystemContext& Context) const
{
	CleanupRun(Context);
	GameStateUtils::RequestStateChange(Context, GameState::Victory);
}

void RunControllerSystem::HandleRunDefeat(const SystemContext& Context) const
{
	CleanupRun(Context);
	GameStateUtils::RequestStateChange(Context, GameState::Defeat);
}

bool RunControllerSystem::AreAllBricksDestroyed(const SystemContext& Context) const
{
	bool AllDestroyed = true;
	Context.EntityAdmin.GetGroup<CollisionComponent, HealthComponent>().ForEach([&AllDestroyed](const Entity& Entity, const CollisionComponent& Collision, const HealthComponent& Health)
	{
		AllDestroyed &= (Collision.Channel != CollisionChannel::Brick || Health.CurrentHealth <= 0);
	});
	return AllDestroyed;
}

bool RunControllerSystem::HasPlayerLost(const SystemContext& Context) const
{
	bool TriggerHasHealth = true;
	Context.EntityAdmin.GetGroup<CollisionComponent, HealthComponent>().ForEach([&TriggerHasHealth](const Entity& Entity, const CollisionComponent& Collision, const HealthComponent& Health)
	{
		if (Collision.Channel == CollisionChannel::Trigger)
		{
			TriggerHasHealth &= (Health.CurrentHealth > 0);
		}
	});
	return !TriggerHasHealth;
}

bool RunControllerSystem::AdvanceToNextStage(const SystemContext& Context, int CurrentStageId) const
{
	CleanupCurrentStage(Context);
	
	const auto NextStageNumber = CurrentStageId + 1;
	const auto StageDataOpt = StageDataLoader::LoadStageByNumber(NextStageNumber);
	if (!StageDataOpt.has_value())
	{
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "RunControllerSystem::AdvanceToNextStage -> No more stages found. Victory!");
		return false;
	}

	SpawnStageEntities(Context, StageDataOpt.value());
	SetStageData(Context, StageDataOpt.value());
	return true;
}

void RunControllerSystem::SpawnStageEntities(const SystemContext& Context, const StageData& StageData) const
{
	auto& Admin = Context.EntityAdmin;
	auto& TexManager = Context.Managers.TextureManager;
	
	for (const auto& WallData : StageData.Walls)
	{
		StageUtils::AddWall(Context, WallData);
	}
	
	for (const auto& BrickData : StageData.Bricks)
	{
		StageUtils::AddBrick(Context, BrickData);
	}
	
	StageUtils::AddTrigger(Context, StageData.Trigger);
	StageUtils::AddPlayer(Context, StageData.PlayerData);
	StageUtils::AddBall(Context, StageData.BallData);
}

void RunControllerSystem::SetStageData(const SystemContext& Context, const StageData& StageData) const
{
	const auto StageDataGroup = Context.EntityAdmin.GetGroup<StageDataComponent>();
	assert(StageDataGroup.Size() == 1 && "Expected exactly one StageDataComponent in the world");
	if (StageDataGroup.Empty())
		return;

	auto& StageDataComp = Context.EntityAdmin.AccessComponent<StageDataComponent>(StageDataGroup[0]);
	StageDataComp.CurrentStageData = StageData;
}

void RunControllerSystem::CleanupCurrentStage(const SystemContext& Context) const
{
	Context.EntityAdmin.GetGroup<CollisionComponent>().ForEach([&Context](const Entity& Entity, const CollisionComponent& Collision)
	{
		Context.EntityAdmin.DestroyEntity(Entity);
	});
}