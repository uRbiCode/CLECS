#include "StageUtils.h"
#include "RunStateComponent.h"
#include "EntityAdmin.h"
#include "SystemContext.h"
#include "StageDataComponent.h"
#include "HealthComponent.h"
#include "CollisionComponent.h"
#include "TextureComponent.h"
#include "TextureManager.h"
#include "ShapeComponents.h"
#include "TransformComponent.h"
#include "RenderComponent.h"
#include "VelocityComponent.h"
#include "PlayerControllerComponent.h"
#include <cassert>
#include <optional>

namespace
{
	void TryAddTexture(const SystemContext& Context, const Entity& Entity, const TextureData& Data)
	{
		if (Data.Path.empty())
			return;

		const auto Texture = Context.Managers.TextureManager.GetTexture(Data.Path);
		if (Texture == nullptr)
			return;

		Context.EntityAdmin.AddComponent<TextureComponent>(Entity, TextureComponent{
			Texture,
			Data.SourceRect
		});
	}
}

StageData StageUtils::GetCurrentStageData(const SystemContext& Context)
{
	const auto StageDataGroup = Context.EntityAdmin.GetGroup<StageDataComponent>();
	assert(StageDataGroup.Size() == 1 && "Expected exactly one StageDataComponent in the world");

	if (StageDataGroup.Empty())
		return {};

	return Context.EntityAdmin.GetComponent<StageDataComponent>(StageDataGroup[0]).CurrentStageData;
}

int StageUtils::GetCurrentPlayerHealth(const SystemContext& Context)
{
	std::optional<int> Health = std::nullopt;

	const auto RunStateGroup = Context.EntityAdmin.GetGroup<RunStateComponent, HealthComponent>();
	assert(RunStateGroup.Size() == 1 && "Expected exactly one RunStateComponent with HealthComponent in the world");

	if (!RunStateGroup.Empty())
	{
		Health = Context.EntityAdmin.GetComponent<HealthComponent>(RunStateGroup[0]).CurrentHealth;
	}
	else
	{
		 SDL_LogError(SDL_LOG_CATEGORY_ASSERT, "StageUtils::GetCurrentPlayerHealth -> RunStateComponent with HealthComponent is missing");
	}
	
	return Health.value_or(0);
}

void StageUtils::AddWall(const SystemContext& Context, const WallData& WallData)
{
	auto& Admin = Context.EntityAdmin;
	auto& TexManager = Context.Managers.TextureManager;

	const auto Wall = Admin.CreateEntity();
	Admin.AddComponent<TransformComponent>(Wall, WallData.Position);
	Admin.AddComponent<RectComponent>(Wall, SDL_FRect{ -WallData.Size.X * 0.5f, -WallData.Size.Y * 0.5f, WallData.Size.X, WallData.Size.Y });
	Admin.AddComponent<ColorComponent>(Wall, SDL_FColor{ 0.3f, 0.3f, 0.3f, 1.f });
	Admin.AddComponent<RenderComponent>(Wall);
	
	TryAddTexture(Context, Wall, WallData.TextureData);
	
	auto& WallCollisionComponent = Admin.AddComponent<CollisionComponent>(Wall, CollisionChannel::Static);
	WallCollisionComponent.ResponseTable[ChannelToIndex(CollisionChannel::Static)] = CollisionResponse::Ignore;
	WallCollisionComponent.ResponseTable[ChannelToIndex(CollisionChannel::Trigger)] = CollisionResponse::Ignore;
}

