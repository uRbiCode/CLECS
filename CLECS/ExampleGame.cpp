#include "Game.h"

/* Macro for game programmers to define their game entry point.
 * Usage in your own .cpp file:
 *
 * #include "GameEntry.h"
 *
 * class MyGame : public Game { ... };
 *
 * CLECS_DEFINE_GAME_ENTRY(MyGame)
 */

class ExampleGame : public Game
{
public:
	bool Initialize(World& GameWorld) override
	{
		// TODO: Add your systems here
		// GameWorld.AddSystem<YourSystem>();

		// TODO: Create initial entities
		// EntityManager* EntityMgr = GameWorld.GetEntityManager();
		// EntityMgr->CreateEntity();

		return true;
	}

	void Shutdown(World& GameWorld) override
	{
		// TODO: Clean up game-specific resources
	}

	const char* GetWindowTitle() const override
	{
		return "CLECS Example Game";
	}

	int GetWindowWidth() const override
	{
		return 1280;
	}

	int GetWindowHeight() const override
	{
		return 720;
	}
};

CLECS_DEFINE_GAME_ENTRY(ExampleGame)
