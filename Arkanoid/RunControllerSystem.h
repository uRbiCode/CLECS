#pragma once
#include "System.h"

struct SystemContext;
struct StageData;
struct HealthChangedEvent;

class RunControllerSystem : public System
{
public:
	void Initialize(const SystemContext& Context) const;
	
private:
	void OnHealthChanged(const SystemContext& Context, const HealthChangedEvent& Event) const;

	void HandleStageCleared(const SystemContext& Context) const;

	void HandleRunVictory(const SystemContext& Context) const;
	void HandleRunDefeat(const SystemContext& Context) const;

	bool AreAllBricksDestroyed(const SystemContext& Context) const;
	bool HasPlayerLost(const SystemContext& Context) const;

	//Returns whether advancment was succesful or was it the last stage already
	bool AdvanceToNextStage(const SystemContext& Context, int NextStageId) const;
	void CleanupCurrentStage(const SystemContext& Context) const;
	void SpawnStageEntities(const SystemContext& Context, const StageData& StageData) const;
};