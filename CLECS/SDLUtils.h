#pragma once
#include "MathTypes.h"

struct SDL_Renderer;

// Utility functions related to SDL.
namespace SDLUtils
{
	Vector2D<float> GetRendererLogicalPresentation(SDL_Renderer* Renderer);
	Vector2D<float> TranslateCoordinatesFromWindowToLogical(SDL_Renderer* Renderer, const Vector2D<float>& Coordinates);
};