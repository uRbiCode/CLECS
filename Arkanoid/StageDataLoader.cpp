#include "StageDataLoader.h"
#include <fstream>
#include <SDL3/SDL_log.h>

namespace Constants
{
	constexpr const char* StagesDirectory = "../Assets/Stages/";
	namespace Labels
	{
		constexpr const char* SourceRect = "sourceRect";
		constexpr const char* Position = "position";
		constexpr const char* Size = "size";
		constexpr const char* Stage = "stage";
		constexpr const char* Texture = "texture";
		constexpr const char* Walls = "walls";
		constexpr const char* Bricks = "bricks";
		constexpr const char* Health = "health";
		constexpr const char* Velocity = "velocity";
		constexpr const char* Radius = "radius";
		constexpr const char* Player = "player";
		constexpr const char* Ball = "ball";
		constexpr const char* Trigger = "trigger";
		constexpr const char* Path = "path";
	}
}

StageData StageDataLoader::LoadStageDataByNumber(int StageNumber)
{
	const std::string StagesDirectoryString = Constants::StagesDirectory;
	const std::string FilePath = StagesDirectoryString + Constants::Labels::Stage + std::to_string(StageNumber) + ".json";
	
	std::ifstream File(FilePath);
	if (!File.is_open())
	{
		SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "StageDataLoader::LoadStageByNumber -> Stage file not found: %s", FilePath.c_str());
		return StageData{};
	}

	Json JsonData;
	File >> JsonData;
	return ParseStageData(JsonData);
}

bool StageDataLoader::IsStageDataAvailable(int StageNumber)
{
	const std::string StagesDirectoryString = Constants::StagesDirectory;
	const std::string FilePath = StagesDirectoryString + Constants::Labels::Stage + std::to_string(StageNumber) + ".json";
	
	std::ifstream File(FilePath);
	return File.is_open();
}

Vector2D<float> StageDataLoader::ParseVector2D(const Json& Json)
{
	return Vector2D<float>{ Json["x"].get<float>(), Json["y"].get<float>() };
}

TextureData StageDataLoader::ParseTextureData(const Json& Json)
{
	TextureData Data;
	Data.Path = Json.value(Constants::Labels::Path, "");
	
	if (!Json.contains(Constants::Labels::SourceRect))
		return Data;

	const auto& Rect = Json[Constants::Labels::SourceRect];
	Data.SourceRect = SDL_FRect{
		Rect["x"].get<float>(),
		Rect["y"].get<float>(),
		Rect["w"].get<float>(),
		Rect["h"].get<float>()
	};
	
	return Data;
}

WallData StageDataLoader::ParseWallData(const Json& Json)
{
	WallData Data;
	Data.PositionSize.Position = ParseVector2D(Json[Constants::Labels::Position]);
	Data.PositionSize.Size = ParseVector2D(Json[Constants::Labels::Size]);
	return Data;
}

BrickData StageDataLoader::ParseBrickData(const Json& Json)
{
	BrickData Data;
	Data.PositionSize.Position = ParseVector2D(Json[Constants::Labels::Position]);
	Data.PositionSize.Size = ParseVector2D(Json[Constants::Labels::Size]);
	Data.Health = Json[Constants::Labels::Health].get<int>();
	
	if (!Json.contains(Constants::Labels::Texture))
		return Data;

	Data.TextureData = ParseTextureData(Json[Constants::Labels::Texture]);
	
	return Data;
}

TriggerData StageDataLoader::ParseTriggerData(const Json& Json)
{
	TriggerData Data;
	Data.PositionSize.Position = ParseVector2D(Json[Constants::Labels::Position]);
	Data.PositionSize.Size = ParseVector2D(Json[Constants::Labels::Size]);
	return Data;
}

PlayerData StageDataLoader::ParsePlayerData(const Json& Json)
{
	PlayerData Data;
	Data.PositionSize.Position = ParseVector2D(Json[Constants::Labels::Position]);
	Data.PositionSize.Size = ParseVector2D(Json[Constants::Labels::Size]);
	
	if (!Json.contains(Constants::Labels::Texture))
		return Data;

	Data.TextureData = ParseTextureData(Json[Constants::Labels::Texture]);
	
	return Data;
}

BallData StageDataLoader::ParseBallData(const Json& Json)
{
	BallData Data;
	Data.Position = ParseVector2D(Json[Constants::Labels::Position]);
	Data.Velocity = ParseVector2D(Json[Constants::Labels::Velocity]);
	Data.Radius = Json[Constants::Labels::Radius].get<float>();
	
	if (!Json.contains(Constants::Labels::Texture))
		return Data;

	Data.TextureData = ParseTextureData(Json[Constants::Labels::Texture]);
	
	return Data;
}

StageData StageDataLoader::ParseStageData(const Json& Json)
{
	StageData Stage;
	Stage.PlayerData = ParsePlayerData(Json[Constants::Labels::Player]);
	Stage.BallData = ParseBallData(Json[Constants::Labels::Ball]);
	Stage.Trigger = ParseTriggerData(Json[Constants::Labels::Trigger]);
	
	if (Json.contains(Constants::Labels::Walls) && Json[Constants::Labels::Walls].is_array())
	{
		Stage.Walls.reserve(Json[Constants::Labels::Walls].size());
		for (const auto& WallJson : Json[Constants::Labels::Walls])
		{
			Stage.Walls.push_back(ParseWallData(WallJson));
		}
	}
	
	if (Json.contains(Constants::Labels::Bricks) && Json[Constants::Labels::Bricks].is_array())
	{
		Stage.Bricks.reserve(Json[Constants::Labels::Bricks].size());
		for (const auto& BrickJson : Json[Constants::Labels::Bricks])
		{
			Stage.Bricks.push_back(ParseBrickData(BrickJson));
		}
	}
	
	return Stage;
}