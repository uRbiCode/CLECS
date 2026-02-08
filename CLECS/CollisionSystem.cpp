#include "CollisionSystem.h"
#include "SystemUpdateContext.h"
#include "EntityManager.h"
#include "CollisionComponent.h"
#include "TransformComponent.h"
#include "ShapeComponent.h"
#include "VelocityComponent.h"
#include <cmath>
#include <algorithm>

void CollisionSystem::Update(const SystemUpdateContext& UpdateContext, float DeltaTime)
{
	auto& Manager = UpdateContext.EntityManager;

	// Get all entities with collision components
	auto CollisionGroup = Manager.GetGroup<TransformComponent, ShapeComponent, CollisionComponent>();

	// Phase 1: Resolve existing overlaps (push objects apart if already penetrating)
	for (size_t i = 0; i < CollisionGroup.Size(); ++i)
	{
		const Entity EntityA = CollisionGroup[i];
		const auto& CollisionA = Manager.GetComponent<CollisionComponent>(EntityA);
		auto& TransformA = Manager.AccessComponent<TransformComponent>(EntityA);
		const auto& ShapeA = Manager.GetComponent<ShapeComponent>(EntityA);

		for (size_t j = 0; j < CollisionGroup.Size(); ++j)
		{
			if (i == j)
				continue;

			const Entity EntityB = CollisionGroup[j];
			const auto& CollisionB = Manager.GetComponent<CollisionComponent>(EntityB);

			// Check if A should respond to B
			const auto ResponseA = CollisionA.ResponseTable[ChannelToIndex(CollisionB.Channel)];

			// Skip if A ignores B
			if (ResponseA == CollisionResponse::Ignore)
				continue;

			const auto& TransformB = Manager.GetComponent<TransformComponent>(EntityB);
			const auto& ShapeB = Manager.GetComponent<ShapeComponent>(EntityB);

			// Check for collision and resolve if blocking
			if (ResponseA == CollisionResponse::Block && CheckCollision(TransformA, ShapeA, TransformB, ShapeB))
			{
				// Push A out of B to the nearest valid position
				const Vector2D<float> Separation = CalculateSeparation(TransformA, ShapeA, TransformB, ShapeB);
				TransformA.Position.X += Separation.X;
				TransformA.Position.Y += Separation.Y;

				// If A has velocity, also zero velocity components pointing into the obstacle
				if (Manager.HasComponent<VelocityComponent>(EntityA))
				{
					const bool BIsStatic = !Manager.HasComponent<VelocityComponent>(EntityB);
					if (BIsStatic)
					{
						auto& VelocityA = Manager.AccessComponent<VelocityComponent>(EntityA);
						
						// Zero out velocity components that would push back into the obstacle
						const float SeparationMagnitude = std::sqrtf(Separation.X * Separation.X + Separation.Y * Separation.Y);
						if (SeparationMagnitude > 0.001f)
						{
							const Vector2D<float> SeparationDir = { Separation.X / SeparationMagnitude, Separation.Y / SeparationMagnitude };
							const float VelocityDot = VelocityA.Velocity.X * SeparationDir.X + VelocityA.Velocity.Y * SeparationDir.Y;
							
							// If velocity is pointing back into the obstacle, remove that component
							if (VelocityDot < 0.f)
							{
								VelocityA.Velocity.X -= VelocityDot * SeparationDir.X;
								VelocityA.Velocity.Y -= VelocityDot * SeparationDir.Y;
							}
						}
					}
				}
			}
		}
	}

	// Phase 2: Prevent velocity from causing NEW collisions or INCREASING penetration
	for (size_t i = 0; i < CollisionGroup.Size(); ++i)
	{
		const Entity EntityA = CollisionGroup[i];
		const auto& CollisionA = Manager.GetComponent<CollisionComponent>(EntityA);
		const auto& TransformA = Manager.GetComponent<TransformComponent>(EntityA);
		const auto& ShapeA = Manager.GetComponent<ShapeComponent>(EntityA);

		// Only check entities that have velocity
		if (!Manager.HasComponent<VelocityComponent>(EntityA))
			continue;

		auto& VelocityA = Manager.AccessComponent<VelocityComponent>(EntityA);

		for (size_t j = 0; j < CollisionGroup.Size(); ++j)
		{
			if (i == j)
				continue;

			const Entity EntityB = CollisionGroup[j];
			const auto& CollisionB = Manager.GetComponent<CollisionComponent>(EntityB);

			// Check if A should block against B
			const auto ResponseA = CollisionA.ResponseTable[ChannelToIndex(CollisionB.Channel)];
			if (ResponseA != CollisionResponse::Block)
				continue;

			// Check if B is "static" (no velocity component = immovable)
			const bool BIsStatic = !Manager.HasComponent<VelocityComponent>(EntityB);
			if (!BIsStatic)
				continue; // Skip dynamic-vs-dynamic for this phase

			const auto& TransformB = Manager.GetComponent<TransformComponent>(EntityB);
			const auto& ShapeB = Manager.GetComponent<ShapeComponent>(EntityB);

			// Check if currently colliding
			const bool CurrentlyColliding = CheckCollision(TransformA, ShapeA, TransformB, ShapeB);

			// Check angular velocity (rotation) first
			if (VelocityA.AngularVelocity != 0.f)
			{
				const bool WouldCollideAfterRotation = this->WouldCollideAfterRotation(TransformA, ShapeA, VelocityA.AngularVelocity, DeltaTime, TransformB, ShapeB);
				
				// Only block rotation if it would cause a NEW collision (not already colliding)
				if (!CurrentlyColliding && WouldCollideAfterRotation)
				{
					VelocityA.AngularVelocity = 0.f;
				}
			}

			// Check linear velocity (movement)
			if (VelocityA.Velocity.X != 0.f || VelocityA.Velocity.Y != 0.f)
			{
				const bool WouldCollideAfterMovement = this->WouldCollideAfterMovement(TransformA, ShapeA, VelocityA.Velocity, DeltaTime, TransformB, ShapeB);
				
				// Only block movement if it would cause a NEW collision (not already colliding)
				if (!CurrentlyColliding && WouldCollideAfterMovement)
				{
					// Zero velocity to prevent penetration
					VelocityA.Velocity = { 0.f, 0.f };
				}
			}
		}
	}
}

