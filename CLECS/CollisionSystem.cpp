#include "CollisionSystem.h"
#include "SystemContext.h"
#include "EntityAdmin.h"
#include "CollisionComponent.h"
#include "TransformComponent.h"
#include "ShapeComponent.h"
#include "VelocityComponent.h"
#include <cmath>
#include <algorithm>
#include <array>

namespace
{
	struct CollisionResult
	{
		bool Collides;
		Vector2D<float> Separation;
	};

	struct CollisionShape
	{
		Vector2D<float> Position;
		float Rotation;
		const ShapeComponent* Shape;
		
		bool IsCircle() const { return Shape->Type == ShapeComponent::ShapeType::Circle; }
		bool IsRotated() const { return !IsCircle() && Rotation != 0.f; }
		float GetRadius() const { return Shape->Rect.w * 0.5f; }
		
		static CollisionShape From(const TransformComponent& Transform, const ShapeComponent& Shape)
		{
			return { Transform.Position, Transform.Rotation, &Shape };
		}
		
		CollisionShape Predict(const Vector2D<float>& VelDelta, float RotDelta) const
		{
			return { { Position.X + VelDelta.X, Position.Y + VelDelta.Y }, Rotation + RotDelta, Shape };
		}
	};

	struct OBBGeometry
	{
		std::array<Vector2D<float>, 4> Corners;
		std::array<Vector2D<float>, 2> Axes;
		
		static OBBGeometry Compute(const CollisionShape& Shape)
		{
			const float HalfW = Shape.Shape->Rect.w * 0.5f;
			const float HalfH = Shape.Shape->Rect.h * 0.5f;
			const float CosTheta = std::cosf(Shape.Rotation);
			const float SinTheta = std::sinf(Shape.Rotation);
			
			OBBGeometry Result;
			const Vector2D<float> LocalCorners[4] = { {-HalfW, -HalfH}, {HalfW, -HalfH}, {HalfW, HalfH}, {-HalfW, HalfH} };
			
			for (int i = 0; i < 4; ++i)
			{
				const float RotX = LocalCorners[i].X * CosTheta - LocalCorners[i].Y * SinTheta;
				const float RotY = LocalCorners[i].X * SinTheta + LocalCorners[i].Y * CosTheta;
				Result.Corners[i] = { Shape.Position.X + RotX, Shape.Position.Y + RotY };
			}
			
			for (int i = 0; i < 2; ++i)
			{
				const int idx = i * 3;
				Result.Axes[i] = { Result.Corners[idx == 0 ? 1 : 3].X - Result.Corners[0].X,
				                    Result.Corners[idx == 0 ? 1 : 3].Y - Result.Corners[0].Y };
				const float Length = std::sqrtf(Result.Axes[i].X * Result.Axes[i].X + Result.Axes[i].Y * Result.Axes[i].Y);
				if (Length > 0.f)
				{
					Result.Axes[i].X /= Length;
					Result.Axes[i].Y /= Length;
				}
			}
			
			return Result;
		}
		
		Vector2D<float> Project(const Vector2D<float>& Axis) const
		{
			float Min = Corners[0].X * Axis.X + Corners[0].Y * Axis.Y;
			float Max = Min;
			for (int i = 1; i < 4; ++i)
			{
				const float Proj = Corners[i].X * Axis.X + Corners[i].Y * Axis.Y;
				Min = std::min(Min, Proj);
				Max = std::max(Max, Proj);
			}
			return { Min, Max };
		}
	};

	Vector2D<float> ToLocalSpace(const Vector2D<float>& WorldPos, const CollisionShape& LocalFrame)
	{
		const Vector2D<float> Relative = { WorldPos.X - LocalFrame.Position.X, WorldPos.Y - LocalFrame.Position.Y };
		const float CosTheta = std::cosf(-LocalFrame.Rotation);
		const float SinTheta = std::sinf(-LocalFrame.Rotation);
		return { Relative.X * CosTheta - Relative.Y * SinTheta, Relative.X * SinTheta + Relative.Y * CosTheta };
	}

	Vector2D<float> ToWorldSpace(const Vector2D<float>& LocalVec, float Rotation)
	{
		const float CosTheta = std::cosf(Rotation);
		const float SinTheta = std::sinf(Rotation);
		return { LocalVec.X * CosTheta - LocalVec.Y * SinTheta, LocalVec.X * SinTheta + LocalVec.Y * CosTheta };
	}

