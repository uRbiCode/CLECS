#include "CollisionResolverSystem.h"
#include "SystemContext.h"
#include "EventBus.h"
#include "CollisionEvent.h"
#include <TransformComponent.h>
#include <ShapeComponent.h>
#include "VelocityComponent.h"
#include "EntityAdmin.h"
#include "CollisionComponent.h"
#include <cassert>

namespace
{
	Vector2D<float> GetCenter(const TransformComponent& Transform, const ShapeComponent& Shape)
	{
		return {
			Transform.Position.X + Shape.Rect.x + Shape.Rect.w * 0.5f,
			Transform.Position.Y + Shape.Rect.y + Shape.Rect.h * 0.5f
		};
	}

	Vector2D<float> GetDirection(const Vector2D<float>& From, const Vector2D<float>& To)
	{
		Vector2D<float> Direction = To - From;
		const float Length = std::sqrt(Direction.X * Direction.X + Direction.Y * Direction.Y);
		if (Length > 0.f)
		{
			Direction.X /= Length;
			Direction.Y /= Length;
		}
		return Direction;
	}

	void ResolveCollisionWithDirection(EntityAdmin& Admin, const Entity& Ball, const Entity& Player, const Vector2D<float>& Separation)
	{
		if (!Admin.HasComponent<VelocityComponent>(Ball))
			return;

		auto& BallTransform = Admin.AccessComponent<TransformComponent>(Ball);
		BallTransform.Position.X += Separation.X;
		BallTransform.Position.Y += Separation.Y;

		auto& Velocity = Admin.AccessComponent<VelocityComponent>(Ball);

		const auto& PlayerTransform = Admin.GetComponent<TransformComponent>(Player);
		const auto& PlayerShape = Admin.GetComponent<ShapeComponent>(Player);
		const auto& BallShape = Admin.GetComponent<ShapeComponent>(Ball);
		
		const Vector2D<float> PlayerCenter = GetCenter(PlayerTransform, PlayerShape);
		const Vector2D<float> BallCenter = GetCenter(BallTransform, BallShape);

		const Vector2D<float> NewDirection = GetDirection(PlayerCenter, BallCenter);

		const float CurrentSpeed = std::sqrt(Velocity.Velocity.X * Velocity.Velocity.X + 
		                                     Velocity.Velocity.Y * Velocity.Velocity.Y);

		Velocity.Velocity.X = NewDirection.X * CurrentSpeed;
		Velocity.Velocity.Y = NewDirection.Y * CurrentSpeed;
	}

	void ResolveRegularCollision(EntityAdmin& Admin, const Entity& EntityToMove, const Vector2D<float>& Separation)
	{
		if (Admin.HasComponent<VelocityComponent>(EntityToMove))
		{
			auto& Transform = Admin.AccessComponent<TransformComponent>(EntityToMove);
			Transform.Position.X += Separation.X;
			Transform.Position.Y += Separation.Y;

			auto& Velocity = Admin.AccessComponent<VelocityComponent>(EntityToMove);
			
			if (Separation.X != 0.f)
			{
				Velocity.Velocity.X = -Velocity.Velocity.X;
			}
			if (Separation.Y != 0.f)
			{
				Velocity.Velocity.Y = -Velocity.Velocity.Y;
			}
		}
	}
}

void CollisionResolverSystem::Initialize(const SystemContext& Context) const
{
	auto& EventBus = Context.EventBus;
	EventBus.Subscribe<CollisionEvent>(this, [this](const SystemContext& Context, const CollisionEvent& Event)
	{
		OnCollision(Context, Event);
		return;
	});
}

void CollisionResolverSystem::OnCollision(const SystemContext& Context, const CollisionEvent& Event) const
{
	auto& Admin = Context.EntityAdmin;
	assert(Admin.HasComponent<CollisionComponent>(Event.EntityA) && "EntityA must have a CollisionComponent");
	assert(Admin.HasComponent<CollisionComponent>(Event.EntityB) && "EntityB must have a CollisionComponent");

	const auto& CollisionA = Admin.GetComponent<CollisionComponent>(Event.EntityA);
	const auto& CollisionB = Admin.GetComponent<CollisionComponent>(Event.EntityB);

	const auto ResponseA = CollisionA.ResponseTable[ChannelToIndex(CollisionB.Channel)];
	const auto ResponseB = CollisionB.ResponseTable[ChannelToIndex(CollisionA.Channel)];

	const bool IsBallPlayerCollision = 
		(CollisionA.Channel == CollisionChannel::Ball && CollisionB.Channel == CollisionChannel::Player)
		|| (CollisionA.Channel == CollisionChannel::Player && CollisionB.Channel == CollisionChannel::Ball);

	const auto DecideCollisionResponse = [&Admin](bool IsBallPlayerCollision, CollisionResponse Response, const Entity& ThisEntity, CollisionChannel ThisCollisionChannel, const Entity& OtherEntity, const Vector2D<float>& Separation)
	{
		if (Response == CollisionResponse::Block)
		{
			if (IsBallPlayerCollision && ThisCollisionChannel == CollisionChannel::Ball)
			{
				ResolveCollisionWithDirection(Admin, ThisEntity, OtherEntity, Separation);
			}
			else
			{
				ResolveRegularCollision(Admin, ThisEntity, Separation);
			}
		}
	};
	
	DecideCollisionResponse(IsBallPlayerCollision, ResponseA, Event.EntityA, CollisionA.Channel, Event.EntityB, Event.Separation);
	DecideCollisionResponse(IsBallPlayerCollision, ResponseB, Event.EntityB, CollisionB.Channel, Event.EntityA, -Event.Separation);
}