#pragma once
#include "System.h"

struct SystemContext;
struct StageData;
enum class RunState : uint8_t;

/* Top-level controller for Run. Manages internal state machnies between stages and upgrades.
 * Determines when and with what resolution end the run.
 */
class RunControllerSystem : public System
{
public:
	void Initialize(const SystemContext& Context) const;
	
private:
	// Run GameState management
	void BeginRun(const SystemContext& Context) const;
	void ChangeRunState(const SystemContext& Context, RunState NewState) const;
	void CleanupRun(const SystemContext& Context) const;

	// Run GameState initialization and cleanup
	void AddRunStateComponent(const SystemContext& Context) const;
	void AddStageDataComponent(const SystemContext& Context) const;
	void RemoveRunStateComponent(const SystemContext& Context) const;
	void RemoveStageDataComponent(const SystemContext& Context) const;

	// Reponses to stage ending
	void HandleStageCleared(const SystemContext& Context) const;
	void HandleRunVictory(const SystemContext& Context) const;
	void HandleRunDefeat(const SystemContext& Context) const;
	void TryStartNextStage(const SystemContext& Context) const;

	// Stage management
	void SpawnStageEntities(const SystemContext& Context, const StageData& StageData) const;
	void SetStageData(const SystemContext& Context, const StageData& StageData) const;
	void CleanupCurrentStage(const SystemContext& Context) const;

	// Part of stage management. Returns whether advancment was succesful or was it the last stage already
	bool AdvanceToNextStage(const SystemContext& Context, int CurrentStageId) const;

	// Upgrades management
	bool AreUpgradesAvailable(const SystemContext& Context) const;
};