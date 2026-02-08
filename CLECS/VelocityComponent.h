#pragma once
#include "MathTypes.h"

// Velocity component for movement
struct VelocityComponent
{
    Vector2D<float> Velocity = { 0.f, 0.f };
	float AngularVelocity = 0.f;
};