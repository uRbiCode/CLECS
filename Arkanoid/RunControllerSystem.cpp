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
#include "PlayerReadyEvent.h"
#include <cassert>

namespace
{
	constexpr int InitialPlayerHealth = 3;

	void ChangeRunState(const SystemContext& Context, RunState NewState)
	{
		const auto RunStateGroup = Context.EntityAdmin.GetGroup<RunStateComponent>();
		assert(RunStateGroup.Size() == 1 && "Expected exactly one RunStateComponent in the world");
		if (RunStateGroup.Empty())
			return;

		auto& RunStateComp = Context.EntityAdmin.AccessComponent<RunStateComponent>(RunStateGroup[0]);
		RunStateComp.State = NewState;
		Context.EventBus.Notify(Context, ChangeRunStateEvent{ NewState });
	}

	void RemoveRunStateComponent(const SystemContext& Context)
	{
		Context.EntityAdmin.GetGroup<RunStateComponent>().ForEach([&Context](const Entity& Entity, const RunStateComponent& RunState)
		{
			Context.EntityAdmin.DestroyEntity(Entity);
		});
	}

	void RemoveStageDataComponent(const SystemContext& Context)
	{
		Context.EntityAdmin.GetGroup<StageDataComponent>().ForEach([&Context](const Entity& Entity, const StageDataComponent& StageData)
		{
			Context.EntityAdmin.DestroyEntity(Entity);
		});
	}

	void CleanupCurrentStage(const SystemContext& Context)
	{
		Context.EntityAdmin.GetGroup<CollisionComponent>().ForEach([&Context](const Entity& Entity, const CollisionComponent& Collision)
		{
			Context.EntityAdmin.DestroyEntity(Entity);
		});
	}

	void CleanupRun(const SystemContext& Context)
	{
		CleanupCurrentStage(Context);
		RemoveRunStateComponent(Context);
		RemoveStageDataComponent(Context);
	}

	void SpawnStageEntities(const SystemContext& Context, const StageData& StageData)
	{
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

	void SetStageData(const SystemContext& Context, const StageData& StageData)
	{
		const auto StageDataGroup = Context.EntityAdmin.GetGroup<StageDataComponent>();
		assert(StageDataGroup.Size() == 1 && "Expected exactly one StageDataComponent in the world");
		if (StageDataGroup.Empty())
			return;

		auto& StageDataComp = Context.EntityAdmin.AccessComponent<StageDataComponent>(StageDataGroup[0]);
		StageDataComp.CurrentStageData = StageData;
	}

	bool AreUpgradesAvailable(const SystemContext& Context)
	{
		const auto AvailableUpgradesGroup = Context.EntityAdmin.GetGroup<AvailableUpgradesComponent>();
		assert(AvailableUpgradesGroup.Size() == 1 && "Expected exactly one AvailableUpgradesComponent in the world");
		if (AvailableUpgradesGroup.Empty())
			return false;

		return !Context.EntityAdmin.AccessComponent<AvailableUpgradesComponent>(AvailableUpgradesGroup[0]).AvailableUpgrades.empty();
	}

	// Returns whether advancement was successful or was it the last stage already.
	bool AdvanceToNextStage(const SystemContext& Context, int CurrentStageId)
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

	void HandleRunVictory(const SystemContext& Context)
	{
		CleanupRun(Context);
		GameStateUtils::RequestStateChange(Context, GameState::Victory);
	}

	void HandleRunDefeat(const SystemContext& Context)
	{
		CleanupRun(Context);
		GameStateUtils::RequestStateChange(Context, GameState::Defeat);
	}

	void TryStartNextStage(const SystemContext& Context)
	{
		const auto RunStateGroup = Context.EntityAdmin.GetGroup<RunStateComponent>();
		assert(RunStateGroup.Size() == 1 && "Expected exactly one RunStateComponent in the world");
		if (RunStateGroup.Empty())
			return;

		auto& RunStateComp = Context.EntityAdmin.AccessComponent<RunStateComponent>(RunStateGroup[0]);
		if (AdvanceToNextStage(Context, RunStateComp.CurrentStage))
		{
			RunStateComp.CurrentStage++;
			ChangeRunState(Context, RunState::PlayerPrepare);
		}
		else
		{
			HandleRunVictory(Context);
		}
	}

	void HandleStageCleared(const SystemContext& Context)
	{
		CleanupCurrentStage(Context);

		const auto RunStateGroup = Context.EntityAdmin.GetGroup<RunStateComponent>();
		assert(RunStateGroup.Size() == 1 && "Expected exactly one RunStateComponent in the world");
		if (RunStateGroup.Empty())
		{
			HandleRunVictory(Context);
			return;
		}

		auto& RunStateComp = Context.EntityAdmin.AccessComponent<RunStateComponent>(RunStateGroup[0]);
		const auto NextStageNumber = RunStateComp.CurrentStage + 1;

		if (StageDataLoader::IsStageDataAvailable(NextStageNumber) && AreUpgradesAvailable(Context))
		{
			ChangeRunState(Context, RunState::UpgradeSelection);
			return;
		}

		TryStartNextStage(Context);
	}

	void AddRunStateComponent(const SystemContext& Context)
	{
		auto& Admin = Context.EntityAdmin;
		const auto RunStateEntity = Admin.CreateEntity();
		Admin.AddComponent<RunStateComponent>(RunStateEntity, RunStateComponent{ RunState::PlayerPrepare });
		Admin.AddComponent<HealthComponent>(RunStateEntity, HealthComponent{ InitialPlayerHealth });
		Admin.AddComponent<AvailableUpgradesComponent>(RunStateEntity, std::move(UpgradeLoader::LoadUpgradeDefinitions()));
		Admin.AddComponent<OwnedUpgradesComponent>(RunStateEntity);
	}

	void AddStageDataComponent(const SystemContext& Context)
	{
		auto& Admin = Context.EntityAdmin;
		const auto StageDataEntity = Admin.CreateEntity();
		Admin.AddComponent<StageDataComponent>(StageDataEntity);
	}

	void BeginRun(const SystemContext& Context)
	{
		AddRunStateComponent(Context);
		AddStageDataComponent(Context);
		TryStartNextStage(Context);
	}

	void OnPlayerReady(const SystemContext& Context)
	{
		ChangeRunState(Context, RunState::Stage);
	}
}

void RunControllerSystem::Initialize(const SystemContext& Context)
{
	const void* const Id = reinterpret_cast<const void*>(&Initialize);

	Context.EventBus.Subscribe<GameStateBeginEvent>(Id, [](const SystemContext& Context, const GameStateBeginEvent& Event)
	{
		if (Event.BeginningState != GameState::Run)
			return;

		BeginRun(Context);
	});

	Context.EventBus.Subscribe<StageEndEvent>(Id, [](const SystemContext& Context, const StageEndEvent& Event)
	{
		if (!Event.Victory)
		{
			HandleRunDefeat(Context);
			return;
		}

		HandleStageCleared(Context);
	});

	Context.EventBus.Subscribe<UpgradeSelectedEvent>(Id, [](const SystemContext& Context, const UpgradeSelectedEvent& Event)
	{
		TryStartNextStage(Context);
	});

	Context.EventBus.Subscribe<PlayerReadyEvent>(Id, [](const SystemContext& Context, const PlayerReadyEvent& Event)
	{
		OnPlayerReady(Context);
	});
}