bool CollisionSystem::WouldCollideAfterRotation(
	const TransformComponent& Transform,
	const ShapeComponent& Shape,
	float AngularVelocity,
	float DeltaTime,
	const TransformComponent& ObstacleTransform,
	const ShapeComponent& ObstacleShape) const
{
	// Create predicted transform after rotation
	TransformComponent PredictedTransform = Transform;
	PredictedTransform.Rotation += AngularVelocity * DeltaTime;

	return CheckCollision(PredictedTransform, Shape, ObstacleTransform, ObstacleShape);
}

bool CollisionSystem::WouldCollideAfterMovement(
	const TransformComponent& Transform,
	const ShapeComponent& Shape,
	const Vector2D<float>& Velocity,
	float DeltaTime,
	const TransformComponent& ObstacleTransform,
	const ShapeComponent& ObstacleShape) const
{
	// Create predicted transform after movement
	TransformComponent PredictedTransform = Transform;
	PredictedTransform.Position.X += Velocity.X * DeltaTime;
	PredictedTransform.Position.Y += Velocity.Y * DeltaTime;

	return CheckCollision(PredictedTransform, Shape, ObstacleTransform, ObstacleShape);
}

Vector2D<float> CollisionSystem::CalculateSeparation(
	const TransformComponent& TransformA,
	const ShapeComponent& ShapeA,
	const TransformComponent& TransformB,
	const ShapeComponent& ShapeB) const
{
	const bool AIsCircle = (ShapeA.Type == ShapeComponent::ShapeType::Circle);
	const bool BIsCircle = (ShapeB.Type == ShapeComponent::ShapeType::Circle);

	if (AIsCircle && BIsCircle)
	{
		return CalculateCircleCircleSeparation(TransformA, ShapeA, TransformB, ShapeB);
	}
	else if (!AIsCircle && !BIsCircle)
	{
		// Check if either has rotation - if so, use OBB separation
		const bool AUsesRotation = (TransformA.Rotation != 0.f);
		const bool BUsesRotation = (TransformB.Rotation != 0.f);

		if (AUsesRotation || BUsesRotation)
		{
			return CalculateOBBSeparation(TransformA, ShapeA, TransformB, ShapeB);
		}
		else
		{
			return CalculateAABBSeparation(TransformA, ShapeA, TransformB, ShapeB);
		}
	}
	else if (AIsCircle)
	{
		const bool BUsesRotation = (TransformB.Rotation != 0.f);
		if (BUsesRotation)
		{
			return CalculateCircleOBBSeparation(TransformA.Position, ShapeA.Rect.w * 0.5f, TransformB, ShapeB);
		}
		else
		{
			return CalculateCircleRectSeparation(TransformA.Position, ShapeA.Rect.w * 0.5f, TransformB, ShapeB);
		}
	}
	else
	{
		// Rectangle vs Circle - flip the separation
		const bool AUsesRotation = (TransformA.Rotation != 0.f);
		if (AUsesRotation)
		{
			const Vector2D<float> Sep = CalculateCircleOBBSeparation(TransformB.Position, ShapeB.Rect.w * 0.5f, TransformA, ShapeA);
			return { -Sep.X, -Sep.Y };
		}
		else
		{
			const Vector2D<float> Sep = CalculateCircleRectSeparation(TransformB.Position, ShapeB.Rect.w * 0.5f, TransformA, ShapeA);
			return { -Sep.X, -Sep.Y };
		}
	}
}

