#pragma once

struct SystemContext;
struct PlayerData;

namespace UpgradeUtils
{
	PlayerData GetModifiedPlayerData(const SystemContext& Context, const PlayerData& OriginalData);
};