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

namespace
{
	void TryAddTexture(const SystemContext& Context, const Entity& Entity, const TextureData& Data)
	{
		if (Data.Path.empty())
			return;

		auto Texture = Context.TextureManager.GetTexture(Data.Path);
		if (Texture == nullptr)
			return;

		Context.EntityAdmin.AddComponent<TextureComponent>(Entity, TextureComponent{
			Texture,
			Data.SourceRect
		});
	}

	void AddWall(const SystemContext& Context, const WallData& WallData)
	{
		auto& Admin = Context.EntityAdmin;
		auto& TexManager = Context.TextureManager;

		auto Wall = Admin.CreateEntity();
		Admin.AddComponent<TransformComponent>(Wall, WallData.Position);
		Admin.AddComponent<RectComponent>(Wall, SDL_FRect{ -WallData.Size.X * 0.5f, -WallData.Size.Y * 0.5f, WallData.Size.X, WallData.Size.Y });
		Admin.AddComponent<ColorComponent>(Wall, SDL_FColor{ 0.3f, 0.3f, 0.3f, 1.f });
		Admin.AddComponent<RenderComponent>(Wall);
		
		TryAddTexture(Context, Wall, WallData.TextureData);
		
		auto& WallCollisionComponent = Admin.AddComponent<CollisionComponent>(Wall, CollisionChannel::Static);
		WallCollisionComponent.ResponseTable[ChannelToIndex(CollisionChannel::Static)] = CollisionResponse::Ignore;
		WallCollisionComponent.ResponseTable[ChannelToIndex(CollisionChannel::Trigger)] = CollisionResponse::Ignore;
	}

	void AddTrigger(const SystemContext& Context, const TriggerData& TriggerData)
	{
		auto& Admin = Context.EntityAdmin;

		auto TriggerEntity = Admin.CreateEntity();
		Admin.AddComponent<TransformComponent>(TriggerEntity, TriggerData.PositionSize.Position);
		Admin.AddComponent<RectComponent>(TriggerEntity, SDL_FRect{ -TriggerData.PositionSize.Size.X * 0.5f, -TriggerData.PositionSize.Size.Y * 0.5f, TriggerData.PositionSize.Size.X, TriggerData.PositionSize.Size.Y });
		Admin.AddComponent<HealthComponent>(TriggerEntity, TriggerData.Health);
		auto& TriggerCollisionComponent = Admin.AddComponent<CollisionComponent>(TriggerEntity, CollisionChannel::Trigger);
		TriggerCollisionComponent.ResponseTable[ChannelToIndex(CollisionChannel::Static)] = CollisionResponse::Ignore;
	}

	void AddBrick(const SystemContext& Context, const BrickData& BrickData)
	{
		auto& Admin = Context.EntityAdmin;
		auto& TexManager = Context.TextureManager;

		auto BrickEntity = Admin.CreateEntity();
		Admin.AddComponent<TransformComponent>(BrickEntity, BrickData.PositionSize.Position);
		Admin.AddComponent<RectComponent>(BrickEntity, SDL_FRect{ -BrickData.PositionSize.Size.X * 0.5f, -BrickData.PositionSize.Size.Y * 0.5f, BrickData.PositionSize.Size.X, BrickData.PositionSize.Size.Y });
		Admin.AddComponent<RenderComponent>(BrickEntity);
		
		TryAddTexture(Context, BrickEntity, BrickData.TextureData);
		
		auto& BrickCollisionComponent = Admin.AddComponent<CollisionComponent>(BrickEntity, CollisionChannel::Brick);
		BrickCollisionComponent.ResponseTable[ChannelToIndex(CollisionChannel::Ball)] = CollisionResponse::Ignore;
		Admin.AddComponent<HealthComponent>(BrickEntity, BrickData.Health);
	}

