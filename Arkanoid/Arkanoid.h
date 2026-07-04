#pragma once
#include "Game.h"

/* Entry point to the game.
 * Responsible for registering Modules and providing initial renderer configuration.
 */
class Arkanoid : public Game
{
public:
	void Shutdown() const override {}

	RendererInitializationData GetRendererConfig() const override;

protected:
	void RegisterGameModules(ModulesInitializationData& Data) const override;
};