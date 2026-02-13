#pragma once

struct SystemContext;
struct PlayerData;
struct BallData;
struct Upgrade;

// Utility functions related to upgrades.
namespace UpgradeUtils
{
	PlayerData GetModifiedPlayerData(const SystemContext& Context, const PlayerData& OriginalData);
	BallData GetModifiedBallData(const SystemContext& Context, const BallData& OriginalData);
	void ApplyHealUpgrade(const SystemContext& Context, const Upgrade& Upgrade);
	float GetBallVelocityModifier(const SystemContext& Context);
};