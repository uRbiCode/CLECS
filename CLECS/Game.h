#pragma once
#include "RendererInitializationData.h"

struct ModulesInitializationData;

/* Game is the main entry point for CLECS games.
 * You must inherit from this class to define the entry point of the game.
 */
class Game
{
public:
	virtual ~Game() = default;

	void InitializeModules(ModulesInitializationData& Data) const;

	virtual void Shutdown() const = 0;

	virtual RendererInitializationData GetRendererConfig() const = 0;

protected:
	virtual void RegisterGameModules(ModulesInitializationData& Data) const = 0;

private:
	void RegisterCoreModules(ModulesInitializationData& Data) const;
};