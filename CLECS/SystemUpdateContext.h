#pragma once

class EntityManager;
class InputState;
struct SDL_Window;
struct SDL_Renderer;

struct SystemUpdateContext
{
	EntityManager& EntityManager;
	SDL_Window& Window;
	SDL_Renderer& Renderer;
	const InputState& Input;
};
