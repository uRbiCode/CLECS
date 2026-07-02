#include "CollisionUtils.h"
#include "ShapeComponents.h"
#include "PositionComponent.h"
#include <algorithm>

SDL_FRect CollisionUtils::GetWorldAABB(const PositionComponent& Position, const RectComponent& Rect)
{
	SDL_FRect Result = {};
	Result.x = Position.Position.X + Rect.Rect.x;
	Result.y = Position.Position.Y + Rect.Rect.y;
	Result.w = Rect.Rect.w;
	Result.h = Rect.Rect.h;
	return Result;
}

Vector2D<float> CollisionUtils::GetSeparationRectRect(const SDL_FRect& A, const SDL_FRect& B)
{
	const float OverlapLeft = (A.x + A.w) - B.x;
	const float OverlapRight = (B.x + B.w) - A.x;
	const float OverlapTop = (A.y + A.h) - B.y;
	const float OverlapBottom = (B.y + B.h) - A.y;

	const float MinOverlapX = std::min(OverlapLeft, OverlapRight);
	const float MinOverlapY = std::min(OverlapTop, OverlapBottom);

	if (MinOverlapX < MinOverlapY)
		return { (OverlapLeft < OverlapRight) ? -MinOverlapX : MinOverlapX, 0.f };

	return { 0.f, (OverlapTop < OverlapBottom) ? -MinOverlapY : MinOverlapY };
}

Vector2D<float> CollisionUtils::GetSeparationCircleRect(const PositionComponent& CirclePosition, const CircleComponent& Circle, const PositionComponent& RectPosition, const RectComponent& Rect)
{
	const float CircleCenterX = CirclePosition.Position.X;
	const float CircleCenterY = CirclePosition.Position.Y;
	
	const float RectLeft = RectPosition.Position.X + Rect.Rect.x;
	const float RectRight = RectLeft + Rect.Rect.w;
	const float RectTop = RectPosition.Position.Y + Rect.Rect.y;
	const float RectBottom = RectTop + Rect.Rect.h;
	
	const float ClosestX = std::max(RectLeft, std::min(CircleCenterX, RectRight));
	const float ClosestY = std::max(RectTop, std::min(CircleCenterY, RectBottom));
	
	const float DX = CircleCenterX - ClosestX;
	const float DY = CircleCenterY - ClosestY;
	const float Distance = std::sqrt(DX * DX + DY * DY);
	
	if (Distance > 0.f)
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
		return { -DistLeft - Circle.Radius, 0.f };
	if (MinDist == DistRight)
		return { DistRight + Circle.Radius, 0.f };
	if (MinDist == DistTop)
		return { 0.f, -DistTop - Circle.Radius };

	return { 0.f, DistBottom + Circle.Radius };
}

bool CollisionUtils::CheckAABB(const SDL_FRect& A, const SDL_FRect& B)
{
	return !(A.x + A.w < B.x || B.x + B.w < A.x || A.y + A.h < B.y || B.y + B.h < A.y);
}

bool CollisionUtils::CheckCircleRect(const PositionComponent& CirclePosition, const CircleComponent& Circle, const PositionComponent& RectPosition, const RectComponent& Rect)
{
	const float CircleCenterX = CirclePosition.Position.X;
	const float CircleCenterY = CirclePosition.Position.Y;
	const float RectLeft = RectPosition.Position.X + Rect.Rect.x;
	const float RectRight = RectLeft + Rect.Rect.w;
	const float RectTop = RectPosition.Position.Y + Rect.Rect.y;
	const float RectBottom = RectTop + Rect.Rect.h;

	const float ClosestX = std::max(RectLeft, std::min(CircleCenterX, RectRight));
	const float ClosestY = std::max(RectTop, std::min(CircleCenterY, RectBottom));

	const float DistX = CircleCenterX - ClosestX;
	const float DistY = CircleCenterY - ClosestY;
	const float DistanceSquared = DistX * DistX + DistY * DistY;

	return DistanceSquared < (Circle.Radius * Circle.Radius);
}