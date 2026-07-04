#pragma once
#include "MathTypes.h"

// Tracks initial velocity of an entity that can be reset to.
struct VelocityResetComponent
{
    Vector2D<float> ResetVelocity = { 0.f, 0.f };
};

// Tracks initial position of an entity that can be reset to.
struct PositionResetComponent
{
    Vector2D<float> ResetPosition = { 0.f, 0.f };
};