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
	
	if (Json.contains("paddleWidthIncrease"))
	{
		UpgradeData.PaddleWidthIncrease = Json["paddleWidthIncrease"].get<float>();
	}

	return UpgradeData;
}