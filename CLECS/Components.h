#pragma once
#include <SDL3/SDL.h>

// Transform component for position, rotation, scale
struct TransformComponent
{
    float x = 0.0f;
    float y = 0.0f;
    float rotation = 0.0f;
    float scaleX = 1.0f;
    float scaleY = 1.0f;
};

// Sprite component for rendering textures
struct SpriteComponent
{
    SDL_Texture* texture = nullptr;
    SDL_Rect sourceRect = {0, 0, 0, 0};
    SDL_FColor colorMod = {1.0f, 1.0f, 1.0f, 1.0f};
    int zOrder = 0;  // For render ordering
    bool visible = true;
};

// Primitive shape component
struct ShapeComponent
{
    enum class ShapeType { Rectangle, Circle, Line };
    ShapeType type = ShapeType::Rectangle;
    SDL_FRect rect = {0, 0, 0, 0};
    SDL_FColor color = {1.0f, 1.0f, 1.0f, 1.0f};
    bool filled = true;
    bool visible = true;
};