	void AddPlayer(const SystemContext& Context, const StageData& StageData)
	{
		auto& Admin = Context.EntityAdmin;

		auto PlayerEntity = Admin.CreateEntity();
		Admin.AddComponent<TransformComponent>(PlayerEntity, StageData.PlayerData.PositionSize.Position);
		Admin.AddComponent<RectComponent>(PlayerEntity, SDL_FRect{ -StageData.PlayerData.PositionSize.Size.X * 0.5f, -StageData.PlayerData.PositionSize.Size.Y * 0.5f, StageData.PlayerData.PositionSize.Size.X, StageData.PlayerData.PositionSize.Size.Y });
		Admin.AddComponent<RenderComponent>(PlayerEntity);
		Admin.AddComponent<VelocityComponent>(PlayerEntity);
		auto& PlayerCollisionComponent = Admin.AddComponent<CollisionComponent>(PlayerEntity, CollisionChannel::Player);
		PlayerCollisionComponent.ResponseTable[ChannelToIndex(CollisionChannel::Ball)] = CollisionResponse::Ignore;
		PlayerCollisionComponent.ResponseTable[ChannelToIndex(CollisionChannel::Trigger)] = CollisionResponse::Ignore;
		Admin.AddComponent<PlayerControllerComponent>(PlayerEntity);

		TryAddTexture(Context, PlayerEntity, StageData.PlayerData.TextureData);
	}

	void AddBall(const SystemContext& Context, const BallData& BallData)
	{
		auto& Admin = Context.EntityAdmin;

		auto BallEntity = Admin.CreateEntity();
		Admin.AddComponent<TransformComponent>(BallEntity, BallData.Position);
		Admin.AddComponent<CircleComponent>(BallEntity, BallData.Radius);
		Admin.AddComponent<RenderComponent>(BallEntity);
		Admin.AddComponent<VelocityComponent>(BallEntity, BallData.Velocity);
		auto& BallCollisionComponent = Admin.AddComponent<CollisionComponent>(BallEntity, CollisionChannel::Ball);
		BallCollisionComponent.ResponseTable[ChannelToIndex(CollisionChannel::Trigger)] = CollisionResponse::Ignore;

		TryAddTexture(Context, BallEntity, BallData.TextureData);
	}
}

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

	// Move forward from stage 0
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

	// TODO: load from file instead of hardcoding
	constexpr float WallThickness = 20.f;
	constexpr float ScreenWidth = 640.f;
	constexpr float ScreenHeight = 480.f;
	constexpr int StagesCount = 3;

	constexpr Vector2D<float> BallInitialPosition = { ScreenWidth * 0.5f, ScreenHeight * 0.5f };
	constexpr Vector2D<float> BallInitialVelocity = { 0.f, 250.f };
	constexpr Vector2D<float> PlayerInitialPosition = { ScreenWidth * 0.5f, ScreenHeight * 0.9f };

	// Create brick grid
	constexpr int BrickRows = 5;
	constexpr int BrickColumns = 5;
	constexpr float BrickWidth = 64.f;
	constexpr float BrickHeight = 32.f;
	constexpr float BrickSpacing = 8.f;
	constexpr float GridStartX = (ScreenWidth - (BrickColumns * (BrickWidth + BrickSpacing))) * 0.5f;
	constexpr float GridStartY = WallThickness + BrickSpacing;

	auto StageDataEntity = Admin.CreateEntity();
	auto& StageDataEntityComponent = Admin.AddComponent<StageDataComponent>(StageDataEntity);

	for (int i = 0; i < StagesCount; ++i)
	{
		StageData NewStageData;
		NewStageData.PlayerData.PositionSize.Position = PlayerInitialPosition;
		NewStageData.PlayerData.PositionSize.Size = Vector2D<float>{ 80.f, 20.f };
		NewStageData.PlayerData.TextureData.Path = "../Assets/Textures/paddles_and_balls.png";
		NewStageData.PlayerData.TextureData.SourceRect = SDL_FRect{ 0.f, 7.f, 32.f, 8.f };

		NewStageData.BallData.Position = BallInitialPosition;
		NewStageData.BallData.Velocity = BallInitialVelocity;
		NewStageData.BallData.Radius = 10.f;
		NewStageData.BallData.TextureData.Path = "../Assets/Textures/paddles_and_balls.png";
		NewStageData.BallData.TextureData.SourceRect = SDL_FRect{ 160.f, 5.f, 10.f, 10.f };

		NewStageData.Walls.push_back(WallData{ Vector2D<float>{ ScreenWidth * 0.5f, WallThickness * 0.5f }, Vector2D<float>{ ScreenWidth, WallThickness } });
		NewStageData.Walls.push_back(WallData{ Vector2D<float>{ WallThickness * 0.5f, ScreenHeight * 0.5f }, Vector2D<float>{ WallThickness, ScreenHeight } });
		NewStageData.Walls.push_back(WallData{ Vector2D<float>{ ScreenWidth - WallThickness * 0.5f, ScreenHeight * 0.5f }, Vector2D<float>{ WallThickness, ScreenHeight } });

		NewStageData.Trigger = TriggerData{ Vector2D<float>{ ScreenWidth * 0.5f, ScreenHeight - WallThickness * 0.5f }, Vector2D<float>{ ScreenWidth, WallThickness } };

		for (int Row = 0; Row < BrickRows; ++Row)
		{
			for (int Col = 0; Col < BrickColumns; ++Col)
			{
				BrickData NewBrick;
				NewBrick.PositionSize.Position = Vector2D<float>{ GridStartX + Col * (BrickWidth + BrickSpacing) + BrickWidth * 0.5f, GridStartY + Row * (BrickHeight + BrickSpacing) + BrickHeight * 0.5f };
				NewBrick.PositionSize.Size = Vector2D<float>{ BrickWidth, BrickHeight };
				NewBrick.Health = 1;
				NewBrick.TextureData.Path = "../Assets/Textures/bricks.png";
				NewBrick.TextureData.SourceRect = SDL_FRect{ 0.f, 23.f + static_cast<float>(16.f * Row), 32.f, 8.f};
				NewStageData.Bricks.push_back(NewBrick);
			}
		}
		
		StageDataEntityComponent.Stages.push_back(NewStageData);
	}
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
	// TODO: Transition to victory state? For now mainmenu
	GameStateUtils::RequestStateChange(Context, GameState::MainMenu);
}