Vector2D<float> CollisionSystem::CalculateOBBSeparation(
	const TransformComponent& TransformA,
	const ShapeComponent& ShapeA,
	const TransformComponent& TransformB,
	const ShapeComponent& ShapeB) const
{
	const auto CornersA = GetOBBCorners(TransformA, ShapeA);
	const auto CornersB = GetOBBCorners(TransformB, ShapeB);

	const auto AxesA = GetOBBAxes(CornersA);
	const auto AxesB = GetOBBAxes(CornersB);

	// Find the axis with minimum overlap (MTV - Minimum Translation Vector)
	float MinOverlap = std::numeric_limits<float>::max();
	Vector2D<float> MinAxis = { 0.f, 0.f };

	// Test all 4 axes
	std::array<Vector2D<float>, 4> AllAxes = { AxesA[0], AxesA[1], AxesB[0], AxesB[1] };

	for (const auto& Axis : AllAxes)
	{
		const auto [MinA, MaxA] = ProjectOBBOntoAxis(CornersA, Axis);
		const auto [MinB, MaxB] = ProjectOBBOntoAxis(CornersB, Axis);

		const float Overlap1 = MaxA - MinB;
		const float Overlap2 = MaxB - MinA;
		const float Overlap = std::min(Overlap1, Overlap2);

		if (Overlap < MinOverlap)
		{
			MinOverlap = Overlap;
			MinAxis = Axis;

			// Determine direction - push A away from B
			const float CenterA = (MinA + MaxA) * 0.5f;
			const float CenterB = (MinB + MaxB) * 0.5f;
			if (CenterA < CenterB)
			{
				MinAxis.X = -MinAxis.X;
				MinAxis.Y = -MinAxis.Y;
			}
		}
	}

	return { MinAxis.X * MinOverlap, MinAxis.Y * MinOverlap };
}

