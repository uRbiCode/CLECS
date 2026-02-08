#pragma once

class EntityManager;
struct SDL_Window;
struct SDL_Renderer;

struct SystemUpdateContext
{
	EntityManager& EntityManager;
	SDL_Window& Window;
	SDL_Renderer& Renderer;
	float DeltaTime = 0.f;
};
