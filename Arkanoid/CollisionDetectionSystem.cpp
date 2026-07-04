#include "CollisionDetectionSystem.h"
#include "SystemContext.h"
#include "ShapeComponents.h"
#include "MathTypes.h"
#include "PositionComponent.h"
#include "Query.h"
#include "DamageComponent.h"
#include "HealthComponent.h"
#include "RenderComponents.h"
#include "PlayerMoveSpeedComponent.h"
#include "CommandRunner.h"
#include "CollisionComponents.h"
#include "VelocityComponent.h"
#include "CollisionUtils.h"
#include "ComponentUtils.h"

namespace
{
	void UpdateBallRegularCollision(SystemContext& Context, const BallQuery& BallQuery)
	{
		AddComponentsCommand<CollisionComponent> AddCollisionCommand(0);
		AddComponentsCommand<HealthDeltaComponent> AddHealthDeltaCommand(0);

		const BricksQuery BricksQuery(Context.QueryContext);
		const WallsQuery WallsQuery(Context.QueryContext);

		BallQuery.ForEach([&](Entity BallEntity, const PositionComponent& BallPosition, const CircleComponent& BallCircle, const DamageComponent& BallDamage)
		{
			Vector2D<float> TotalSeparation{ 0.f, 0.f };
			BricksQuery.ForEach([&](Entity BrickEntity, const PositionComponent& BrickPosition, const RectComponent& BrickRect, const HealthComponent& BrickHealth, const GameRenderComponent& BrickRender)
			{
				if (!CollisionUtils::CheckCircleRect(BallPosition, BallCircle, BrickPosition, BrickRect))
					return;
				
				AddHealthDeltaCommand.WithEntry(BrickEntity, HealthDeltaComponent{ -BallDamage.Damage });
				TotalSeparation += CollisionUtils::GetSeparationCircleRect(BallPosition, BallCircle, BrickPosition, BrickRect);
			});

			WallsQuery.ForEach([&](Entity WallEntity, const PositionComponent& WallPosition, const RectComponent& WallRect, const GameRenderComponent& WallRender)
			{
				if (!CollisionUtils::CheckCircleRect(BallPosition, BallCircle, WallPosition, WallRect))
					return;

				TotalSeparation += CollisionUtils::GetSeparationCircleRect(BallPosition, BallCircle, WallPosition, WallRect);
			});

			if (std::abs(TotalSeparation.X) < FLT_EPSILON && std::abs(TotalSeparation.Y) < FLT_EPSILON)
				return;

			AddCollisionCommand.WithEntry(BallEntity, CollisionComponent{ TotalSeparation });
		});

		if (!AddCollisionCommand.AccessEntries().empty())
		{
			Context.Commands.Submit(std::move(AddCollisionCommand));
		}
		
		if (!AddHealthDeltaCommand.AccessEntries().empty())
		{
			Context.Commands.Submit(std::move(AddHealthDeltaCommand));
		}
	}

	void UpdateBallPaddleCollision(SystemContext& Context, const BallQuery& BallQuery)
	{
		const PaddleQuery PaddleQuery(Context.QueryContext);
		AddComponentsCommand<DirectionCollisionComponent> AddDirectionCollisionCommand(0);
		BallQuery.ForEach([&](Entity BallEntity, const PositionComponent& BallPosition, const CircleComponent& BallCircle, const DamageComponent& BallDamage)
		{
			Vector2D<float> PaddleCenter{ 0.f, 0.f };
			Vector2D<float> Separation{ 0.f, 0.f };
			PaddleQuery.ForEach([&](Entity PaddleEntity, const PositionComponent& PaddlePosition, const RectComponent& PaddleRect, const PlayerMoveSpeedComponent& PaddleMoveSpeed)
			{
				if (!CollisionUtils::CheckCircleRect(BallPosition, BallCircle, PaddlePosition, PaddleRect))
					return;

				Separation = CollisionUtils::GetSeparationCircleRect(BallPosition, BallCircle, PaddlePosition, PaddleRect);
				PaddleCenter = { PaddlePosition.Position.X + PaddleRect.Rect.x + PaddleRect.Rect.w * 0.5f,
								PaddlePosition.Position.Y + PaddleRect.Rect.y + PaddleRect.Rect.h * 0.5f };
			});

			if (std::abs(Separation.X) < FLT_EPSILON && std::abs(Separation.Y) < FLT_EPSILON)
				return;

			AddDirectionCollisionCommand.WithEntry(BallEntity, DirectionCollisionComponent{ Separation, PaddleCenter });
		});

		if (AddDirectionCollisionCommand.AccessEntries().empty())
			return;

		Context.Commands.Submit(std::move(AddDirectionCollisionCommand));
	}

