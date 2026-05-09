#include "CollisionKinematicResolverSystem.h"
#include "SystemContext.h"
#include "EventBus.h"
#include "CollisionEvent.h"
#include "TransformComponent.h"
#include "ShapeComponents.h"
#include "VelocityComponent.h"
#include "EntityAdmin.h"
#include "CollisionComponent.h"
#include <cassert>

namespace
{
	Vector2D<float> GetRectCenter(const TransformComponent& Transform, const RectComponent& Rect)
	{
		return {
			Transform.Position.X + Rect.Rect.x + Rect.Rect.w * 0.5f,
			Transform.Position.Y + Rect.Rect.y + Rect.Rect.h * 0.5f
		};
	}

	Vector2D<float> GetCircleCenter(const TransformComponent& Transform)
	{
		return Transform.Position;
	}

	Vector2D<float> GetEntityCenter(EntityAdmin& Admin, const Entity& Entity)
	{
		const auto& Transform = Admin.GetComponent<TransformComponent>(Entity);
		
		if (Admin.HasComponent<CircleComponent>(Entity))
		{
			return GetCircleCenter(Transform);
		}
		else if (Admin.HasComponent<RectComponent>(Entity))
		{
			const auto& Rect = Admin.GetComponent<RectComponent>(Entity);
			return GetRectCenter(Transform, Rect);
		}
		
		return Transform.Position;
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

	void ResolveRegularCollision(EntityAdmin& Admin, const Entity& EntityToMove, const Vector2D<float>& Separation)
	{
		if (!Admin.HasComponent<VelocityComponent>(EntityToMove))
			return;

		auto& Transform = Admin.AccessComponent<TransformComponent>(EntityToMove);
		Transform.Position.X += Separation.X;
		Transform.Position.Y += Separation.Y;

		auto& Velocity = Admin.AccessComponent<VelocityComponent>(EntityToMove);

		if (std::abs(Separation.X) > FLT_EPSILON)
		{
			Velocity.Velocity.X = -Velocity.Velocity.X;
		}
		if (std::abs(Separation.Y) > FLT_EPSILON)
		{
			Velocity.Velocity.Y = -Velocity.Velocity.Y;
		}
	}

	void ResolveCollisionWithDirection(EntityAdmin& Admin, const Entity& Ball, const Entity& Player, const Vector2D<float>& Separation)
	{
		if (!Admin.HasComponent<VelocityComponent>(Ball))
			return;

		auto& BallTransform = Admin.AccessComponent<TransformComponent>(Ball);
		BallTransform.Position.X += Separation.X;
		BallTransform.Position.Y += Separation.Y;

		auto& Velocity = Admin.AccessComponent<VelocityComponent>(Ball);

		const Vector2D<float> PlayerCenter   = GetEntityCenter(Admin, Player);
		const Vector2D<float> BallCenter     = GetEntityCenter(Admin, Ball);
		const Vector2D<float> NewDirection   = GetDirection(PlayerCenter, BallCenter);

		const float CurrentSpeed = std::sqrt(Velocity.Velocity.X * Velocity.Velocity.X +
		                                     Velocity.Velocity.Y * Velocity.Velocity.Y);

		Velocity.Velocity.X = NewDirection.X * CurrentSpeed;
		Velocity.Velocity.Y = NewDirection.Y * CurrentSpeed;
	}

	void OnCollision(const SystemContext& Context, const CollisionEvent& Event)
	{
		auto& Admin = Context.EntityAdmin;
		if (!Admin.HasComponent<CollisionComponent>(Event.EntityA) || !Admin.HasComponent<CollisionComponent>(Event.EntityB))
			return;

		const auto& CollisionA = Admin.GetComponent<CollisionComponent>(Event.EntityA);
		const auto& CollisionB = Admin.GetComponent<CollisionComponent>(Event.EntityB);

		const auto ResponseA = CollisionA.ResponseTable[ChannelToIndex(CollisionB.Channel)];
		const auto ResponseB = CollisionB.ResponseTable[ChannelToIndex(CollisionA.Channel)];

		const bool IsBallPlayerCollision =
			(CollisionA.Channel == CollisionChannel::Ball && CollisionB.Channel == CollisionChannel::Player)
			|| (CollisionA.Channel == CollisionChannel::Player && CollisionB.Channel == CollisionChannel::Ball);

		const auto DecideCollisionResponse = [&Admin](bool IsBallPlayerCollision, CollisionResponse Response, const Entity& ThisEntity, CollisionChannel ThisCollisionChannel, const Entity& OtherEntity, const Vector2D<float>& Separation)
		{
			if (Response != CollisionResponse::Block)
				return;

			if (IsBallPlayerCollision && ThisCollisionChannel == CollisionChannel::Ball)
			{
				ResolveCollisionWithDirection(Admin, ThisEntity, OtherEntity, Separation);
				return;
			}

			ResolveRegularCollision(Admin, ThisEntity, Separation);
		};

		DecideCollisionResponse(IsBallPlayerCollision, ResponseA, Event.EntityA, CollisionA.Channel, Event.EntityB,  Event.Separation);
		DecideCollisionResponse(IsBallPlayerCollision, ResponseB, Event.EntityB, CollisionB.Channel, Event.EntityA, -Event.Separation);
	}
}

void CollisionKinematicResolverSystem::Initialize(const SystemContext& Context)
{
	const void* const Id = reinterpret_cast<const void*>(&Initialize);

	Context.EventBus.Subscribe<CollisionEvent>(Id, [](const SystemContext& Context, const CollisionEvent& Event)
	{
		OnCollision(Context, Event);
	});
}