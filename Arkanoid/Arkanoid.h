#pragma once
#include "Game.h"

class EntityAdmin;

/* Entry point to the game.
 * Responsible for registering systems and providing initial renderer configuration.
 * Here one has to determine order of execution for systems, which may prove crucial for the synchronous framework.
 */
class Arkanoid : public Game
{
public:
	bool Initialize(WorldInitializationData& Data) override;

	void Shutdown() override {}

	RendererInitializationData GetRendererConfig() const override;

private:
	void AddGameStateComponent(EntityAdmin& Admin) const;
};

CLECS_DEFINE_GAME_ENTRY(Arkanoid)