void RunControllerSystem::HandleRunDefeat(const SystemContext& Context) const
{
	CleanupRun(Context);
	// TODO: Transition to defeat state? For now mainmenu
	GameStateUtils::RequestStateChange(Context, GameState::MainMenu);
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
	
	bool AdvancementSuccessful = false;

	auto& Admin = Context.EntityAdmin;
	Admin.GetGroup<StageDataComponent>().ForEach([this, &Context, CurrentStageId, &AdvancementSuccessful](const Entity& StageEntity, StageDataComponent& StageData)
	{
		if (!AdvancementSuccessful && StageData.Stages.size() > CurrentStageId)
		{
			// Current not next due to array offset
			SpawnStageEntities(Context, StageData.Stages[CurrentStageId]);
			AdvancementSuccessful = true;
		}
	});

	return AdvancementSuccessful;
}

void RunControllerSystem::SpawnStageEntities(const SystemContext& Context, const StageData& StageData) const
{
	auto& Admin = Context.EntityAdmin;
	auto& TexManager = Context.TextureManager;
	
	for (const auto& WallData : StageData.Walls)
	{
		AddWall(Context, WallData);
	}
	
	for (const auto& BrickData : StageData.Bricks)
	{
		AddBrick(Context, BrickData);
	}
	
	AddTrigger(Context, StageData.Trigger);
	AddPlayer(Context, StageData);
	AddBall(Context, StageData.BallData);
}

void RunControllerSystem::CleanupCurrentStage(const SystemContext& Context) const
{
	Context.EntityAdmin.GetGroup<CollisionComponent>().ForEach([&Context](const Entity& Entity, const CollisionComponent& Collision)
	{
		Context.EntityAdmin.DestroyEntity(Entity);
	});
}