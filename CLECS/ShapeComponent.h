#pragma once
#include <SDL3/SDL.h>

// Primitive shape component
struct ShapeComponent
{
    enum class ShapeType { Rectangle, Circle, Line };
    ShapeType Type = ShapeType::Rectangle;
    SDL_FRect Rect = {0, 0, 0, 0};
    SDL_FColor Color = {1.f, 1.f, 1.f, 1.f};
    bool Filled = true;
    bool Visible = true;
};