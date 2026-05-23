#include "Arkanoid.h"
#include "ArkanoidModule.h"
#include "ModulesInitializationData.h"
#include "EntityAdmin.h"

namespace
{
	constexpr int ScreenWidth = 640;
	constexpr int ScreenHeight = 480;
}

RendererInitializationData Arkanoid::GetRendererConfig() const
{
	RendererInitializationData RendererConfig;
	RendererConfig.WindowTitle = "Arkanoid";
	RendererConfig.WindowWidth = ScreenWidth;
	RendererConfig.WindowHeight = ScreenHeight;
	return RendererConfig;
}

void Arkanoid::RegisterGameModules(ModulesInitializationData& Data) const
{
	Data.RegisterModule<ArkanoidModule>();
}