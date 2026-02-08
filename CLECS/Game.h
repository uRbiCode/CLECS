#pragma once
#include <memory>

class WorldInitializationData;
struct RendererInitializationData;

/* Game is the main entrypoint for CLECS applications.
 * Inherit from this class to define your game's initialization logic and configuration.
 */
class Game
{
public:
	virtual ~Game() = default;

	/* Called once during game initialization.
	 * Use this to register systems, create initial entities, and set up game state.
	 * Return false to abort game initialization.
	 */
	virtual bool Initialize(WorldInitializationData& Data) = 0;

	/* Called once during game shutdown.
	 * Use this to clean up any game-specific resources.
	 */
	virtual void Shutdown() = 0;

	virtual RendererInitializationData GetRendererConfig() const = 0;
};

std::unique_ptr<Game> CreateGame();

#define CLECS_DEFINE_GAME_ENTRY(GameClass) \
	std::unique_ptr<Game> CreateGame() \
	{ \
		return std::make_unique<GameClass>(); \
	}
