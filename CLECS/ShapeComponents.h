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