	CollisionResult CircleVsCircle(const CollisionShape& A, const CollisionShape& B)
	{
		const Vector2D<float> Delta = { A.Position.X - B.Position.X, A.Position.Y - B.Position.Y };
		const float DistSq = Delta.X * Delta.X + Delta.Y * Delta.Y;
		const float RadiusSum = A.GetRadius() + B.GetRadius();
		
		if (DistSq > RadiusSum * RadiusSum || DistSq <= 0.f)
			return { DistSq <= RadiusSum * RadiusSum, {0.f, 0.f} };
		
		const float Dist = std::sqrtf(DistSq);
		const float Penetration = RadiusSum - Dist;
		return { true, { (Delta.X / Dist) * Penetration, (Delta.Y / Dist) * Penetration } };
	}

	CollisionResult CircleVsRect(const CollisionShape& Circle, const CollisionShape& Rect)
	{
		const Vector2D<float> Local = Rect.IsRotated() ? ToLocalSpace(Circle.Position, Rect) : 
		                               Vector2D<float>{ Circle.Position.X - Rect.Position.X, Circle.Position.Y - Rect.Position.Y };
		
		const float HalfW = Rect.Shape->Rect.w * 0.5f;
		const float HalfH = Rect.Shape->Rect.h * 0.5f;
		const Vector2D<float> Closest = { std::clamp(Local.X, -HalfW, HalfW), std::clamp(Local.Y, -HalfH, HalfH) };
		const Vector2D<float> Delta = { Local.X - Closest.X, Local.Y - Closest.Y };
		const float DistSq = Delta.X * Delta.X + Delta.Y * Delta.Y;
		const float Radius = Circle.GetRadius();
		
		if (DistSq > Radius * Radius)
			return { false, {0.f, 0.f} };
		
		Vector2D<float> LocalSep = {0.f, 0.f};
		
		if (DistSq > 0.001f)
		{
			const float Dist = std::sqrtf(DistSq);
			const float Penetration = Radius - Dist;
			LocalSep = { (Delta.X / Dist) * Penetration, (Delta.Y / Dist) * Penetration };
		}
		else
		{
			const float Dists[4] = { Local.X + HalfW, HalfW - Local.X, Local.Y + HalfH, HalfH - Local.Y };
			const auto MinIdx = static_cast<size_t>(std::min_element(Dists, Dists + 4) - Dists);
			
			if (MinIdx == 0)
			{
				LocalSep = { -(Radius + HalfW - Local.X), 0.f };
			}
			else if (MinIdx == 1)
			{	
				LocalSep = { Radius + HalfW + Local.X, 0.f };
			}
			else if (MinIdx == 2)
			{
				LocalSep = { 0.f, -(Radius + HalfH - Local.Y) };
			}
			else
			{
				LocalSep = { 0.f, Radius + HalfH + Local.Y };
			}
		}
		
		const Vector2D<float> WorldSep = Rect.IsRotated() ? ToWorldSpace(LocalSep, Rect.Rotation) : LocalSep;
		return { true, WorldSep };
	}

	CollisionResult AABBVsAABB(const CollisionShape& A, const CollisionShape& B)
	{
		const float LeftA = A.Position.X + A.Shape->Rect.x;
		const float TopA = A.Position.Y + A.Shape->Rect.y;
		const float RightA = LeftA + A.Shape->Rect.w;
		const float BottomA = TopA + A.Shape->Rect.h;
		
		const float LeftB = B.Position.X + B.Shape->Rect.x;
		const float TopB = B.Position.Y + B.Shape->Rect.y;
		const float RightB = LeftB + B.Shape->Rect.w;
		const float BottomB = TopB + B.Shape->Rect.h;
		
		if (RightA < LeftB || RightB < LeftA || BottomA < TopB || BottomB < TopA)
			return { false, {0.f, 0.f} };
		
		const float OverlapX[2] = { RightA - LeftB, RightB - LeftA };
		const float OverlapY[2] = { BottomA - TopB, BottomB - TopA };
		const float MinOverlapX = std::min(OverlapX[0], OverlapX[1]);
		const float MinOverlapY = std::min(OverlapY[0], OverlapY[1]);
		
		Vector2D<float> Sep = {0.f, 0.f};
		if (MinOverlapX < MinOverlapY)
		{
			Sep.X = (OverlapX[0] < OverlapX[1]) ? -MinOverlapX : MinOverlapX;
		}
		else
		{
			Sep.Y = (OverlapY[0] < OverlapY[1]) ? -MinOverlapY : MinOverlapY;
		}
		
		return { true, Sep };
	}

