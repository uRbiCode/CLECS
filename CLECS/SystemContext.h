#pragma once

class EntityAdmin;
class InputState;
class EventBus;
struct SDL_Window;
struct SDL_Renderer;
class TextureManager;

struct SystemContext
{
	EntityAdmin& EntityAdmin;
	SDL_Window& Window;
	SDL_Renderer& Renderer;
	const InputState& Input;
	EventBus& EventBus;
	TextureManager& TextureManager;
};
