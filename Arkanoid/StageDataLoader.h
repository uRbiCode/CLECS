#pragma once
#include <nlohmann/json.hpp>
#include "StageDataComponent.h"
#include <string>
#include <vector>
#include <optional>

using Json = nlohmann::json;

// Wrapper for JSON parser of StageData.
class StageDataLoader
{
public:
	static std::optional<StageData> LoadStageDataByNumber(int StageNumber);
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