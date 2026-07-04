#include "RunUtils.h"
#include "Query.h"
#include "UpgradeComponents.h"
#include "SystemContext.h"
#include "PositionComponent.h"
#include "RenderComponents.h"
#include "ShapeComponents.h"
#include "StageDataLoader.h"
#include "CommandRunner.h"
#include "HealthComponent.h"
#include <TextureComponent.h>
#include "TextureManager.h"
#include "DamageComponent.h"
#include "PlayerMoveSpeedComponent.h"
#include "ResetComponents.h"
#include "UpgradeComponents.h"

namespace
{
	namespace Constants
	{
		constexpr int Damage = 1;
		constexpr float PlayerMoveSpeed = 300.f;
	}

	int GetPersistentPlayerHealth(SystemContext& Context)
	{
		int PersistentHealth = 0;
		const Query<WritesList<>, ReadsList<BackgroundRenderComponent, HealthComponent>, ExcludeList<>> HealthQuery(Context.QueryContext);
		HealthQuery.ForEach([&](Entity Entity, const BackgroundRenderComponent& BackgroundRender, const HealthComponent& Health)
		{
			PersistentHealth = Health.CurrentHealth;
		});
		return PersistentHealth;
	}

	float GetPaddleWidthMultiplier(SystemContext& Context)
	{
		float PaddleWidthMultiplier = 1.f;
		const Query<WritesList<>, ReadsList<PaddleWidthMultiplierUpgradeComponent>, ExcludeList<AvailableUpgradeComponent>> PaddleWidthQuery(Context.QueryContext);
		PaddleWidthQuery.ForEach([&](Entity Entity, const PaddleWidthMultiplierUpgradeComponent& PaddleWidthUpgrade)
		{
			PaddleWidthMultiplier *= PaddleWidthUpgrade.Multiplier;
		});
		return PaddleWidthMultiplier;
	}

	float GetBallSpeedMultiplier(SystemContext& Context)
	{
		float BallSpeedMultiplier = 1.f;
		const Query<WritesList<>, ReadsList<BallSpeedMultiplierUpgradeComponent>, ExcludeList<AvailableUpgradeComponent>> BallSpeedQuery(Context.QueryContext);
		BallSpeedQuery.ForEach([&](Entity Entity, const BallSpeedMultiplierUpgradeComponent& BallSpeedUpgrade)
		{
			BallSpeedMultiplier *= BallSpeedUpgrade.Multiplier;
		});
		return BallSpeedMultiplier;
	}

	float GetBallSizeMultiplier(SystemContext& Context)
	{
		float BallSizeMultiplier = 1.f;
		const Query<WritesList<>, ReadsList<BallSizeMultiplierUpgradeComponent>, ExcludeList<AvailableUpgradeComponent>> BallSizeQuery(Context.QueryContext);
		BallSizeQuery.ForEach([&](Entity Entity, const BallSizeMultiplierUpgradeComponent& BallSizeUpgrade)
		{
			BallSizeMultiplier *= BallSizeUpgrade.Multiplier;
		});
		return BallSizeMultiplier;
	}
}

// Since we choose one upgrade after each stage, this is true.
int RunUtils::GetCurrentStageNumber(SystemContext& Context)
{
	const Query<WritesList<>, ReadsList<UpgradeDescriptionComponent>, ExcludeList<AvailableUpgradeComponent>> OwnedUpgradesQuery(Context.QueryContext);
	return OwnedUpgradesQuery.Size() + 1;
}

void RunUtils::SpawnWalls(SystemContext& Context, std::vector<WallData>&& Data)
{
	AddEntitiesCommand<PositionComponent, RectComponent, GameRenderComponent> AddWallsCommand(Data.size());
	for (const WallData& WallData : Data)
	{
		AddWallsCommand.WithEntry(PositionComponent{ WallData.PositionSize.Position },
			RectComponent{ SDL_FRect{ -WallData.PositionSize.Size.X * 0.5f, -WallData.PositionSize.Size.Y * 0.5f, WallData.PositionSize.Size.X, WallData.PositionSize.Size.Y } },
			GameRenderComponent{ SDL_FColor{ 0.3f, 0.3f, 0.3f, 1.f } });
	}
	Context.Commands.Submit(std::move(AddWallsCommand));
}

