#include "SDL3/SDL.h"
#include "SDL3/SDL_main.h"
#include "GameRunner.h"
#include "Game.h"

int main(int argc, char* argv[])
{
	return GameRunner::Run(CreateGame());
}