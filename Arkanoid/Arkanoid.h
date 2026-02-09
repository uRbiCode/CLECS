#pragma once
#include "Game.h"

struct RendererInitializationData;

class Arkanoid : public Game
{
public:
	bool Initialize(WorldInitializationData& Data) override;

	void Shutdown() override {}

	RendererInitializationData GetRendererConfig() const override;
};

CLECS_DEFINE_GAME_ENTRY(Arkanoid)