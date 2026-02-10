#include "RunControllerSystem.h"
#include "SystemContext.h"
#include "EventBus.h"
#include "RunStateComponent.h"
#include "StageDataComponent.h"
#include "EntityAdmin.h"
#include "CollisionComponent.h"
#include "TransformComponent.h"
#include <ShapeComponents.h>
#include <RenderComponent.h>
#include <TextureComponent.h>
#include "HealthComponent.h"
#include "VelocityComponent.h"
#include "PlayerControllerComponent.h"
#include "HealthChangedEvent.h"
#include "TextureManager.h"

namespace
{
	void AddWall(EntityAdmin& Admin, TextureManager& TexManager, const WallData& WallData)
	{
		auto Wall = Admin.CreateEntity();
		Admin.AddComponent<TransformComponent>(Wall, WallData.Position);
		Admin.AddComponent<RectComponent>(Wall, SDL_FRect{ -WallData.Size.X * 0.5f, -WallData.Size.Y * 0.5f, WallData.Size.X, WallData.Size.Y });
		Admin.AddComponent<ColorComponent>(Wall, SDL_FColor{ 0.3f, 0.3f, 0.3f, 1.f });
		Admin.AddComponent<RenderComponent>(Wall);
		
		if (!WallData.TexturePath.empty())
		{
			SDL_Texture* Texture = TexManager.GetTexture(WallData.TexturePath);
			if (Texture)
			{
				Admin.AddComponent<TextureComponent>(Wall, TextureComponent{
					Texture,
					WallData.TextureSourceRect
				});
			}
		}
		
		auto& WallCollisionComponent = Admin.AddComponent<CollisionComponent>(Wall, CollisionChannel::Static);
		WallCollisionComponent.ResponseTable[ChannelToIndex(CollisionChannel::Static)] = CollisionResponse::Ignore;
		WallCollisionComponent.ResponseTable[ChannelToIndex(CollisionChannel::Trigger)] = CollisionResponse::Ignore;
	}

	void AddTrigger(EntityAdmin& Admin, const TriggerData& TriggerData)
	{
		auto TriggerEntity = Admin.CreateEntity();
		Admin.AddComponent<TransformComponent>(TriggerEntity, TriggerData.Position);
		Admin.AddComponent<RectComponent>(TriggerEntity, SDL_FRect{ -TriggerData.Size.X * 0.5f, -TriggerData.Size.Y * 0.5f, TriggerData.Size.X, TriggerData.Size.Y });
		Admin.AddComponent<HealthComponent>(TriggerEntity, TriggerData.Health);
		auto& TriggerCollisionComponent = Admin.AddComponent<CollisionComponent>(TriggerEntity, CollisionChannel::Trigger);
		TriggerCollisionComponent.ResponseTable[ChannelToIndex(CollisionChannel::Static)] = CollisionResponse::Ignore;
	}

	void AddBrick(EntityAdmin& Admin, TextureManager& TexManager, const BrickData& BrickData)
	{
		auto BrickEntity = Admin.CreateEntity();
		Admin.AddComponent<TransformComponent>(BrickEntity, BrickData.Position);
		Admin.AddComponent<RectComponent>(BrickEntity, SDL_FRect{ -BrickData.Size.X * 0.5f, -BrickData.Size.Y * 0.5f, BrickData.Size.X, BrickData.Size.Y });
		Admin.AddComponent<RenderComponent>(BrickEntity);
		
		if (!BrickData.TexturePath.empty())
		{
			SDL_Texture* Texture = TexManager.GetTexture(BrickData.TexturePath);
			if (Texture)
			{
				Admin.AddComponent<TextureComponent>(BrickEntity, TextureComponent{
					Texture,
					BrickData.TextureSourceRect
				});
			}
		}
		else
		{
			Admin.AddComponent<ColorComponent>(BrickEntity, BrickData.Color);
		}
		
		auto& BrickCollisionComponent = Admin.AddComponent<CollisionComponent>(BrickEntity, CollisionChannel::Brick);
		BrickCollisionComponent.ResponseTable[ChannelToIndex(CollisionChannel::Ball)] = CollisionResponse::Ignore;
		Admin.AddComponent<HealthComponent>(BrickEntity, BrickData.Health);
	}

	void AddBackgroundRenderEntity(const SystemContext& Context)
	{
		auto& Admin = Context.EntityAdmin;
		
		int WindowWidth, WindowHeight;
		SDL_GetWindowSize(&Context.Window, &WindowWidth, &WindowHeight);

		auto BackgroundEntity = Admin.CreateEntity();
		Admin.AddComponent<TransformComponent>(BackgroundEntity);
		Admin.AddComponent<RectComponent>(BackgroundEntity, SDL_FRect{ 
			0.f, 
			0.f, 
			static_cast<float>(WindowWidth), 
			static_cast<float>(WindowHeight)
		});
		Admin.AddComponent<RenderComponent>(BackgroundEntity, -1);
		
		auto Texture = Context.TextureManager.LoadTexture("../Assets/Textures/Background_Tiles.png");
		if (Texture == nullptr)
			return;

		Admin.AddComponent<TextureComponent>(BackgroundEntity, TextureComponent{ Texture, {32.f, 20.f, 31.f, 25.f} });
	}

