#pragma once
#include "Game.h"

struct RendererInitializationData;

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
	bool Initialize(WorldInitializationData& Data) override;

	void Shutdown() override {}

	RendererInitializationData GetRendererConfig() const override;
};

CLECS_DEFINE_GAME_ENTRY(ExampleGame)