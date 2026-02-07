#pragma once
#include <memory>

class Game;
class SDL_Window;
class World;
class WorldInitializationData;

/* GameRunner manages the lifecycle of a CLECS game.
 * It handles SDL initialization, window creation, and the main game loop.
 */
class GameRunner
{
public:
	/* Runs the specified game.
	 * Returns the exit code (0 for success, non-zero for errors).
	 */
	static int Run(std::unique_ptr<Game> GameInstance);

private:
	static bool InitializeSDL();
	static void ShutdownSDL();
	static SDL_Window* CreateGameWindow(const Game& GameInstance);
	static bool InitializeWorld(World& GameWorld, WorldInitializationData& Data);
	static int RunGameLoop(World& GameWorld);
};