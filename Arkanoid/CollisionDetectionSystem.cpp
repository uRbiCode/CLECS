#include "CollisionDetectionSystem.h"
#include "SystemContext.h"
#include "EntityAdmin.h"
#include "CollisionComponent.h"
#include "TransformComponent.h"
#include "ShapeComponents.h"
#include "VelocityComponent.h"
#include <cmath>
#include <algorithm>
#include "CollisionEvent.h"
#include "EventBus.h"

namespace
{
	bool CheckAABB(const SDL_FRect& A, const SDL_FRect& B)
	{
		return !(A.x + A.w < B.x || B.x + B.w < A.x || A.y + A.h < B.y || B.y + B.h < A.y);
	}

	SDL_FRect GetWorldAABB(const TransformComponent& Transform, const RectComponent& Rect)
	{
		SDL_FRect Result;
		Result.x = Transform.Position.X + Rect.Rect.x;
		Result.y = Transform.Position.Y + Rect.Rect.y;
		Result.w = Rect.Rect.w;
		Result.h = Rect.Rect.h;
		return Result;
	}

	SDL_FRect GetCircleAABB(const TransformComponent& Transform, const CircleComponent& Circle)
	{
		SDL_FRect Result;
		Result.x = Transform.Position.X - Circle.Radius;
		Result.y = Transform.Position.Y - Circle.Radius;
		Result.w = Circle.Radius * 2.0f;
		Result.h = Circle.Radius * 2.0f;
		return Result;
	}

	bool CheckCircleRect(const TransformComponent& CircleTransform, const CircleComponent& Circle,
	                     const TransformComponent& RectTransform, const RectComponent& Rect)
	{
		const float CircleCenterX = CircleTransform.Position.X;
		const float CircleCenterY = CircleTransform.Position.Y;
		
		const float RectLeft = RectTransform.Position.X + Rect.Rect.x;
		const float RectRight = RectLeft + Rect.Rect.w;
		const float RectTop = RectTransform.Position.Y + Rect.Rect.y;
		const float RectBottom = RectTop + Rect.Rect.h;
		
		const float ClosestX = std::max(RectLeft, std::min(CircleCenterX, RectRight));
		const float ClosestY = std::max(RectTop, std::min(CircleCenterY, RectBottom));
		
		const float DistX = CircleCenterX - ClosestX;
		const float DistY = CircleCenterY - ClosestY;
		const float DistanceSquared = DistX * DistX + DistY * DistY;
		
		return DistanceSquared < (Circle.Radius * Circle.Radius);
	}

	bool CheckCircleCircle(const TransformComponent& TransformA, const CircleComponent& CircleA,
	                       const TransformComponent& TransformB, const CircleComponent& CircleB)
	{
		const float DX = TransformB.Position.X - TransformA.Position.X;
		const float DY = TransformB.Position.Y - TransformA.Position.Y;
		const float DistanceSquared = DX * DX + DY * DY;
		const float RadiusSum = CircleA.Radius + CircleB.Radius;
		
		return DistanceSquared < (RadiusSum * RadiusSum);
	}

	Vector2D<float> GetSeparationRectRect(const SDL_FRect& A, const SDL_FRect& B)
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

		return { 0.f, (OverlapTop < OverlapBottom) ? -MinOverlapY : MinOverlapY };
	}

	Vector2D<float> GetSeparationCircleRect(const TransformComponent& CircleTransform, const CircleComponent& Circle,
	                                        const TransformComponent& RectTransform, const RectComponent& Rect)
	{
		const float CircleCenterX = CircleTransform.Position.X;
		const float CircleCenterY = CircleTransform.Position.Y;
		
		const float RectLeft = RectTransform.Position.X + Rect.Rect.x;
		const float RectRight = RectLeft + Rect.Rect.w;
		const float RectTop = RectTransform.Position.Y + Rect.Rect.y;
		const float RectBottom = RectTop + Rect.Rect.h;
		
		const float ClosestX = std::max(RectLeft, std::min(CircleCenterX, RectRight));
		const float ClosestY = std::max(RectTop, std::min(CircleCenterY, RectBottom));
		
		const float DX = CircleCenterX - ClosestX;
		const float DY = CircleCenterY - ClosestY;
		const float Distance = std::sqrt(DX * DX + DY * DY);
		
		if (Distance > 0.0f)
		{
			const float Penetration = Circle.Radius - Distance;
			return { (DX / Distance) * Penetration, (DY / Distance) * Penetration };
		}
		
		const float DistLeft = CircleCenterX - RectLeft;
		const float DistRight = RectRight - CircleCenterX;
		const float DistTop = CircleCenterY - RectTop;
		const float DistBottom = RectBottom - CircleCenterY;
		
		const float MinDist = std::min({DistLeft, DistRight, DistTop, DistBottom});
		
		if (MinDist == DistLeft)
			return { -DistLeft - Circle.Radius, 0.0f };
		if (MinDist == DistRight)
			return { DistRight + Circle.Radius, 0.0f };
		if (MinDist == DistTop)
			return { 0.0f, -DistTop - Circle.Radius };

		return { 0.0f, DistBottom + Circle.Radius };
	}

	Vector2D<float> GetSeparationCircleCircle(const TransformComponent& TransformA, const CircleComponent& CircleA,
	                                          const TransformComponent& TransformB, const CircleComponent& CircleB)
	{
		const float DX = TransformB.Position.X - TransformA.Position.X;
		const float DY = TransformB.Position.Y - TransformA.Position.Y;
		const float Distance = std::sqrt(DX * DX + DY * DY);
		
		if (Distance > 0.0f)
		{
			const float RadiusSum = CircleA.Radius + CircleB.Radius;
			const float Penetration = RadiusSum - Distance;
			return { -(DX / Distance) * Penetration, -(DY / Distance) * Penetration };
		}
		
		return { -(CircleA.Radius + CircleB.Radius), 0.0f };
	}
}

