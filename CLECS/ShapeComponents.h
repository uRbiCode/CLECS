#pragma once
#include <SDL3/SDL_rect.h>

// Indicates whether a shape should be filled or just outlined.
// Shape is only an outline by default. Presence of this component indicates otherwise.
struct ShapeFillComponent
{
};

// Represents a rectangle shape.
struct RectComponent
{
    SDL_FRect Rect = {0.f, 0.f, 0.f, 0.f};
};

// Represents a circle shape.
struct CircleComponent
{
    float Radius = 0.f;
};