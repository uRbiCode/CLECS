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
#include "TextureManager.h"
#include "RenderConstants.h"
#include "GameStateEvents.h"
#include "GameStateUtils.h"
#include "StageDataLoader.h"
#include "StageUtils.h"
#include "StageEndEvent.h"
#include "ChangeRunStateEvent.h"
#include "UpgradeLoader.h"
#include "UpgradeEvent.h"
#include "UpgradeUtils.h"
#include <cassert>

namespace
{
	constexpr int InitialPlayerHealth = 3;
}

void RunControllerSystem::Initialize(const SystemContext& Context) const
{
	Context.EventBus.Subscribe<GameStateBeginEvent>(this, [this](const SystemContext& Context, const GameStateBeginEvent& Event)
	{
		if (Event.BeginningState != GameState::Run)
			return;

		BeginRun(Context);
	});

	Context.EventBus.Subscribe<StageEndEvent>(this, [this](const SystemContext& Context, const StageEndEvent& Event)
	{
		if (!Event.Victory)
		{
			HandleRunDefeat(Context);
			return;
		}

		HandleStageCleared(Context);
	});

	Context.EventBus.Subscribe<UpgradeSelectedEvent>(this, [this](const SystemContext& Context, const UpgradeSelectedEvent& Event)
	{
		TryStartNextStage(Context);
	});
}

void RunControllerSystem::BeginRun(const SystemContext& Context) const
{
	AddRunStateComponent(Context);
	AddStageDataComponent(Context);

	TryStartNextStage(Context);
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
	Admin.AddComponent<RunStateComponent>(RunStateEntity, RunStateComponent { RunState::Stage });
	Admin.AddComponent<HealthComponent>(RunStateEntity, HealthComponent{ InitialPlayerHealth });
	Admin.AddComponent<AvailableUpgradesComponent>(RunStateEntity, std::move(UpgradeLoader::LoadUpgradeDefinitions()));
	Admin.AddComponent<OwnedUpgradesComponent>(RunStateEntity);
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

void RunControllerSystem::HandleStageCleared(const SystemContext& Context) const
{
	CleanupCurrentStage(Context);

	const auto RunStateGroup = Context.EntityAdmin.GetGroup<RunStateComponent>();
	assert(RunStateGroup.Size() == 1 && "Expected exactly one RunStateComponent in the world");
	if (RunStateGroup.Empty())
		return;

	auto& RunStateComp = Context.EntityAdmin.AccessComponent<RunStateComponent>(RunStateGroup[0]);
	const auto NextStageNumber = RunStateComp.CurrentStage + 1;

	if (StageDataLoader::IsStageDataAvailable(NextStageNumber) && AreUpgradesAvailable(Context))
	{
		ChangeRunState(Context, RunState::UpgradeSelection);
		return;
	}

	TryStartNextStage(Context);
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

bool RunControllerSystem::AdvanceToNextStage(const SystemContext& Context, int CurrentStageId) const
{
	const auto NextStageNumber = CurrentStageId + 1;
	const auto StageDataOpt = StageDataLoader::LoadStageDataByNumber(NextStageNumber);
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
	StageUtils::AddPlayer(Context, UpgradeUtils::GetModifiedPlayerData(Context, StageData.PlayerData));
	StageUtils::AddBall(Context, UpgradeUtils::GetModifiedBallData(Context, StageData.BallData));
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

bool RunControllerSystem::AreUpgradesAvailable(const SystemContext& Context) const
{
	const auto AvailableUpgradesGroup = Context.EntityAdmin.GetGroup<AvailableUpgradesComponent>();
	assert(AvailableUpgradesGroup.Size() == 1 && "Expected exactly one AvailableUpgradesComponent in the world");
	if (AvailableUpgradesGroup.Empty())
		return false;

	return !Context.EntityAdmin.AccessComponent<AvailableUpgradesComponent>(AvailableUpgradesGroup[0]).AvailableUpgrades.empty();
}

void RunControllerSystem::ChangeRunState(const SystemContext& Context, RunState NewState) const
{
	Context.EventBus.Notify(Context, ChangeRunStateEvent{ NewState });
}

void RunControllerSystem::TryStartNextStage(const SystemContext& Context) const
{
	const auto RunStateGroup = Context.EntityAdmin.GetGroup<RunStateComponent>();
	assert(RunStateGroup.Size() == 1 && "Expected exactly one RunStateComponent in the world");
	if (RunStateGroup.Empty())
		return;

	auto& RunStateComp = Context.EntityAdmin.AccessComponent<RunStateComponent>(RunStateGroup[0]);
	if (AdvanceToNextStage(Context, RunStateComp.CurrentStage))
	{
		RunStateComp.CurrentStage++;
		ChangeRunState(Context, RunState::Stage);
	}
	else
	{
		HandleRunVictory(Context);
	}
}