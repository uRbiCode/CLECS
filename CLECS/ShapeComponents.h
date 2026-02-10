#pragma once
#include <SDL3/SDL.h>
#include "MathTypes.h"

struct ShapeFillComponent
{
	bool Filled = true;
};

struct ColorComponent
{
    SDL_FColor Color = {1.f, 1.f, 1.f, 1.f};
};

struct RectComponent
{
    SDL_FRect Rect = {0, 0, 0, 0};
};

struct CircleComponent
{
    float Radius = 0.f;
};

struct LineComponent
{
	Vector2D<float> Start = { 0.f, 0.f };
    Vector2D<float> End = { 0.f, 0.f };
};