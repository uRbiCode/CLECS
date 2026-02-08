#pragma once
#include <memory>

class Game;
class World;
class WorldInitializationData;

/* GameRunner manages the lifecycle of a CLECS game.
 * It handles SDL initialization and the main game loop.
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
	static bool InitializeWorld(World& GameWorld, WorldInitializationData& Data);
	static void Shutdown(Game& GameInstance, World& GameWorld);
	static int RunGameLoop(World& GameWorld);
};