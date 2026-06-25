#pragma once
#include "SDL3/SDL.h"

// Used by RenderSystem to render Background layer.
struct BackgroundRenderComponent
{
    SDL_FColor Color = {1.f, 1.f, 1.f, 1.f};
};

// Used by RenderSystem to render Game layer.
struct GameRenderComponent
{
    SDL_FColor Color = {1.f, 1.f, 1.f, 1.f};
};

// Used by RenderSystem to render UI layer.
struct UIRenderComponent
{
    SDL_FColor Color = {1.f, 1.f, 1.f, 1.f};
};