void RunUtils::SpawnBricks(SystemContext& Context, std::vector<BrickData>&& Data)
{
	AddEntitiesCommand<PositionComponent, RectComponent, HealthComponent, GameRenderComponent, TextureComponent> AddBricksCommand(Data.size());
	for (const BrickData& BrickData : Data)
	{
		AddBricksCommand.WithEntry(PositionComponent{ BrickData.PositionSize.Position },
			RectComponent{ SDL_FRect{ -BrickData.PositionSize.Size.X * 0.5f, -BrickData.PositionSize.Size.Y * 0.5f, BrickData.PositionSize.Size.X, BrickData.PositionSize.Size.Y } },
			HealthComponent{ BrickData.Health },
			GameRenderComponent{ SDL_FColor{ 1.f, 1.f, 1.f, 1.f } },
			TextureComponent{ Context.Managers.TextureManager.GetTexture(BrickData.TextureData.Path), BrickData.TextureData.SourceRect });
	}
	Context.Commands.Submit(std::move(AddBricksCommand));
}

void RunUtils::SpawnTrigger(SystemContext& Context, TriggerData&& Data)
{
	AddEntitiesCommand<PositionComponent, RectComponent, HealthComponent> AddTriggerCommand(1);
	AddTriggerCommand.WithEntry(PositionComponent{ Data.PositionSize.Position },
		RectComponent{ SDL_FRect{ -Data.PositionSize.Size.X * 0.5f, -Data.PositionSize.Size.Y * 0.5f, Data.PositionSize.Size.X, Data.PositionSize.Size.Y } },
		HealthComponent{ GetPersistentPlayerHealth(Context)});
	Context.Commands.Submit(std::move(AddTriggerCommand));
}

void RunUtils::SpawnPlayer(SystemContext& Context, PlayerData&& Data)
{
	Data.PositionSize.Size.X *= GetPaddleWidthMultiplier(Context);

	AddEntitiesCommand<PositionComponent, PositionResetComponent, RectComponent, VelocityResetComponent, PlayerMoveSpeedComponent, GameRenderComponent, TextureComponent> AddPlayerCommand(1);
	AddPlayerCommand.WithEntry(PositionComponent{ Data.PositionSize.Position },
		PositionResetComponent{ Data.PositionSize.Position },
		RectComponent{ SDL_FRect{ -Data.PositionSize.Size.X * 0.5f, -Data.PositionSize.Size.Y * 0.5f, Data.PositionSize.Size.X, Data.PositionSize.Size.Y } },
		VelocityResetComponent{ { 0.f, 0.f } },
		PlayerMoveSpeedComponent{ Constants::PlayerMoveSpeed, 0.f },
		GameRenderComponent{ SDL_FColor{ 1.f, 1.f, 1.f, 1.f } },
		TextureComponent{ Context.Managers.TextureManager.GetTexture(Data.TextureData.Path), Data.TextureData.SourceRect });
	Context.Commands.Submit(std::move(AddPlayerCommand));
}

void RunUtils::SpawnBall(SystemContext& Context, BallData&& Data)
{
	Data.Radius *= GetBallSizeMultiplier(Context);
	Data.Velocity *= GetBallSpeedMultiplier(Context);
	AddEntitiesCommand<PositionComponent, PositionResetComponent, CircleComponent, VelocityResetComponent, GameRenderComponent, TextureComponent, DamageComponent> AddBallCommand(1);
	AddBallCommand.WithEntry(PositionComponent{ Data.Position },
		PositionResetComponent{ Data.Position },
		CircleComponent{ Data.Radius },
		VelocityResetComponent{ Data.Velocity },
		GameRenderComponent{ SDL_FColor{ 1.f, 1.f, 1.f, 1.f } },
		TextureComponent{ Context.Managers.TextureManager.GetTexture(Data.TextureData.Path), Data.TextureData.SourceRect },
		DamageComponent{ Constants::Damage });
	Context.Commands.Submit(std::move(AddBallCommand));
}

void RunUtils::ResetStage(SystemContext& Context)
{
	const Query<WritesList<PositionComponent>, ReadsList<PositionResetComponent>, ExcludeList<>> ResetQuery(Context.QueryContext);
	ResetQuery.ForEach([&](Entity Entity, PositionComponent& Position, const PositionResetComponent& PositionReset)
	{
		Position.Position = PositionReset.ResetPosition;
	});
}