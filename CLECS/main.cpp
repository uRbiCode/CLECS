#include "SDL3/SDL.h"
#include "SDL3/SDL_main.h"
#include "GameRunner.h"
#include "Game.h"

// This file is intentionally minimal - it only includes SDL main setup.
// Users define their game entry point using CLECS_DEFINE_GAME_ENTRY macro in their own file.

int main(int argc, char* argv[])
{
	return GameRunner::Run(CreateGame());
}