#pragma once
#include <vector>
#include <string>
#include "MathTypes.h"
#include <SDL3/SDL.h>

struct TextureData
{
    std::string Path;
    SDL_FRect SourceRect{ 0.f, 0.f, 0.f, 0.f };
};

struct PositionSizeData
{
    Vector2D<float> Position;
    Vector2D<float> Size;
};

struct PlayerData
{
    PositionSizeData PositionSize;
	TextureData TextureData;
};

struct BallData
{
    Vector2D<float> Position;
    float Radius = 10.f;
    Vector2D<float> Velocity;
    TextureData TextureData;
};

struct WallData
{
    Vector2D<float> Position;
    Vector2D<float> Size;
	SDL_FColor Color{ 1.f, 1.f, 1.f, 1.f };
	TextureData TextureData;
    int Health = 1;
};

struct TriggerData
{
	PositionSizeData PositionSize;
};

struct BrickData
{
    PositionSizeData PositionSize;
    SDL_FColor Color = {1.f, 1.f, 1.f, 1.f};
    int Health = 1;
    TextureData TextureData;
};

struct StageData
{
	PlayerData PlayerData;
	BallData BallData;
    TriggerData Trigger;
	std::vector<WallData> Walls;
    std::vector<BrickData> Bricks;
};

struct StageDataComponent
{
    StageData CurrentStageData;
};