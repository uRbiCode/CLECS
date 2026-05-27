#pragma once
#include "QueryContext.h"

class QueryContext;
class CommandRunner;

class InputState;
class EventBus;
struct SDL_Window;
struct SDL_Renderer;
class TextureManager;
class AudioManager;
class FontManager;

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
	QueryContext QueryContext;
	CommandRunner& Commands;

	SDL_Window& Window;
	SDL_Renderer& Renderer;
	const InputState& Input;
	EventBus& EventBus;
	Managers Managers;
};