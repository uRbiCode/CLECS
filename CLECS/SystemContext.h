#pragma once
#include "FontManager.h"

class EntityAdmin;
class InputState;
class EventBus;
struct SDL_Window;
struct SDL_Renderer;
class TextureManager;
class AudioManager;

struct Managers
{
	TextureManager& TextureManager;
	AudioManager& AudioManager;
	FontManager& FontManager;
};

struct SystemContext
{
	EntityAdmin& EntityAdmin;
	SDL_Window& Window;
	SDL_Renderer& Renderer;
	const InputState& Input;
	EventBus& EventBus;
	Managers Managers;
};
