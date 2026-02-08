#pragma once
#include "System.h"
#include "MathTypes.h"
#include <array>
#include <utility>

struct TransformComponent;
struct ShapeComponent;

class CollisionSystem : public System
{
public:
	void Update(const SystemUpdateContext& UpdateContext, float DeltaTime) override;

private:
	bool CheckCollision(
		const TransformComponent& TransformA,
		const ShapeComponent& ShapeA,
		const TransformComponent& TransformB,
		const ShapeComponent& ShapeB) const;

	Vector2D<float> CalculateSeparation(
		const TransformComponent& TransformA,
		const ShapeComponent& ShapeA,
		const TransformComponent& TransformB,
		const ShapeComponent& ShapeB) const;

	Vector2D<float> CalculateCircleCircleSeparation(
		const TransformComponent& TransformA,
		const ShapeComponent& ShapeA,
		const TransformComponent& TransformB,
		const ShapeComponent& ShapeB) const;

	Vector2D<float> CalculateAABBSeparation(
		const TransformComponent& TransformA,
		const ShapeComponent& ShapeA,
		const TransformComponent& TransformB,
		const ShapeComponent& ShapeB) const;

	Vector2D<float> CalculateCircleRectSeparation(
		const Vector2D<float>& CirclePos,
		float Radius,
		const TransformComponent& RectTransform,
		const ShapeComponent& RectShape) const;

	bool CircleCircleCollision(
		const Vector2D<float>& PosA, float RadiusA,
		const Vector2D<float>& PosB, float RadiusB) const;

	bool AABBCollision(
		const TransformComponent& TransformA, const ShapeComponent& ShapeA,
		const TransformComponent& TransformB, const ShapeComponent& ShapeB) const;

	bool OBBCollision(
		const TransformComponent& TransformA, const ShapeComponent& ShapeA,
		const TransformComponent& TransformB, const ShapeComponent& ShapeB) const;

	bool CircleAABBCollision(
		const Vector2D<float>& CirclePos, float Radius,
		const TransformComponent& RectTransform, const ShapeComponent& RectShape) const;

	bool CircleOBBCollision(
		const Vector2D<float>& CirclePos, float Radius,
		const TransformComponent& RectTransform, const ShapeComponent& RectShape) const;

	std::array<Vector2D<float>, 4> GetOBBCorners(
		const TransformComponent& Transform,
		const ShapeComponent& Shape) const;

	std::array<Vector2D<float>, 2> GetOBBAxes(
		const std::array<Vector2D<float>, 4>& Corners) const;

	bool TestSeparatingAxis(
		const std::array<Vector2D<float>, 4>& CornersA,
		const std::array<Vector2D<float>, 4>& CornersB,
		const Vector2D<float>& Axis) const;

	Vector2D<float> ProjectOBBOntoAxis(
		const std::array<Vector2D<float>, 4>& Corners,
		const Vector2D<float>& Axis) const;
};