	CollisionResult OBBVsOBB(const CollisionShape& A, const CollisionShape& B)
	{
		const OBBGeometry GeomA = OBBGeometry::Compute(A);
		const OBBGeometry GeomB = OBBGeometry::Compute(B);
		
		float MinOverlap = std::numeric_limits<float>::max();
		Vector2D<float> MinAxis = {0.f, 0.f};
		
		for (int i = 0; i < 2; ++i)
		{
			for (const auto* Geom : {&GeomA, &GeomB})
			{
				const Vector2D<float>& Axis = (Geom == &GeomA) ? GeomA.Axes[i] : GeomB.Axes[i];
				const Vector2D<float> ProjA = GeomA.Project(Axis);
				const Vector2D<float> ProjB = GeomB.Project(Axis);
				
				if (ProjA.Y < ProjB.X || ProjB.Y < ProjA.X)
					return { false, {0.f, 0.f} };
				
				const float Overlap = std::min(ProjA.Y - ProjB.X, ProjB.Y - ProjA.X);
				if (Overlap < MinOverlap)
				{
					MinOverlap = Overlap;
					MinAxis = Axis;
					if ((ProjA.X + ProjA.Y) * 0.5f < (ProjB.X + ProjB.Y) * 0.5f)
					{
						MinAxis = { -MinAxis.X, -MinAxis.Y };
					}
				}
			}
		}
		
		return { true, { MinAxis.X * MinOverlap, MinAxis.Y * MinOverlap } };
	}

	CollisionResult CheckAndResolve(const CollisionShape& A, const CollisionShape& B)
	{
		if (A.IsCircle() && B.IsCircle())
		{
			return CircleVsCircle(A, B);
		}
		
		if (A.IsCircle())
		{
			return CircleVsRect(A, B);
		}
		
		if (B.IsCircle())
		{
			const auto Result = CircleVsRect(B, A);
			return { Result.Collides, { -Result.Separation.X, -Result.Separation.Y } };
		}
		
		if (A.IsRotated() || B.IsRotated())
		{
			return OBBVsOBB(A, B);
		}
		
		return AABBVsAABB(A, B);
	}
}

