#pragma once
#include "Game.h"

struct RendererInitializationData;
class EntityAdmin;

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