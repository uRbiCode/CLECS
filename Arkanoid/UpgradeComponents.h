#pragma once
#include <string>

// Marks entities that are available for upgrade selection. So ones not owned by player.
struct AvailableUpgradeComponent
{
};

// Contains visual represantation for an upgrade.
struct UpgradeDescriptionComponent
{
	std::string Name;
	std::string Description;
};

// Stores the multiplier for the paddle width when this upgrade is chosen.
struct PaddleWidthMultiplierUpgradeComponent
{
	float Multiplier = 1.f;
};

// Stores the multiplier for the ball speed when this upgrade is chosen.
struct BallSpeedMultiplierUpgradeComponent
{
	float Multiplier = 1.f;
};

// Stores the multiplier for the ball size when this upgrade is chosen.
struct BallSizeMultiplierUpgradeComponent
{
	float Multiplier = 1.f;
};

// Stores the amount of health to be added to the player when this upgrade is chosen.
struct HealUpgradeComponent
{
	int Heal = 0;
};