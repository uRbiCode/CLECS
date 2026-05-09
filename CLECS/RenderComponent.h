#pragma once
#include "SDL3/SDL.h"

// Used by RenderSystem to set SDL_Renderer color.
struct ColorComponent
{
    SDL_FColor Color = {1.f, 1.f, 1.f, 1.f};
};

// Used by RenderSystem to recognize components meaningful for rendering.
struct RenderComponent
{
	int Layer = 0;
    bool Visible = true;
};