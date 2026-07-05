#pragma once
#include <memory>

class Game;
class World;
class WorldInitializationData;
struct RendererInitializationData;

/* GameRunner manages the lifecycle of a CLECS game.
 * It handles SDL initialization and the main game loop.
 */
class GameRunner
{
public:
	static int Run(std::unique_ptr<Game> GameInstance);

private:
	static bool InitializeSDL();
	static bool InitializeWorld(World& GameWorld, WorldInitializationData&& WorldData, RendererInitializationData&& RendererData);
	static int RunGameLoop(World& GameWorld);
	static void Shutdown(Game& GameInstance, World& GameWorld);
};