	void UpdateBallTriggersCollision(SystemContext& Context, const BallQuery& BallQuery)
	{
		const TriggerQuery TriggerQuery(Context.QueryContext);
		AddComponentsCommand<HealthDeltaComponent> AddHealthDeltaCommand(0);
		TriggerQuery.ForEach([&](Entity TriggerEntity, const PositionComponent& TriggerPosition, const RectComponent& TriggerRect, const HealthComponent& TriggerHealth)
		{
			int TotalHealthDelta = 0;
			BallQuery.ForEach([&](Entity BallEntity, const PositionComponent& BallPosition, const CircleComponent& BallCircle, const DamageComponent& BallDamage)
			{
				if (!CollisionUtils::CheckCircleRect(BallPosition, BallCircle, TriggerPosition, TriggerRect))
					return;

				TotalHealthDelta -= BallDamage.Damage;
			});

			AddHealthDeltaCommand.WithEntry(TriggerEntity, HealthDeltaComponent{ TotalHealthDelta });
		});

		if (!AddHealthDeltaCommand.AccessEntries().empty())
		{
			Context.Commands.Submit(std::move(AddHealthDeltaCommand));
		}
	}
}

void CollisionDetectionSystem::CleanupCollisionComponents(SystemContext& Context, float DeltaTime)
{
	ComponentUtils::RemoveAllComponentsTyped<CollisionComponent>(Context);
	ComponentUtils::RemoveAllComponentsTyped<DirectionCollisionComponent>(Context);
}

void CollisionDetectionSystem::UpdateBallCollision(SystemContext& Context, float DeltaTime)
{
	const BallQuery BallQuery(Context.QueryContext);

	UpdateBallRegularCollision(Context, BallQuery);
	UpdateBallPaddleCollision(Context, BallQuery);
	UpdateBallTriggersCollision(Context, BallQuery);
}

void CollisionDetectionSystem::UpdatePaddleCollision(SystemContext& Context, float DeltaTime)
{
	const PaddleQuery PaddleQuery(Context.QueryContext);
	const WallsQuery WallsQuery(Context.QueryContext);
	AddComponentsCommand<CollisionComponent> AddCollisionCommand(0);
	PaddleQuery.ForEach([&](Entity PaddleEntity, const PositionComponent& PaddlePosition, const RectComponent& PaddleRect, const PlayerMoveSpeedComponent& PaddleMoveSpeed)
	{
		Vector2D<float> TotalSeparation{ 0.f, 0.f };
		WallsQuery.ForEach([&](Entity WallEntity, const PositionComponent& WallPosition, const RectComponent& WallRect, const GameRenderComponent& WallRender)
		{
			if (!CollisionUtils::CheckAABB(CollisionUtils::GetWorldAABB(PaddlePosition, PaddleRect), CollisionUtils::GetWorldAABB(WallPosition, WallRect)))
				return;

			TotalSeparation += CollisionUtils::GetSeparationRectRect(
				CollisionUtils::GetWorldAABB(PaddlePosition, PaddleRect),
				CollisionUtils::GetWorldAABB(WallPosition, WallRect));
		});

		if (std::abs(TotalSeparation.X) < FLT_EPSILON && std::abs(TotalSeparation.Y) < FLT_EPSILON)
			return;

		AddCollisionCommand.WithEntry(PaddleEntity, CollisionComponent{ TotalSeparation });
	});

	if (!AddCollisionCommand.AccessEntries().empty())
	{
		Context.Commands.Submit(std::move(AddCollisionCommand));
	}
}