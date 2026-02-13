#pragma once
#include <string>
#include <SDL3/SDL_pixels.h>

// Used by RenderSystem to display text.
struct TextComponent
{
	std::string Text;
	std::string FontFilePath;
	int FontPointSize = 12;
	SDL_Color Color = {255, 255, 255, 255};
};