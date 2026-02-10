#pragma once
#include "MathTypes.h"

struct CurrentStageResetComponent
{
	Vector2D<float> BallInitialPosition = { 0.f, 0.f };
	Vector2D<float> BallInitialVelocity = { 0.f, 0.f };
	Vector2D<float> PlayerInitialPosition = { 0.f, 0.f };
};