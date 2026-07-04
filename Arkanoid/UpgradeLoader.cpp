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

size_t UpgradeLoader::GetUpgradeCount()
{
	std::ifstream File(UpgradesFilePath);
	if (!File.is_open())
	{
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UpgradeLoader::GetUpgradeCount -> Upgrades file not found: %s", UpgradesFilePath);
		return 0;
	}

	Json JsonData;
	File >> JsonData;

	if (JsonData.contains("upgrades") && JsonData["upgrades"].is_array())
		return JsonData["upgrades"].size();

	return 0;
}

UpgradeDefinition UpgradeLoader::ParseUpgradeDefinition(const Json& Json)
{
	UpgradeDefinition Definition;
	Definition.UpgradeDescription.Name = Json.value("name", "");
	Definition.UpgradeDescription.Description = Json.value("description", "");

	Definition.PaddleWidthMultiplierUpgrade = ParsePaddleWidthMultiplierUpgrade(Json["upgrade"]);
	Definition.BallSpeedMultiplierUpgrade = ParseBallSpeedMultiplierUpgrade(Json["upgrade"]);
	Definition.BallSizeMultiplierUpgrade = ParseBallSizeMultiplierUpgrade(Json["upgrade"]);
	Definition.HealUpgrade = ParseHealUpgrade(Json["upgrade"]);
	return Definition;
}

std::optional<PaddleWidthMultiplierUpgradeComponent> UpgradeLoader::ParsePaddleWidthMultiplierUpgrade(const Json& Json)
{
	if (!Json.contains("paddleWidthMultiplier"))
		return std::nullopt;

	PaddleWidthMultiplierUpgradeComponent Component;
	Component.Multiplier = Json["paddleWidthMultiplier"].get<float>();
	return Component;
}

std::optional<BallSpeedMultiplierUpgradeComponent> UpgradeLoader::ParseBallSpeedMultiplierUpgrade(const Json& Json)
{
	if (!Json.contains("ballSpeedMultiplier"))
		return std::nullopt;

	BallSpeedMultiplierUpgradeComponent Component;
	Component.Multiplier = Json["ballSpeedMultiplier"].get<float>();
	return Component;
}

std::optional<BallSizeMultiplierUpgradeComponent> UpgradeLoader::ParseBallSizeMultiplierUpgrade(const Json& Json)
{
	if (!Json.contains("ballSizeMultiplier"))
		return std::nullopt;

	BallSizeMultiplierUpgradeComponent Component;
	Component.Multiplier = Json["ballSizeMultiplier"].get<float>();
	return Component;
}

std::optional<HealUpgradeComponent> UpgradeLoader::ParseHealUpgrade(const Json& Json)
{
	if (!Json.contains("heal"))
		return std::nullopt;

	HealUpgradeComponent Component;
	Component.Heal = Json["heal"].get<int>();
	return Component;
}