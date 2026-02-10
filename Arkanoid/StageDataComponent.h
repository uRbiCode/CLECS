#pragma once
#include <vector>
#include <string>
#include "MathTypes.h"
#include <SDL3/SDL.h>

struct BallData
{
    Vector2D<float> Position;
    Vector2D<float> Velocity;
};

struct WallData
{
    Vector2D<float> Position;
    Vector2D<float> Size;
	SDL_FColor Color{ 1.f, 1.f, 1.f, 1.f };
    std::string TexturePath;
	SDL_FRect TextureSourceRect{ 0.f, 0.f, 0.f, 0.f };
    int Health = 1;
};

struct TriggerData
{
    Vector2D<float> Position;
    Vector2D<float> Size;
    int Health = 3;
};

struct BrickData
{
    Vector2D<float> Position;
    Vector2D<float> Size;
    SDL_FColor Color = {1.f, 1.f, 1.f, 1.f};
    int Health = 1;
    std::string TexturePath;
	SDL_FRect TextureSourceRect{ 0.f, 0.f, 0.f, 0.f };
};

struct StageData
{
    Vector2D<float> PlayerSpawnPosition;

	BallData BallData;
    TriggerData Trigger;
	std::vector<WallData> Walls;
    std::vector<BrickData> Bricks;
};

struct StageDataComponent
{
    std::vector<StageData> Stages;
};