void CollisionSystem::Update(const SystemContext& Context, float DeltaTime) const
{
	auto& Admin = Context.EntityAdmin;
	auto CollisionGroup = Admin.GetGroup<TransformComponent, ShapeComponent, CollisionComponent>();

	// Phase 1: Resolve existing overlaps
	for (size_t i = 0; i < CollisionGroup.Size(); ++i)
	{
		const Entity EntityA = CollisionGroup[i];
		const auto& CollisionA = Admin.GetComponent<CollisionComponent>(EntityA);
		auto& TransformA = Admin.AccessComponent<TransformComponent>(EntityA);
		const auto& ShapeA = Admin.GetComponent<ShapeComponent>(EntityA);
		const CollisionShape A = CollisionShape::From(TransformA, ShapeA);

		for (size_t j = i + 1; j < CollisionGroup.Size(); ++j)
		{
			const Entity EntityB = CollisionGroup[j];
			const auto& CollisionB = Admin.GetComponent<CollisionComponent>(EntityB);
			const auto ResponseA = CollisionA.ResponseTable[ChannelToIndex(CollisionB.Channel)];
			
			if (ResponseA != CollisionResponse::Block)
				continue;

			const auto& TransformB = Admin.GetComponent<TransformComponent>(EntityB);
			const auto& ShapeB = Admin.GetComponent<ShapeComponent>(EntityB);
			const CollisionShape B = CollisionShape::From(TransformB, ShapeB);

			const auto Result = CheckAndResolve(A, B);
			if (Result.Collides)
			{
				TransformA.Position.X += Result.Separation.X;
				TransformA.Position.Y += Result.Separation.Y;

				if (Admin.HasComponent<VelocityComponent>(EntityA) && !Admin.HasComponent<VelocityComponent>(EntityB))
				{
					auto& VelocityA = Admin.AccessComponent<VelocityComponent>(EntityA);
					const float SepMag = std::sqrtf(Result.Separation.X * Result.Separation.X + Result.Separation.Y * Result.Separation.Y);
					if (SepMag > 0.001f)
					{
						const Vector2D<float> SepDir = { Result.Separation.X / SepMag, Result.Separation.Y / SepMag };
						const float VelDot = VelocityA.Velocity.X * SepDir.X + VelocityA.Velocity.Y * SepDir.Y;
						if (VelDot < 0.f)
						{
							VelocityA.Velocity.X -= VelDot * SepDir.X;
							VelocityA.Velocity.Y -= VelDot * SepDir.Y;
						}
					}
				}
			}
		}
	}

	// Phase 2: Prevent new collisions from velocity with sliding
	for (size_t i = 0; i < CollisionGroup.Size(); ++i)
	{
		const Entity EntityA = CollisionGroup[i];
		if (!Admin.HasComponent<VelocityComponent>(EntityA))
			continue;

		const auto& CollisionA = Admin.GetComponent<CollisionComponent>(EntityA);
		const auto& TransformA = Admin.GetComponent<TransformComponent>(EntityA);
		const auto& ShapeA = Admin.GetComponent<ShapeComponent>(EntityA);
		auto& VelocityA = Admin.AccessComponent<VelocityComponent>(EntityA);
		const CollisionShape A = CollisionShape::From(TransformA, ShapeA);

		for (size_t j = 0; j < CollisionGroup.Size(); ++j)
		{
			if (i == j)
				continue;

			const Entity EntityB = CollisionGroup[j];
			if (Admin.HasComponent<VelocityComponent>(EntityB))
				continue;

			const auto& CollisionB = Admin.GetComponent<CollisionComponent>(EntityB);
			if (CollisionA.ResponseTable[ChannelToIndex(CollisionB.Channel)] != CollisionResponse::Block)
				continue;

			const auto& TransformB = Admin.GetComponent<TransformComponent>(EntityB);
			const auto& ShapeB = Admin.GetComponent<ShapeComponent>(EntityB);
			const CollisionShape B = CollisionShape::From(TransformB, ShapeB);

			const bool CurrentlyColliding = CheckAndResolve(A, B).Collides;

			if (!CurrentlyColliding)
			{
				if (VelocityA.AngularVelocity != 0.f)
				{
					const CollisionShape PredictedA = A.Predict({0.f, 0.f}, VelocityA.AngularVelocity * DeltaTime);
					if (CheckAndResolve(PredictedA, B).Collides)
					{
						VelocityA.AngularVelocity = 0.f;
					}
				}

				if (VelocityA.Velocity.X != 0.f || VelocityA.Velocity.Y != 0.f)
				{
					const Vector2D<float> VelDelta = { VelocityA.Velocity.X * DeltaTime, VelocityA.Velocity.Y * DeltaTime };
					const CollisionShape PredictedA = A.Predict(VelDelta, 0.f);
					const auto PredictedResult = CheckAndResolve(PredictedA, B);
					
					if (PredictedResult.Collides)
					{
						// Instead of zeroing velocity, project it along the collision normal to allow sliding
						const float SepMag = std::sqrtf(PredictedResult.Separation.X * PredictedResult.Separation.X + 
						                                 PredictedResult.Separation.Y * PredictedResult.Separation.Y);
						
						if (SepMag > 0.001f)
						{
							// Normalize the separation vector to get collision normal
							const Vector2D<float> Normal = { PredictedResult.Separation.X / SepMag, PredictedResult.Separation.Y / SepMag };
							
							// Remove the velocity component pointing into the obstacle (dot product with normal)
							const float VelDot = VelocityA.Velocity.X * Normal.X + VelocityA.Velocity.Y * Normal.Y;
							
							if (VelDot < 0.f)
							{
								// Subtract the normal component, keeping the tangential (sliding) component
								VelocityA.Velocity.X -= VelDot * Normal.X;
								VelocityA.Velocity.Y -= VelDot * Normal.Y;
							}
						}
						else
						{
							VelocityA.Velocity = { 0.f, 0.f };
						}
					}
				}
			}
		}
	}
}