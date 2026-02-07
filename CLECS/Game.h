#pragma once
#include "World.h"

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
	virtual bool Initialize(World& GameWorld) = 0;

	/* Called once during game shutdown.
	 * Use this to clean up any game-specific resources.
	 */
	virtual void Shutdown(World& GameWorld) {}

	/* Returns the title that will be displayed in the window.
	 */
	virtual const char* GetWindowTitle() const { return "CLECS Game"; }

	/* Returns the initial window width.
	 */
	virtual int GetWindowWidth() const { return 1280; }

	/* Returns the initial window height.
	 */
	virtual int GetWindowHeight() const { return 720; }
};

std::unique_ptr<Game> CreateGame();

#define CLECS_DEFINE_GAME_ENTRY(GameClass) \
	std::unique_ptr<Game> CreateGame() \
	{ \
		return std::make_unique<GameClass>(); \
	}
