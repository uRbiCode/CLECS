#pragma once
#include <memory>

class WorldInitializationData;
struct RendererInitializationData;

/* Game is the main entry point for CLECS games.
 * One has to inherit from this class and use CLECS_DEFINE_GAME_ENTRY macro to define the entry point of the game.
 */
class Game
{
public:
	virtual ~Game() = default;

	virtual bool Initialize(WorldInitializationData& Data) = 0;

	virtual void Shutdown() = 0;

	virtual RendererInitializationData GetRendererConfig() const = 0;
};

std::unique_ptr<Game> CreateGame();

#define CLECS_DEFINE_GAME_ENTRY(GameClass) \
	std::unique_ptr<Game> CreateGame() \
	{ \
		return std::make_unique<GameClass>(); \
	}
