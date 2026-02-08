#include "MovementUtils.h"

Vector2D<float> MovementUtils::GetPositionAfterMove(const Vector2D<float>& CurrentPosition, const Vector2D<float>& Velocity, float DeltaTime)
{
	return {
		CurrentPosition.X + Velocity.X * DeltaTime,
		CurrentPosition.Y + Velocity.Y * DeltaTime
	};
}

float MovementUtils::GetRotationAfterMove(float CurrentRotation, float AngularVelocity, float DeltaTime)
{
	return CurrentRotation + AngularVelocity * DeltaTime;
}