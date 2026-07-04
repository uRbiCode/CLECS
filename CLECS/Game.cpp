#include "Game.h"
#include "ModulesInitializationData.h"
#include "CoreModule.h"
#include "AudioModule.h"

void Game::InitializeModules(ModulesInitializationData& Data) const
{
	RegisterGameModules(Data);
	RegisterCoreModules(Data);
}

void Game::RegisterCoreModules(ModulesInitializationData& Data) const
{
	Data.RegisterModule<CoreModule>();
	Data.RegisterModule<AudioModule>();
}