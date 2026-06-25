#pragma once
#include <SDL3/SDL_rect.h>

struct SDL_Texture;

// Used by RenderSystem to display textures.
struct TextureComponent
{
	SDL_Texture* Texture = nullptr;
	SDL_FRect SourceRect{};
};