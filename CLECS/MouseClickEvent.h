#pragma once
#include "MathTypes.h"
#include <SDL3/SDL_mouse.h>

// Send when the user clicks a mouse button. Contains the position of the click and which button was clicked.
struct MouseClickEvent
{
	Vector2D<float> Position;
	Uint8 Button = SDL_BUTTON_LEFT;
};