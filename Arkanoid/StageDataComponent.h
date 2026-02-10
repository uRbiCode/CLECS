#pragma once
#include "MathTypes.h"
#include "CollisionComponent.h"
#include <vector>
#include <SDL3/SDL_pixels.h>

struct BrickData
{
	Vector2D<float> Position{};
	Vector2D<float> Size{};
	SDL_FColor Color{ 1.f, 1.f, 1.f, 1.f };
	int Health = 1;
};

struct WallData
{
	Vector2D<float> Position{};
	Vector2D<float> Size{};
};

struct TriggerData
{
	Vector2D<float> Position{};
	Vector2D<float> Size{};
};

struct StageData
{
	Vector2D<float> PlayerSpawnPosition;
	Vector2D<float> BallSpawnPosition;
	Vector2D<float> BallInitialVelocity;

	TriggerData Trigger;
	std::vector<WallData> Walls;
	std::vector<BrickData> Bricks;
};

struct StageDataComponent
{
	std::vector<StageData> Stages;
};