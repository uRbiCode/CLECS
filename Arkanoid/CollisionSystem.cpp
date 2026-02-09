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
}

void CollisionSystem::Update(const SystemContext& Context, float DeltaTime) const
{
	auto& Admin = Context.EntityAdmin;
	auto CollisionGroup = Admin.GetGroup<TransformComponent, ShapeComponent, CollisionComponent>();

	for (size_t i = 0; i < CollisionGroup.Size(); ++i)
	{
		const Entity EntityA = CollisionGroup[i];
		const auto& CollisionA = Admin.GetComponent<CollisionComponent>(EntityA);
		auto& TransformA = Admin.AccessComponent<TransformComponent>(EntityA);
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
			if (ResponseA == CollisionResponse::Block)
			{
				TransformA.Position.X += Separation.X;
				TransformA.Position.Y += Separation.Y;

				if (Admin.HasComponent<VelocityComponent>(EntityA))
				{
					auto& VelocityA = Admin.AccessComponent<VelocityComponent>(EntityA);
					
					if (Separation.X != 0.f)
					{
						VelocityA.Velocity.X = -VelocityA.Velocity.X;
					}
					if (Separation.Y != 0.f)
					{
						VelocityA.Velocity.Y = -VelocityA.Velocity.Y;
					}
				}
			}
		}
	}
}