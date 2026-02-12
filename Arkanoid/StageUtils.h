#pragma once

struct StageData;
struct SystemContext;

struct WallData;
struct TriggerData;
struct BrickData;
struct PlayerData;
struct BallData;

namespace StageUtils
{
	StageData GetCurrentStageData(const SystemContext& Context);
	int GetCurrentPlayerHealth(const SystemContext& Context);

	void AddWall(const SystemContext& Context, const WallData& WallData);
	void AddTrigger(const SystemContext& Context, const TriggerData& TriggerData);
	void AddBrick(const SystemContext& Context, const BrickData& BrickData);
	void AddPlayer(const SystemContext& Context, const PlayerData& StageData);
	void AddBall(const SystemContext& Context, const BallData& BallData);
}