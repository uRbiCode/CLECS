#pragma once
#include "MathTypes.h"
#include <SDL3/SDL.h>
#include <vector>
#include <string>

// Stores data for creating a texture for an entity.
struct TextureData
{
    std::string Path;
    SDL_FRect SourceRect{ 0.f, 0.f, 0.f, 0.f };
};

// Stores position and size data for an entity.
struct PositionSizeData
{
    Vector2D<float> Position;
    Vector2D<float> Size;
};

// Stores data used for creating player entity (the paddle we control).
struct PlayerData
{
    PositionSizeData PositionSize;
	TextureData TextureData;
};

// Stores data used for creating the ball entity.
struct BallData
{
    Vector2D<float> Position;
    float Radius = 10.f;
    Vector2D<float> Velocity;
    TextureData TextureData;
};

// Stores data used for creating wall entity.
struct WallData
{
    Vector2D<float> Position;
    Vector2D<float> Size;
	SDL_FColor Color{ 1.f, 1.f, 1.f, 1.f };
	TextureData TextureData;
    int Health = 1;
};

// Stores data used for creating trigger entity.
struct TriggerData
{
	PositionSizeData PositionSize;
};

// Stores data used for creating brick entity.
struct BrickData
{
    PositionSizeData PositionSize;
    SDL_FColor Color = {1.f, 1.f, 1.f, 1.f};
    int Health = 1;
    TextureData TextureData;
};

/* Combines all previous structs into description of a stage.
 * This is the data loaded from JSON files and used for creating entities for a stage.
 */
struct StageData
{
	PlayerData PlayerData;
	BallData BallData;
    TriggerData Trigger;
	std::vector<WallData> Walls;
    std::vector<BrickData> Bricks;
};

/* Stores data for the current stage.
 * Used by CurrentStageSystem for reseting the stage.
 * Filled by RunControllerSystem when a new stage is loaded.
 */
struct StageDataComponent
{
    StageData CurrentStageData;
};