Vector2D<float> CollisionSystem::CalculateCircleOBBSeparation(
	const Vector2D<float>& CirclePos,
	const float Radius,
	const TransformComponent& RectTransform,
	const ShapeComponent& RectShape) const
{
	// Transform circle to OBB's local space
	const float CosTheta = std::cosf(-RectTransform.Rotation);
	const float SinTheta = std::sinf(-RectTransform.Rotation);
	
	const Vector2D<float> Relative = { CirclePos.X - RectTransform.Position.X, CirclePos.Y - RectTransform.Position.Y };
	
	const Vector2D<float> Local = {
		Relative.X * CosTheta - Relative.Y * SinTheta,
		Relative.X * SinTheta + Relative.Y * CosTheta
	};

	const float HalfW = RectShape.Rect.w * 0.5f;
	const float HalfH = RectShape.Rect.h * 0.5f;

	// Find closest point on rect to circle center (in local space)
	const Vector2D<float> Closest = {
		std::clamp(Local.X, -HalfW, HalfW),
		std::clamp(Local.Y, -HalfH, HalfH)
	};

	const Vector2D<float> Delta = { Local.X - Closest.X, Local.Y - Closest.Y };
	const float DistanceSquared = Delta.X * Delta.X + Delta.Y * Delta.Y;

	if (DistanceSquared < Radius * Radius)
	{
		Vector2D<float> LocalSeparation;

		if (DistanceSquared > 0.001f)
		{
			// Circle is outside but overlapping
			const float Distance = std::sqrtf(DistanceSquared);
			const float Penetration = Radius - Distance;
			LocalSeparation = { (Delta.X / Distance) * Penetration, (Delta.Y / Distance) * Penetration };
		}
		else
		{
			// Circle center is inside - push to nearest edge
			const float DistLeft = Local.X + HalfW;
			const float DistRight = HalfW - Local.X;
			const float DistTop = Local.Y + HalfH;
			const float DistBottom = HalfH - Local.Y;

			const float MinDist = std::min({ DistLeft, DistRight, DistTop, DistBottom });

			if (MinDist == DistLeft)
				LocalSeparation = { -(Radius + HalfW - Local.X), 0.f };
			else if (MinDist == DistRight)
				LocalSeparation = { Radius + HalfW + Local.X, 0.f };
			else if (MinDist == DistTop)
				LocalSeparation = { 0.f, -(Radius + HalfH - Local.Y) };
			else
				LocalSeparation = { 0.f, Radius + HalfH + Local.Y };
		}

		// Transform separation back to world space
		const float WorldX = LocalSeparation.X * CosTheta + LocalSeparation.Y * SinTheta;
		const float WorldY = -LocalSeparation.X * SinTheta + LocalSeparation.Y * CosTheta;
		return { WorldX, WorldY };
	}

	return { 0.f, 0.f };
}

Vector2D<float> CollisionSystem::CalculateCircleCircleSeparation(
	const TransformComponent& TransformA,
	const ShapeComponent& ShapeA,
	const TransformComponent& TransformB,
	const ShapeComponent& ShapeB) const
{
	const Vector2D<float> Delta = { TransformA.Position.X - TransformB.Position.X, TransformA.Position.Y - TransformB.Position.Y };
	const float DistanceSquared = Delta.X * Delta.X + Delta.Y * Delta.Y;

	if (DistanceSquared > 0.f)
	{
		const float Distance = std::sqrtf(DistanceSquared);
		const float RadiusA = ShapeA.Rect.w * 0.5f;
		const float RadiusB = ShapeB.Rect.w * 0.5f;
		const float Penetration = (RadiusA + RadiusB) - Distance;

		if (Penetration > 0.f)
		{
			// Push A away from B
			return { (Delta.X / Distance) * Penetration, (Delta.Y / Distance) * Penetration };
		}
	}

	return { 0.f, 0.f };
}

