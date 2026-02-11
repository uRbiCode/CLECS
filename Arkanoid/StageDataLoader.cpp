#include "StageDataLoader.h"
#include <fstream>
#include <stdexcept>

std::vector<StageData> StageDataLoader::LoadFromFile(const std::string& FilePath)
{
	std::ifstream File(FilePath);
	if (!File.is_open())
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "StageDataLoader::LoadFromFile -> Failed to open stage data file: %s", FilePath.c_str());
		return {};
	}

	Json JsonData;
	File >> JsonData;

	std::vector<StageData> Stages;
	for (const auto& StageJson : JsonData["stages"])
	{
		Stages.push_back(ParseStageData(StageJson));
	}

	return Stages;
}

Vector2D<float> StageDataLoader::ParseVector2D(const Json& Json)
{
	return Vector2D<float>{ Json["x"].get<float>(), Json["y"].get<float>() };
}

TextureData StageDataLoader::ParseTextureData(const Json& Json)
{
	TextureData Data;
	Data.Path = Json.value("path", "");
	
	if (Json.contains("sourceRect"))
	{
		const auto& Rect = Json["sourceRect"];
		Data.SourceRect = SDL_FRect{
			Rect["x"].get<float>(),
			Rect["y"].get<float>(),
			Rect["w"].get<float>(),
			Rect["h"].get<float>()
		};
	}
	
	return Data;
}

WallData StageDataLoader::ParseWallData(const Json& Json)
{
	WallData Data;
	Data.Position = ParseVector2D(Json["position"]);
	Data.Size = ParseVector2D(Json["size"]);
	
	if (Json.contains("texture"))
	{
		Data.TextureData = ParseTextureData(Json["texture"]);
	}
	
	return Data;
}

BrickData StageDataLoader::ParseBrickData(const Json& Json)
{
	BrickData Data;
	Data.PositionSize.Position = ParseVector2D(Json["position"]);
	Data.PositionSize.Size = ParseVector2D(Json["size"]);
	Data.Health = Json["health"].get<int>();
	
	if (Json.contains("texture"))
	{
		Data.TextureData = ParseTextureData(Json["texture"]);
	}
	
	return Data;
}

TriggerData StageDataLoader::ParseTriggerData(const Json& Json)
{
	TriggerData Data;
	Data.PositionSize.Position = ParseVector2D(Json["position"]);
	Data.PositionSize.Size = ParseVector2D(Json["size"]);
	Data.Health = Json.value("health", 1);
	return Data;
}

PlayerData StageDataLoader::ParsePlayerData(const Json& Json)
{
	PlayerData Data;
	Data.PositionSize.Position = ParseVector2D(Json["position"]);
	Data.PositionSize.Size = ParseVector2D(Json["size"]);
	
	if (Json.contains("texture"))
	{
		Data.TextureData = ParseTextureData(Json["texture"]);
	}
	
	return Data;
}

BallData StageDataLoader::ParseBallData(const Json& Json)
{
	BallData Data;
	Data.Position = ParseVector2D(Json["position"]);
	Data.Velocity = ParseVector2D(Json["velocity"]);
	Data.Radius = Json["radius"].get<float>();
	
	if (Json.contains("texture"))
	{
		Data.TextureData = ParseTextureData(Json["texture"]);
	}
	
	return Data;
}

StageData StageDataLoader::ParseStageData(const Json& Json)
{
	StageData Stage;
	Stage.PlayerData = ParsePlayerData(Json["player"]);
	Stage.BallData = ParseBallData(Json["ball"]);
	Stage.Trigger = ParseTriggerData(Json["trigger"]);
	
	for (const auto& WallJson : Json["walls"])
	{
		Stage.Walls.push_back(ParseWallData(WallJson));
	}
	
	for (const auto& BrickJson : Json["bricks"])
	{
		Stage.Bricks.push_back(ParseBrickData(BrickJson));
	}
	
	return Stage;
}