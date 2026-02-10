#pragma once
#include <SDL3/SDL_rect.h>

struct SDL_Texture;

struct TextureComponent
{
	SDL_Texture* Texture = nullptr;
	SDL_FRect SourceRect = {};
	bool Tiled = false;
};