#include "SDLUtils.h"
#include "SDL3/SDL.h"
#include <SDL3/SDL_render.h>

Vector2D<float> SDLUtils::GetRendererLogicalPresentation(SDL_Renderer* Renderer)
{
	if (Renderer == nullptr)
		return {};

	Vector2D<int> WindowSize = {};
	SDL_RendererLogicalPresentation LogicalPresentation = SDL_LOGICAL_PRESENTATION_LETTERBOX;
	SDL_GetRenderLogicalPresentation(Renderer, &WindowSize.X, &WindowSize.Y, &LogicalPresentation);

	return {static_cast<float>(WindowSize.X), static_cast<float>(WindowSize.Y)};
}

Vector2D<float> SDLUtils::TranslateCoordinatesFromWindowToLogical(SDL_Renderer* Renderer, const Vector2D<float>& Coordinates)
{
	Vector2D<float> TranslatedCoordinates = {};
	SDL_RenderCoordinatesFromWindow(Renderer, Coordinates.X, Coordinates.Y, &TranslatedCoordinates.X, &TranslatedCoordinates.Y);
	return TranslatedCoordinates;
}