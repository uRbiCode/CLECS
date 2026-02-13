#include "SDL3/SDL.h"
#include "SDL3/SDL_main.h"
#include "GameRunner.h"
#include "Game.h"

/* Invokes CLECS GameLoop using SDL macros.
 * Requires CLECS_DEFINE_GAME_ENTRY to be defined. See Arkanoid.h.
 */
int main(int argc, char* argv[])
{
	return GameRunner::Run(CreateGame());
}