Vector2D<float> CollisionSystem::CalculateAABBSeparation(
	const TransformComponent& TransformA,
	const ShapeComponent& ShapeA,
	const TransformComponent& TransformB,
	const ShapeComponent& ShapeB) const
{
	const float LeftA = TransformA.Position.X + ShapeA.Rect.x;
	const float TopA = TransformA.Position.Y + ShapeA.Rect.y;
	const float RightA = LeftA + ShapeA.Rect.w;
	const float BottomA = TopA + ShapeA.Rect.h;

	const float LeftB = TransformB.Position.X + ShapeB.Rect.x;
	const float TopB = TransformB.Position.Y + ShapeB.Rect.y;
	const float RightB = LeftB + ShapeB.Rect.w;
	const float BottomB = TopB + ShapeB.Rect.h;

	// Calculate overlap on each axis
	const float OverlapLeft = RightA - LeftB;
	const float OverlapRight = RightB - LeftA;
	const float OverlapTop = BottomA - TopB;
	const float OverlapBottom = BottomB - TopA;

	// Find minimum overlap
	const float MinOverlapX = std::min(OverlapLeft, OverlapRight);
	const float MinOverlapY = std::min(OverlapTop, OverlapBottom);

	// Separate along axis with least penetration
	if (MinOverlapX < MinOverlapY)
	{
		return (OverlapLeft < OverlapRight) ? Vector2D<float>{ -MinOverlapX, 0.f } : Vector2D<float>{ MinOverlapX, 0.f };
	}
	else
	{
		return (OverlapTop < OverlapBottom) ? Vector2D<float>{ 0.f, -MinOverlapY } : Vector2D<float>{ 0.f, MinOverlapY };
	}
}

Vector2D<float> CollisionSystem::CalculateCircleRectSeparation(
	const Vector2D<float>& CirclePos,
	const float Radius,
	const TransformComponent& RectTransform,
	const ShapeComponent& RectShape) const
{
	const float RectLeft = RectTransform.Position.X + RectShape.Rect.x;
	const float RectTop = RectTransform.Position.Y + RectShape.Rect.y;
	const float RectRight = RectLeft + RectShape.Rect.w;
	const float RectBottom = RectTop + RectShape.Rect.h;

	const float ClosestX = std::clamp(CirclePos.X, RectLeft, RectRight);
	const float ClosestY = std::clamp(CirclePos.Y, RectTop, RectBottom);

	const Vector2D<float> Delta = { CirclePos.X - ClosestX, CirclePos.Y - ClosestY };
	const float DistanceSquared = Delta.X * Delta.X + Delta.Y * Delta.Y;

	if (DistanceSquared > 0.f && DistanceSquared < Radius * Radius)
	{
		const float Distance = std::sqrtf(DistanceSquared);
		const float Penetration = Radius - Distance;
		return { (Delta.X / Distance) * Penetration, (Delta.Y / Distance) * Penetration };
	}
	else if (DistanceSquared == 0.f)
	{
		// Circle center inside rect - push to nearest edge
		const float DistLeft = CirclePos.X - RectLeft;
		const float DistRight = RectRight - CirclePos.X;
		const float DistTop = CirclePos.Y - RectTop;
		const float DistBottom = RectBottom - CirclePos.Y;

		const float MinDist = std::min({ DistLeft, DistRight, DistTop, DistBottom });

		if (MinDist == DistLeft)
		{
			return { -(Radius + DistLeft), 0.f };
		}
		if (MinDist == DistRight)
		{
			return { Radius + DistRight, 0.f };
		}
		if (MinDist == DistTop)
		{
			return { 0.f, -(Radius + DistTop) };
		}

		return { 0.f, Radius + DistBottom };
	}

	return { 0.f, 0.f };
}

bool CollisionSystem::CheckCollision(
	const TransformComponent& TransformA,
	const ShapeComponent& ShapeA,
	const TransformComponent& TransformB,
	const ShapeComponent& ShapeB) const
{
	const bool AIsCircle = (ShapeA.Type == ShapeComponent::ShapeType::Circle);
	const bool BIsCircle = (ShapeB.Type == ShapeComponent::ShapeType::Circle);
	
	const bool AUsesRotation = !AIsCircle && (TransformA.Rotation != 0.f);
	const bool BUsesRotation = !BIsCircle && (TransformB.Rotation != 0.f);

	if (AIsCircle && BIsCircle)
	{
		const float RadiusA = ShapeA.Rect.w * 0.5f;
		const float RadiusB = ShapeB.Rect.w * 0.5f;
		return CircleCircleCollision(TransformA.Position, RadiusA, TransformB.Position, RadiusB);
	}
	else if (!AIsCircle && !BIsCircle)
	{
		if (AUsesRotation || BUsesRotation)
		{
			return OBBCollision(TransformA, ShapeA, TransformB, ShapeB);
		}
		else
		{
			return AABBCollision(TransformA, ShapeA, TransformB, ShapeB);
		}
	}
	else
	{
		if (AIsCircle)
		{
			const float Radius = ShapeA.Rect.w * 0.5f;
			if (BUsesRotation)
			{
				return CircleOBBCollision(TransformA.Position, Radius, TransformB, ShapeB);
			}
			else
			{
				return CircleAABBCollision(TransformA.Position, Radius, TransformB, ShapeB);
			}
		}
		else
		{
			const float Radius = ShapeB.Rect.w * 0.5f;
			if (AUsesRotation)
			{
				return CircleOBBCollision(TransformB.Position, Radius, TransformA, ShapeA);
			}
			else
			{
				return CircleAABBCollision(TransformB.Position, Radius, TransformA, ShapeA);
			}
		}
	}
}

