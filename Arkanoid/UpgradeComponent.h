#pragma once
#include <optional>
#include <vector>
#include <string>

/* Represents modifier which player can choose after completing a stage
 * A single upgrade may or may not use all modifiers.
 */ 
struct Upgrade
{
	std::optional<float> PaddleWidthMultiplier = std::nullopt;
	std::optional<float> BallSpeedMultiplier = std::nullopt;
	std::optional<float> BallSizeMultiplier = std::nullopt;
	std::optional<int> Heal = std::nullopt;

	bool operator==(const Upgrade&) const = default;
};

/* Modifier with its description for displaying purposes
 * Upgrades data is stored within a JSON file.
 */ 
struct UpgradeDefinition
{
	std::string Name = {};
	std::string Description = {};
	Upgrade Upgrade = {};

	bool operator==(const UpgradeDefinition&) const = default;
};

/* Stores possible upgrades to sample from.
 * Component is filled once (by RunControllerSystem) at the beginnig of the run. Then each chosen upgrade is removed from the list.
 * UpgradeControllerSystem controls sampling from and maintaining valid collection of upgrades.
 */
struct AvailableUpgradesComponent
{
	std::vector<UpgradeDefinition> AvailableUpgrades;
};

/* Stores upgrades owned by the player.
 * Each time player chooses an upgrade, it is added to the list.
 * Each time stage is started or reset, the modifiers are applied. 
 * Each time HealthUpgrade is chosen, it is applied immediately once and for all.
 */
struct OwnedUpgradesComponent
{
	std::vector<Upgrade> OwnedUpgrades;
};

/* Stores associated upgrade with entities that present them to the player during upgrade choosing state.
 * When player chooses an upgrade, the UpgradeDefinition is used to apply the upgrade and remove it from AvailableUpgradesComponent.
 */
struct UpgradeComponent
{
	UpgradeDefinition UpgradeDefinition;
};