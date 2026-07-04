#pragma once
#include "UpgradeComponents.h"
#include <nlohmann/json.hpp>
#include <vector>

struct UpgradeDefinition
{
	UpgradeDescriptionComponent UpgradeDescription;
	std::optional<PaddleWidthMultiplierUpgradeComponent> PaddleWidthMultiplierUpgrade;
	std::optional<BallSpeedMultiplierUpgradeComponent> BallSpeedMultiplierUpgrade;
	std::optional<BallSizeMultiplierUpgradeComponent> BallSizeMultiplierUpgrade;
	std::optional<HealUpgradeComponent> HealUpgrade;
};

using Json = nlohmann::json;

// Wrapper for JSON parser of UpgradeData.
class UpgradeLoader
{
public:
	static std::vector<UpgradeDefinition> LoadUpgradeDefinitions();
	static size_t GetUpgradeCount();

private:
	static UpgradeDefinition ParseUpgradeDefinition(const Json& Json);
	static std::optional<PaddleWidthMultiplierUpgradeComponent> ParsePaddleWidthMultiplierUpgrade(const Json& Json);
	static std::optional<BallSpeedMultiplierUpgradeComponent> ParseBallSpeedMultiplierUpgrade(const Json& Json);
	static std::optional<BallSizeMultiplierUpgradeComponent> ParseBallSizeMultiplierUpgrade(const Json& Json);
	static std::optional<HealUpgradeComponent> ParseHealUpgrade(const Json& Json);
};