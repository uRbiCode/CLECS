#pragma once
#include <SDL3/SDL.h>
#include "MathTypes.h"

// Transform component for position, rotation, scale
struct TransformComponent
{
    Vector2D<float> Position = {0.f, 0.f};
    float Rotation = 0.f;
    Vector2D<float> Scale = {1.f, 1.f};
};

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

// Velocity component for movement
struct VelocityComponent
{
    Vector2D<float> Velocity = {0.f, 0.f};
};

// Player controller component to mark entities as player-controlled
struct PlayerControllerComponent
{
    float MoveSpeed = 300.f; // Units per second
};