bool CollisionSystem::CircleCircleCollision(
	const Vector2D<float>& PosA, const float RadiusA,
	const Vector2D<float>& PosB, const float RadiusB) const
{
	const Vector2D<float> Delta = { PosB.X - PosA.X, PosB.Y - PosA.Y };
	const float DistanceSquared = Delta.X * Delta.X + Delta.Y * Delta.Y;
	const float RadiusSum = RadiusA + RadiusB;

	return DistanceSquared <= (RadiusSum * RadiusSum);
}

bool CollisionSystem::AABBCollision(
	const TransformComponent& TransformA, const ShapeComponent& ShapeA,
	const TransformComponent& TransformB, const ShapeComponent& ShapeB) const
{
	const float LeftA = TransformA.Position.X + ShapeA.Rect.x;
	const float TopA = TransformA.Position.Y + ShapeA.Rect.y;
	const float RightA = LeftA + ShapeA.Rect.w;
	const float BottomA = TopA + ShapeA.Rect.h;

	const float LeftB = TransformB.Position.X + ShapeB.Rect.x;
	const float TopB = TransformB.Position.Y + ShapeB.Rect.y;
	const float RightB = LeftB + ShapeB.Rect.w;
	const float BottomB = TopB + ShapeB.Rect.h;

	return !(RightA < LeftB || RightB < LeftA || BottomA < TopB || BottomB < TopA);
}

bool CollisionSystem::OBBCollision(
	const TransformComponent& TransformA, const ShapeComponent& ShapeA,
	const TransformComponent& TransformB, const ShapeComponent& ShapeB) const
{
	const auto CornersA = GetOBBCorners(TransformA, ShapeA);
	const auto CornersB = GetOBBCorners(TransformB, ShapeB);

	const auto AxesA = GetOBBAxes(CornersA);
	const auto AxesB = GetOBBAxes(CornersB);

	return !TestSeparatingAxis(CornersA, CornersB, AxesA[0])
	       && !TestSeparatingAxis(CornersA, CornersB, AxesA[1])
	       && !TestSeparatingAxis(CornersA, CornersB, AxesB[0])
	       && !TestSeparatingAxis(CornersA, CornersB, AxesB[1]);
}

bool CollisionSystem::CircleAABBCollision(
	const Vector2D<float>& CirclePos, const float Radius,
	const TransformComponent& RectTransform, const ShapeComponent& RectShape) const
{
	const float RectLeft = RectTransform.Position.X + RectShape.Rect.x;
	const float RectTop = RectTransform.Position.Y + RectShape.Rect.y;
	const float RectRight = RectLeft + RectShape.Rect.w;
	const float RectBottom = RectTop + RectShape.Rect.h;

	const float ClosestX = std::clamp(CirclePos.X, RectLeft, RectRight);
	const float ClosestY = std::clamp(CirclePos.Y, RectTop, RectBottom);

	const Vector2D<float> Delta = { CirclePos.X - ClosestX, CirclePos.Y - ClosestY };
	const float DistanceSquared = Delta.X * Delta.X + Delta.Y * Delta.Y;

	return DistanceSquared <= (Radius * Radius);
}

