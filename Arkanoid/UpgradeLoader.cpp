#include "UpgradeLoader.h"
#include <fstream>
#include <SDL3/SDL_log.h>

namespace
{
	constexpr const char* UpgradesFilePath = "../Assets/Upgrades/upgrades.json";
}

std::vector<UpgradeDefinition> UpgradeLoader::LoadUpgradeDefinitions()
{
	std::ifstream File(UpgradesFilePath);
	if (!File.is_open())
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UpgradeLoader::LoadUpgradeDefinitions -> Upgrades file not found: %s", UpgradesFilePath);
		return {};
	}

	Json JsonData;
	File >> JsonData;

	std::vector<UpgradeDefinition> Upgrades;
	if (JsonData.contains("upgrades") && JsonData["upgrades"].is_array())
	{
		Upgrades.reserve(JsonData["upgrades"].size());
		for (const auto& UpgradeJson : JsonData["upgrades"])
		{
			Upgrades.push_back(ParseUpgradeDefinition(UpgradeJson));
		}
	}

	return Upgrades;
}

UpgradeDefinition UpgradeLoader::ParseUpgradeDefinition(const Json& Json)
{
	UpgradeDefinition Definition;
	Definition.Name = Json.value("name", "");
	Definition.Description = Json.value("description", "");
	Definition.Upgrade = ParseUpgrade(Json["upgrade"]);
	return Definition;
}

Upgrade UpgradeLoader::ParseUpgrade(const Json& Json)
{
	Upgrade UpgradeData;
	
	if (Json.contains("paddleWidthMultiplier"))
	{
		UpgradeData.PaddleWidthMultiplier = Json["paddleWidthMultiplier"].get<float>();
	}

	if (Json.contains("ballSpeedMultiplier"))
	{
		UpgradeData.BallSpeedMultiplier = Json["ballSpeedMultiplier"].get<float>();
	}
	
	if (Json.contains("ballSizeMultiplier"))
	{
		UpgradeData.BallSizeMultiplier = Json["ballSizeMultiplier"].get<float>();
	}

	if (Json.contains("heal"))
	{
		UpgradeData.Heal = Json["heal"].get<int>();
	}

	return UpgradeData;
}