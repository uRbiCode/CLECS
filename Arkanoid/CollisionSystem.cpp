#include "CollisionSystem.h"
#include "SystemContext.h"
#include "EntityAdmin.h"
#include "CollisionComponent.h"
#include "TransformComponent.h"
#include "ShapeComponent.h"
#include "VelocityComponent.h"
#include <cmath>
#include <algorithm>

namespace
{
	// Simple AABB collision check
	bool CheckAABB(const SDL_FRect& A, const SDL_FRect& B)
	{
		return !(A.x + A.w < B.x || B.x + B.w < A.x || A.y + A.h < B.y || B.y + B.h < A.y);
	}

	// Get world-space AABB from entity
	SDL_FRect GetWorldAABB(const TransformComponent& Transform, const ShapeComponent& Shape)
	{
		SDL_FRect Result;
		Result.x = Transform.Position.X + Shape.Rect.x;
		Result.y = Transform.Position.Y + Shape.Rect.y;
		Result.w = Shape.Rect.w;
		Result.h = Shape.Rect.h;
		return Result;
	}

	// Get center of sprite (world position)
	Vector2D<float> GetCenter(const TransformComponent& Transform, const ShapeComponent& Shape)
	{
		return {
			Transform.Position.X + Shape.Rect.x + Shape.Rect.w * 0.5f,
			Transform.Position.Y + Shape.Rect.y + Shape.Rect.h * 0.5f
		};
	}

	// Calculate separation vector for AABB collision
	Vector2D<float> GetSeparation(const SDL_FRect& A, const SDL_FRect& B)
	{
		const float OverlapLeft = (A.x + A.w) - B.x;
		const float OverlapRight = (B.x + B.w) - A.x;
		const float OverlapTop = (A.y + A.h) - B.y;
		const float OverlapBottom = (B.y + B.h) - A.y;

		const float MinOverlapX = std::min(OverlapLeft, OverlapRight);
		const float MinOverlapY = std::min(OverlapTop, OverlapBottom);

		if (MinOverlapX < MinOverlapY)
		{
			return { (OverlapLeft < OverlapRight) ? -MinOverlapX : MinOverlapX, 0.f };
		}
		else
		{
			return { 0.f, (OverlapTop < OverlapBottom) ? -MinOverlapY : MinOverlapY };
		}
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

	void ResolveCollisionWithDirection(EntityAdmin& Admin, Entity Ball, Entity Paddle, const Vector2D<float>& Separation)
	{
		if (!Admin.HasComponent<VelocityComponent>(Ball))
			return;

		auto& BallTransform = Admin.AccessComponent<TransformComponent>(Ball);
		BallTransform.Position.X += Separation.X;
		BallTransform.Position.Y += Separation.Y;

		auto& Velocity = Admin.AccessComponent<VelocityComponent>(Ball);

		// Get centers of paddle and ball sprites
		const auto& PaddleTransform = Admin.GetComponent<TransformComponent>(Paddle);
		const auto& PaddleShape = Admin.GetComponent<ShapeComponent>(Paddle);
		const auto& BallShape = Admin.GetComponent<ShapeComponent>(Ball);
		
		const Vector2D<float> PaddleCenter = GetCenter(PaddleTransform, PaddleShape);
		const Vector2D<float> BallCenter = GetCenter(BallTransform, BallShape);

		// Calculate direction from paddle center to ball center
		const Vector2D<float> NewDirection = GetDirection(PaddleCenter, BallCenter);

		// Calculate current speed to maintain
		const float CurrentSpeed = std::sqrt(Velocity.Velocity.X * Velocity.Velocity.X + 
		                                     Velocity.Velocity.Y * Velocity.Velocity.Y);

		// Set new velocity based on direction and current speed
		Velocity.Velocity.X = NewDirection.X * CurrentSpeed;
		Velocity.Velocity.Y = NewDirection.Y * CurrentSpeed;
	}

	void ResolveCollision(EntityAdmin& Admin, Entity EntityToMove, const Vector2D<float>& Separation)
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

void CollisionSystem::Update(const SystemContext& Context, float DeltaTime) const
{
	auto& Admin = Context.EntityAdmin;
	auto CollisionGroup = Admin.GetGroup<TransformComponent, ShapeComponent, CollisionComponent>();
	if (CollisionGroup.Empty())
		return;

	const auto DecideCollisionResponse = [&Admin](bool IsBallPaddleCollision, CollisionResponse Response, const Entity& ThisEntity, CollisionChannel ThisCollisionChannel, const Entity& OtherEntity, const Vector2D<float>& Separation)
	{
		if (Response == CollisionResponse::Block)
		{
			if (IsBallPaddleCollision && ThisCollisionChannel == CollisionChannel::Ball)
			{
				ResolveCollisionWithDirection(Admin, ThisEntity, OtherEntity, Separation);
			}
			else
			{
				ResolveCollision(Admin, ThisEntity, Separation);
			}
		}
	};

	for (size_t i = 0; i < CollisionGroup.Size(); ++i)
	{
		const Entity EntityA = CollisionGroup[i];
		const auto& CollisionA = Admin.GetComponent<CollisionComponent>(EntityA);
		const auto& TransformA = Admin.GetComponent<TransformComponent>(EntityA);
		const auto& ShapeA = Admin.GetComponent<ShapeComponent>(EntityA);
		const SDL_FRect BoundsA = GetWorldAABB(TransformA, ShapeA);

		for (size_t j = i + 1; j < CollisionGroup.Size(); ++j)
		{
			const Entity EntityB = CollisionGroup[j];
			const auto& CollisionB = Admin.GetComponent<CollisionComponent>(EntityB);
			
			const auto ResponseA = CollisionA.ResponseTable[ChannelToIndex(CollisionB.Channel)];
			const auto ResponseB = CollisionB.ResponseTable[ChannelToIndex(CollisionA.Channel)];
			
			if (ResponseA != CollisionResponse::Block && ResponseB != CollisionResponse::Block)
				continue;

			const auto& TransformB = Admin.GetComponent<TransformComponent>(EntityB);
			const auto& ShapeB = Admin.GetComponent<ShapeComponent>(EntityB);
			const SDL_FRect BoundsB = GetWorldAABB(TransformB, ShapeB);

			if (!CheckAABB(BoundsA, BoundsB))
				continue;

			const Vector2D<float> Separation = GetSeparation(BoundsA, BoundsB);
			
			// Check if this is a ball-paddle collision
			const bool IsBallPaddleCollision = 
				(CollisionA.Channel == CollisionChannel::Ball && CollisionB.Channel == CollisionChannel::Player) ||
				(CollisionA.Channel == CollisionChannel::Player && CollisionB.Channel == CollisionChannel::Ball);

			
			DecideCollisionResponse(IsBallPaddleCollision, ResponseA, EntityA, CollisionA.Channel, EntityB, Separation);
			DecideCollisionResponse(IsBallPaddleCollision, ResponseB, EntityB, CollisionB.Channel, EntityA, -Separation);
		}
	}
}