bool CollisionSystem::CircleOBBCollision(
	const Vector2D<float>& CirclePos, const float Radius,
	const TransformComponent& RectTransform, const ShapeComponent& RectShape) const
{
	const float CosTheta = std::cosf(-RectTransform.Rotation);
	const float SinTheta = std::sinf(-RectTransform.Rotation);
	
	const Vector2D<float> Relative = { CirclePos.X - RectTransform.Position.X, CirclePos.Y - RectTransform.Position.Y };
	
	const Vector2D<float> Local = {
		Relative.X * CosTheta - Relative.Y * SinTheta,
		Relative.X * SinTheta + Relative.Y * CosTheta
	};

	const float HalfW = RectShape.Rect.w * 0.5f;
	const float HalfH = RectShape.Rect.h * 0.5f;

	const Vector2D<float> Closest = {
		std::clamp(Local.X, -HalfW, HalfW),
		std::clamp(Local.Y, -HalfH, HalfH)
	};

	const Vector2D<float> Delta = { Local.X - Closest.X, Local.Y - Closest.Y };
	const float DistanceSquared = Delta.X * Delta.X + Delta.Y * Delta.Y;

	return DistanceSquared <= (Radius * Radius);
}

std::array<Vector2D<float>, 4> CollisionSystem::GetOBBCorners(
	const TransformComponent& Transform,
	const ShapeComponent& Shape) const
{
	const float HalfW = Shape.Rect.w * 0.5f;
	const float HalfH = Shape.Rect.h * 0.5f;
	const float CosTheta = std::cosf(Transform.Rotation);
	const float SinTheta = std::sinf(Transform.Rotation);

	const std::array<Vector2D<float>, 4> LocalCorners = {{
		{ -HalfW, -HalfH },
		{  HalfW, -HalfH },
		{  HalfW,  HalfH },
		{ -HalfW,  HalfH }
	}};

	std::array<Vector2D<float>, 4> WorldCorners;
	for (int i = 0; i < 4; ++i)
	{
		const float RotatedX = LocalCorners[i].X * CosTheta - LocalCorners[i].Y * SinTheta;
		const float RotatedY = LocalCorners[i].X * SinTheta + LocalCorners[i].Y * CosTheta;
		
		WorldCorners[i].X = Transform.Position.X + RotatedX;
		WorldCorners[i].Y = Transform.Position.Y + RotatedY;
	}

	return WorldCorners;
}

std::array<Vector2D<float>, 2> CollisionSystem::GetOBBAxes(
	const std::array<Vector2D<float>, 4>& Corners) const
{
	std::array<Vector2D<float>, 2> Axes = {{
		{ Corners[1].X - Corners[0].X, Corners[1].Y - Corners[0].Y },
		{ Corners[3].X - Corners[0].X, Corners[3].Y - Corners[0].Y }
	}};

	// Normalize both axes
	for (int i = 0; i < 2; ++i)
	{
		const float Length = std::sqrtf(Axes[i].X * Axes[i].X + Axes[i].Y * Axes[i].Y);
		if (Length > 0.f)
		{
			Axes[i].X /= Length;
			Axes[i].Y /= Length;
		}
	}

	return Axes;
}

bool CollisionSystem::TestSeparatingAxis(
	const std::array<Vector2D<float>, 4>& CornersA,
	const std::array<Vector2D<float>, 4>& CornersB,
	const Vector2D<float>& Axis) const
{
	const auto [MinA, MaxA] = ProjectOBBOntoAxis(CornersA, Axis);
	const auto [MinB, MaxB] = ProjectOBBOntoAxis(CornersB, Axis);

	return (MaxA < MinB || MaxB < MinA);
}

Vector2D<float>	CollisionSystem::ProjectOBBOntoAxis(
	const std::array<Vector2D<float>, 4>& Corners,
	const Vector2D<float>& Axis) const
{
	float Min = Corners[0].X * Axis.X + Corners[0].Y * Axis.Y;
	float Max = Min;

	for (int i = 1; i < 4; ++i)
	{
		const float Projection = Corners[i].X * Axis.X + Corners[i].Y * Axis.Y;
		Min = std::min(Min, Projection);
		Max = std::max(Max, Projection);
	}

	return { Min, Max };
}