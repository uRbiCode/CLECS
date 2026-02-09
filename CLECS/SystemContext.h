#pragma once

class EntityManager;
class InputState;
class EventBus;
struct SDL_Window;
struct SDL_Renderer;

struct SystemContext
{
	EntityManager& EntityManager;
	SDL_Window& Window;
	SDL_Renderer& Renderer;
	const InputState& Input;
	EventBus& EventBus;
};
