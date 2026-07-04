#pragma once
#include <nlohmann/json.hpp>
#include "MathTypes.h"
#include "SDL3/SDL_rect.h"

// Stores data for creating a texture for an entity.
struct TextureData
{
    std::string Path;
    SDL_FRect SourceRect{ 0.f, 0.f, 0.f, 0.f };
};

// Stores position and size data for an entity.
struct PositionSizeData
{
    Vector2D<float> Position = { 0.f, 0.f };
    Vector2D<float> Size = { 0.f, 0.f };
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
    Vector2D<float> Position = { 0.f, 0.f };
    float Radius = 10.f;
    Vector2D<float> Velocity = { 0.f, 0.f };
    TextureData TextureData;
};

// Stores data used for creating wall entity.
struct WallData
{
	PositionSizeData PositionSize;
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

using Json = nlohmann::json;

// Wrapper for JSON parser of StageData.
class StageDataLoader
{
public:
	static StageData LoadStageDataByNumber(int StageNumber);
	static bool IsStageDataAvailable(int StageNumber);

private:
	static StageData ParseStageData(const Json& Json);
	static Vector2D<float> ParseVector2D(const Json& Json);
	static TextureData ParseTextureData(const Json& Json);
	static WallData ParseWallData(const Json& Json);
	static BrickData ParseBrickData(const Json& Json);
	static TriggerData ParseTriggerData(const Json& Json);
	static PlayerData ParsePlayerData(const Json& Json);
	static BallData ParseBallData(const Json& Json);
};