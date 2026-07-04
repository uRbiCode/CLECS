#include "MovementSystem.h"
#include "SystemContext.h"
#include "Query.h"
#include "PositionComponent.h"
#include "VelocityComponent.h"
#include "PlayerMoveSpeedComponent.h"
#include "CommandRunner.h"
#include "CollisionComponents.h"

void MovementSystem::ResolveMovementChanges(SystemContext& Context, float DeltaTime)
{
	const Query<WritesList<VelocityComponent>, ReadsList<PlayerMoveSpeedComponent>, ExcludeList<>> VelocityQuery(Context.QueryContext);
	VelocityQuery.ForEach([](Entity Entity, VelocityComponent& Velocity, const PlayerMoveSpeedComponent& MoveSpeed)
	{
		Velocity.Velocity.X = MoveSpeed.MoveSpeedInputMultiplier * MoveSpeed.MoveSpeed;
	});
}

void MovementSystem::ResolveMovement(SystemContext& Context, float DeltaTime)
{
	const Query<WritesList<PositionComponent>, ReadsList<CollisionComponent>, ExcludeList<>> CollisionQuery(Context.QueryContext);
	CollisionQuery.ForEach([DeltaTime](Entity Entity, PositionComponent& Position, const CollisionComponent& Collision)
	{
		Position.Position += Collision.Separation;
	});

	const Query<WritesList<VelocityComponent>, ReadsList<CollisionComponent>, ExcludeList<>> VelocityCollisionQuery(Context.QueryContext);
	VelocityCollisionQuery.ForEach([](Entity Entity, VelocityComponent& Velocity, const CollisionComponent& Collision)
	{
		if (std::abs(Collision.Separation.X) > FLT_EPSILON)
		{
			Velocity.Velocity.X = -Velocity.Velocity.X;
		}
		if (std::abs(Collision.Separation.Y) > FLT_EPSILON)
		{
			Velocity.Velocity.Y = -Velocity.Velocity.Y;
		}
	});

	const Query<WritesList<VelocityComponent>, ReadsList<PositionComponent, DirectionCollisionComponent>, ExcludeList<>> DirectionQuery(Context.QueryContext);
	DirectionQuery.ForEach([DeltaTime](Entity Entity, VelocityComponent& Velocity, const PositionComponent& Position, const DirectionCollisionComponent& DirectionCollision)
	{
		const Vector2D<float> NewDirection = (Position.Position - DirectionCollision.Source).Normalized();
		const float CurrentSpeed = Velocity.Velocity.Length();
		Velocity.Velocity = NewDirection * CurrentSpeed;
	});

	const Query<WritesList<PositionComponent>, ReadsList<VelocityComponent>, ExcludeList<>> MoveQuery(Context.QueryContext);
	MoveQuery.ForEach([DeltaTime](Entity Entity, PositionComponent& Position, const VelocityComponent& Velocity)
	{
		Position.Position.X += Velocity.Velocity.X * DeltaTime;
		Position.Position.Y += Velocity.Velocity.Y * DeltaTime;
	});
}