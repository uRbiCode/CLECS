#pragma once
#include "System.h"

struct SystemContext;
struct StageData;
enum class RunState : uint8_t;

class RunControllerSystem : public System
{
public:
	void Initialize(const SystemContext& Context) const;
	
private:
	void BeginRun(const SystemContext& Context) const;
	void CleanupRun(const SystemContext& Context) const;

	void AddRunStateComponent(const SystemContext& Context) const;
	void AddStageDataComponent(const SystemContext& Context) const;
	void RemoveRunStateComponent(const SystemContext& Context) const;
	void RemoveStageDataComponent(const SystemContext& Context) const;

	void HandleStageCleared(const SystemContext& Context) const;

	void HandleRunVictory(const SystemContext& Context) const;
	void HandleRunDefeat(const SystemContext& Context) const;

	//Returns whether advancment was succesful or was it the last stage already
	bool AdvanceToNextStage(const SystemContext& Context, int CurrentStageId) const;

	void SpawnStageEntities(const SystemContext& Context, const StageData& StageData) const;
	void SetStageData(const SystemContext& Context, const StageData& StageData) const;
	void CleanupCurrentStage(const SystemContext& Context) const;

	bool AreUpgradesAvailable(const SystemContext& Context) const;
	void ChangeRunState(const SystemContext& Context, RunState NewState) const;
	void TryStartNextStage(const SystemContext& Context) const;
};