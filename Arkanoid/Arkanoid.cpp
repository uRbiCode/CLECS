#include "Arkanoid.h"
#include "ArkanoidModule.h"
#include "ModulesInitializationData.h"
#include "ArkanoidMovementModule.h"
#include "ArkanoidInputModule.h"
#include "ArkanoidCollisionModule.h"
#include "ArkanoidHealthModule.h"

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
	Data.RegisterModule<ArkanoidInputModule>();
	Data.RegisterModule<ArkanoidCollisionModule>();
	Data.RegisterModule<ArkanoidMovementModule>();
	Data.RegisterModule<ArkanoidHealthModule>();
}