#pragma once
#include "MathTypes.h"

namespace MovementUtils
{
	Vector2D<float> GetPositionAfterMove(
		const Vector2D<float>& CurrentPosition,
		const Vector2D<float>& Velocity,
		float DeltaTime);

	float GetRotationAfterMove(float CurrentRotation, float AngularVelocity, float DeltaTime);
}