void CollisionDetectionSystem::Update(const SystemContext& Context, float DeltaTime) const
{
	auto& Admin = Context.EntityAdmin;
	auto CollisionGroup = Admin.GetGroup<TransformComponent, CollisionComponent>();
	if (CollisionGroup.Empty())
		return;

	for (size_t i = 0; i < CollisionGroup.Size(); ++i)
	{
		const const Entity& EntityA = CollisionGroup[i];
		if (!Admin.HasComponent<CollisionComponent>(EntityA))
			continue;

		const auto& CollisionA = Admin.GetComponent<CollisionComponent>(EntityA);
		const auto& TransformA = Admin.GetComponent<TransformComponent>(EntityA);
		
		const bool AHasRect = Admin.HasComponent<RectComponent>(EntityA);
		const bool AHasCircle = Admin.HasComponent<CircleComponent>(EntityA);
		
		if (!AHasRect && !AHasCircle)
			continue;

		for (size_t j = i + 1; j < CollisionGroup.Size(); ++j)
		{
			const const Entity& EntityB = CollisionGroup[j];
			if (!Admin.HasComponent<CollisionComponent>(EntityB))
				continue;

			const auto& CollisionB = Admin.GetComponent<CollisionComponent>(EntityB);
			
			const auto ResponseA = CollisionA.ResponseTable[ChannelToIndex(CollisionB.Channel)];
			const auto ResponseB = CollisionB.ResponseTable[ChannelToIndex(CollisionA.Channel)];
			
			if (ResponseA != CollisionResponse::Block && ResponseB != CollisionResponse::Block)
				continue;

			const auto& TransformB = Admin.GetComponent<TransformComponent>(EntityB);
			const bool BHasRect = Admin.HasComponent<RectComponent>(EntityB);
			const bool BHasCircle = Admin.HasComponent<CircleComponent>(EntityB);
			
			if (!BHasRect && !BHasCircle)
				continue;

			bool Colliding = false;
			Vector2D<float> Separation = { 0.0f, 0.0f };

			if (AHasRect && BHasRect)
			{
				const auto& RectA = Admin.GetComponent<RectComponent>(EntityA);
				const auto& RectB = Admin.GetComponent<RectComponent>(EntityB);
				const SDL_FRect BoundsA = GetWorldAABB(TransformA, RectA);
				const SDL_FRect BoundsB = GetWorldAABB(TransformB, RectB);
				
				if (CheckAABB(BoundsA, BoundsB))
				{
					Colliding = true;
					Separation = GetSeparationRectRect(BoundsA, BoundsB);
				}
			}
			else if (AHasCircle && BHasCircle)
			{
				const auto& CircleA = Admin.GetComponent<CircleComponent>(EntityA);
				const auto& CircleB = Admin.GetComponent<CircleComponent>(EntityB);
				
				if (CheckCircleCircle(TransformA, CircleA, TransformB, CircleB))
				{
					Colliding = true;
					Separation = GetSeparationCircleCircle(TransformA, CircleA, TransformB, CircleB);
				}
			}
			else if (AHasCircle && BHasRect)
			{
				const auto& CircleA = Admin.GetComponent<CircleComponent>(EntityA);
				const auto& RectB = Admin.GetComponent<RectComponent>(EntityB);
				
				if (CheckCircleRect(TransformA, CircleA, TransformB, RectB))
				{
					Colliding = true;
					Separation = GetSeparationCircleRect(TransformA, CircleA, TransformB, RectB);
				}
			}
			else if (AHasRect && BHasCircle)
			{
				const auto& RectA = Admin.GetComponent<RectComponent>(EntityA);
				const auto& CircleB = Admin.GetComponent<CircleComponent>(EntityB);
				
				if (CheckCircleRect(TransformB, CircleB, TransformA, RectA))
				{
					Colliding = true;
					Separation = GetSeparationCircleRect(TransformB, CircleB, TransformA, RectA);
					Separation.X = -Separation.X;
					Separation.Y = -Separation.Y;
				}
			}

			if (Colliding)
			{
				Context.EventBus.Notify(Context, CollisionEvent{ EntityA, EntityB, Separation });
			}
		}
	}
}