#pragma once
#include "MathTypes.h"
#include <SDL3/SDL_mouse.h>

struct MouseClickEvent
{
	Vector2D<float> Position;
	Uint8 Button = SDL_BUTTON_LEFT;
};