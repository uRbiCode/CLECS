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

/* SystemContext is a struct that encapsulates all the necessary context and resources that systems need to operate.
 * It is passed to each system's Initialization and Update function, as well as via events.
 */
struct SystemContext
{
	EntityAdmin& EntityAdmin;
	SDL_Window& Window;
	SDL_Renderer& Renderer;
	const InputState& Input;
	EventBus& EventBus;
	Managers Managers;
};