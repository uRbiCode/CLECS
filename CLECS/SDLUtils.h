#pragma once
#include "MathTypes.h"

struct SDL_Renderer;
struct SDL_Window;

// Utility functions related to SDL.
namespace SDLUtils
{
	Vector2D<int> GetWindowSize(SDL_Window* Window);
	Vector2D<int> GetRendererLogicalPresentation(SDL_Renderer* Renderer);
	Vector2D<float> TranslateCoordinatesFromWindowToLogical(SDL_Renderer* Renderer, SDL_Window* Window, const Vector2D<float>& Coordinates);
};