void StageUtils::AddTrigger(const SystemContext& Context, const TriggerData& TriggerData)
{
	auto& Admin = Context.EntityAdmin;

	const auto TriggerEntity = Admin.CreateEntity();
	Admin.AddComponent<TransformComponent>(TriggerEntity, TriggerData.PositionSize.Position);
	Admin.AddComponent<RectComponent>(TriggerEntity, SDL_FRect{ -TriggerData.PositionSize.Size.X * 0.5f, -TriggerData.PositionSize.Size.Y * 0.5f, TriggerData.PositionSize.Size.X, TriggerData.PositionSize.Size.Y });
	auto& TriggerCollisionComponent = Admin.AddComponent<CollisionComponent>(TriggerEntity, CollisionChannel::Trigger);
	TriggerCollisionComponent.ResponseTable[ChannelToIndex(CollisionChannel::Static)] = CollisionResponse::Ignore;
}

void StageUtils::AddBrick(const SystemContext& Context, const BrickData& BrickData)
{
	auto& Admin = Context.EntityAdmin;
	auto& TexManager = Context.Managers.TextureManager;

	const auto BrickEntity = Admin.CreateEntity();
	Admin.AddComponent<TransformComponent>(BrickEntity, BrickData.PositionSize.Position);
	Admin.AddComponent<RectComponent>(BrickEntity, SDL_FRect{ -BrickData.PositionSize.Size.X * 0.5f, -BrickData.PositionSize.Size.Y * 0.5f, BrickData.PositionSize.Size.X, BrickData.PositionSize.Size.Y });
	Admin.AddComponent<RenderComponent>(BrickEntity);
	
	TryAddTexture(Context, BrickEntity, BrickData.TextureData);
	
	auto& BrickCollisionComponent = Admin.AddComponent<CollisionComponent>(BrickEntity, CollisionChannel::Brick);
	BrickCollisionComponent.ResponseTable[ChannelToIndex(CollisionChannel::Ball)] = CollisionResponse::Ignore;
	Admin.AddComponent<HealthComponent>(BrickEntity, BrickData.Health);
}

void StageUtils::AddPlayer(const SystemContext& Context, const PlayerData& PlayerData)
{
	auto& Admin = Context.EntityAdmin;

	const auto PlayerEntity = Admin.CreateEntity();
	Admin.AddComponent<TransformComponent>(PlayerEntity, PlayerData.PositionSize.Position);
	Admin.AddComponent<RectComponent>(PlayerEntity, SDL_FRect{ -PlayerData.PositionSize.Size.X * 0.5f, -PlayerData.PositionSize.Size.Y * 0.5f, PlayerData.PositionSize.Size.X, PlayerData.PositionSize.Size.Y });
	Admin.AddComponent<RenderComponent>(PlayerEntity);
	Admin.AddComponent<VelocityComponent>(PlayerEntity);
	auto& PlayerCollisionComponent = Admin.AddComponent<CollisionComponent>(PlayerEntity, CollisionChannel::Player);
	PlayerCollisionComponent.ResponseTable[ChannelToIndex(CollisionChannel::Ball)] = CollisionResponse::Ignore;
	PlayerCollisionComponent.ResponseTable[ChannelToIndex(CollisionChannel::Trigger)] = CollisionResponse::Ignore;
	Admin.AddComponent<PlayerControllerComponent>(PlayerEntity);

	TryAddTexture(Context, PlayerEntity, PlayerData.TextureData);
}

void StageUtils::AddBall(const SystemContext& Context, const BallData& BallData)
{
	auto& Admin = Context.EntityAdmin;

	const auto BallEntity = Admin.CreateEntity();
	Admin.AddComponent<TransformComponent>(BallEntity, BallData.Position);
	Admin.AddComponent<CircleComponent>(BallEntity, BallData.Radius);
	Admin.AddComponent<RenderComponent>(BallEntity);
	Admin.AddComponent<VelocityComponent>(BallEntity, BallData.Velocity);
	auto& BallCollisionComponent = Admin.AddComponent<CollisionComponent>(BallEntity, CollisionChannel::Ball);
	BallCollisionComponent.ResponseTable[ChannelToIndex(CollisionChannel::Trigger)] = CollisionResponse::Ignore;

	TryAddTexture(Context, BallEntity, BallData.TextureData);
}