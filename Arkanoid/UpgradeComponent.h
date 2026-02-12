#pragma once
#include <optional>
#include <vector>
#include <string>

// Represents modifier which player can choose after completing a stage
struct Upgrade
{
	std::optional<float> PaddleWidthIncrease = std::nullopt;

	bool operator==(const Upgrade&) const = default;
};

// Modifier with its description and weight for sampling
struct UpgradeDefinition
{
	std::string Name = {};
	std::string Description = {};
	Upgrade Upgrade = {};

	bool operator==(const UpgradeDefinition&) const = default;
};

// Stores possible upgrades to sample from
struct AvailableUpgradesComponent
{
	std::vector<UpgradeDefinition> AvailableUpgrades;
};

// Stores upgrades owned by the player
struct OwnedUpgradesComponent
{
	std::vector<Upgrade> OwnedUpgrades;
};

// Stores associated upgrade 
struct UpgradeComponent
{
	UpgradeDefinition UpgradeDefinition;
};