	void AddPlayer(EntityAdmin& Admin, const StageData& StageData)
	{
		auto PlayerEntity = Admin.CreateEntity();
		Admin.AddComponent<TransformComponent>(PlayerEntity, StageData.PlayerSpawnPosition);
		Admin.AddComponent<RectComponent>(PlayerEntity, SDL_FRect{ -60.f, -10.f, 80.f, 20.f });
		Admin.AddComponent<RenderComponent>(PlayerEntity);
		Admin.AddComponent<VelocityComponent>(PlayerEntity);
		auto& PlayerCollisionComponent = Admin.AddComponent<CollisionComponent>(PlayerEntity, CollisionChannel::Player);
		PlayerCollisionComponent.ResponseTable[ChannelToIndex(CollisionChannel::Ball)] = CollisionResponse::Ignore;
		PlayerCollisionComponent.ResponseTable[ChannelToIndex(CollisionChannel::Trigger)] = CollisionResponse::Ignore;
		Admin.AddComponent<PlayerControllerComponent>(PlayerEntity);
	}

	void AddBall(EntityAdmin& Admin, const BallData& BallData)
	{
		auto BallEntity = Admin.CreateEntity();
		Admin.AddComponent<TransformComponent>(BallEntity, BallData.Position);
		Admin.AddComponent<CircleComponent>(BallEntity, 10.f);
		Admin.AddComponent<ColorComponent>(BallEntity, SDL_FColor{ 0.f, 0.f, 1.f, 1.f });
		Admin.AddComponent<RenderComponent>(BallEntity);
		Admin.AddComponent<VelocityComponent>(BallEntity, BallData.Velocity);
		auto& BallCollisionComponent = Admin.AddComponent<CollisionComponent>(BallEntity, CollisionChannel::Ball);
		BallCollisionComponent.ResponseTable[ChannelToIndex(CollisionChannel::Trigger)] = CollisionResponse::Ignore;
	}
}

void RunControllerSystem::Initialize(const SystemContext& Context) const
{
	Context.EventBus.Subscribe<HealthChangedEvent>(this, [this](const SystemContext& Context, const HealthChangedEvent& Event)
	{
		OnHealthChanged(Context, Event);
		return;
	});

	AddBackgroundRenderEntity(Context);

	// TODO: change, for now simulte starting game from here
	AdvanceToNextStage(Context, 0);
}

void RunControllerSystem::OnHealthChanged(const SystemContext& Context, const HealthChangedEvent& Event) const
{
	if (Event.Delta > 0)
		return;

	if (!Context.EntityAdmin.HasComponent<CollisionComponent>(Event.TargetEntity))
		return;

	const auto& Collision = Context.EntityAdmin.GetComponent<CollisionComponent>(Event.TargetEntity);
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
	Context.EntityAdmin.GetGroup<RunStateComponent>().ForEach([this, &Context](const Entity& RunStateEntity, RunStateComponent& RunStateComponent)
	{
		RunStateComponent.CurrentStage++;
		if (!AdvanceToNextStage(Context, RunStateComponent.CurrentStage))
		{
			HandleRunVictory(Context);
		}
	});
}

void RunControllerSystem::HandleRunVictory(const SystemContext& Context) const
{
	// TODO: implement
}

void RunControllerSystem::HandleRunDefeat(const SystemContext& Context) const
{
	// TODO: implement
}

bool RunControllerSystem::AreAllBricksDestroyed(const SystemContext& Context) const
{
	bool AllDestroyed = true;
	Context.EntityAdmin.GetGroup<CollisionComponent, HealthComponent>().ForEach([&AllDestroyed](const Entity& TargetEntity, const CollisionComponent& Collision, const HealthComponent& Health)
	{
		AllDestroyed &= (Collision.Channel != CollisionChannel::Brick || Health.CurrentHealth <= 0);
	});
	return AllDestroyed;
}

bool RunControllerSystem::HasPlayerLost(const SystemContext& Context) const
{
	bool TriggerHasHealth = true;
	Context.EntityAdmin.GetGroup<CollisionComponent, HealthComponent>().ForEach([&TriggerHasHealth](const Entity& TargetEntity, const CollisionComponent& Collision, const HealthComponent& Health)
	{
		if (Collision.Channel == CollisionChannel::Trigger)
		{
			TriggerHasHealth &= (Health.CurrentHealth > 0);
		}
	});
	return !TriggerHasHealth;
}

bool RunControllerSystem::AdvanceToNextStage(const SystemContext& Context, int NextStageId) const
{
	CleanupCurrentStage(Context);
	
	bool AdvancementSuccessful = false;

	auto& Admin = Context.EntityAdmin;
	Admin.GetGroup<StageDataComponent>().ForEach([this, &Context, NextStageId, &AdvancementSuccessful](const Entity& StageEntity, StageDataComponent& StageData)
	{
		if (!AdvancementSuccessful && StageData.Stages.size() > NextStageId)
		{
			SpawnStageEntities(Context, StageData.Stages[NextStageId]);
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
		AddWall(Admin, TexManager, WallData);
	}
	
	for (const auto& BrickData : StageData.Bricks)
	{
		AddBrick(Admin, TexManager, BrickData);
	}
	
	AddTrigger(Admin, StageData.Trigger);
	AddPlayer(Admin, StageData);
	AddBall(Admin, StageData.BallData);
}

void RunControllerSystem::CleanupCurrentStage(const SystemContext& Context) const
{
	std::vector<Entity> EntitiesToDestroy;
	auto& Admin = Context.EntityAdmin;
	Admin.GetGroup<CollisionComponent>().ForEach([&EntitiesToDestroy](const Entity& TargetEntity, const CollisionComponent& Collision)
	{
		EntitiesToDestroy.push_back(TargetEntity);
	});

	for (const auto& TargetEntity : EntitiesToDestroy)
	{
		Admin.DestroyEntity(TargetEntity);
	}
}