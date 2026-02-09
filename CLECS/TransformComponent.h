#pragma once
#include "MathTypes.h"

struct TransformComponent
{
    Vector2D<float> Position = {0.f, 0.f};
    float Rotation = 0.f;
    Vector2D<float> Scale = {1.f, 1.f};
};