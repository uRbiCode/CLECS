#pragma once
#include <vector>

struct SystemContext;
struct WallData;
struct BrickData;
struct TriggerData;
struct PlayerData;
struct BallData;

namespace RunUtils
{
	int GetCurrentStageNumber(SystemContext& Context);

	void SpawnWalls(SystemContext& Context, std::vector<WallData>&& Data);
	void SpawnBricks(SystemContext& Context, std::vector<BrickData>&& Data);
	void SpawnTrigger(SystemContext& Context, TriggerData&& Data);
	void SpawnPlayer(SystemContext& Context, PlayerData&& Data);
	void SpawnBall(SystemContext& Context, BallData&& Data);
}