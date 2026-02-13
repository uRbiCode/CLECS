#pragma once
#include "MathTypes.h"
#include <SDL3/SDL.h>

// Indicates whether a shape should be filled or just outlined.
struct ShapeFillComponent
{
	bool Filled = true;
};

// Represents a rectangle shape.
struct RectComponent
{
    SDL_FRect Rect = {0, 0, 0, 0};
};

// Represents a circle shape.
struct CircleComponent
{
    float Radius = 0.f;
};

// Represents a line defined by a start and end point.
struct LineComponent
{
	Vector2D<float> Start = { 0.f, 0.f };
    Vector2D<float> End = { 0.f, 0.f };
};