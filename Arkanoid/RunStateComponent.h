#pragma once
#include <cstdint>

enum class RunState : uint8_t
{
	Invalid = 0,
	PlayerPrepare,
	Stage,
	UpgradeSelection
};

/* Stores state of the current run.
 * Used by RunControllerSystem to manage run sequence.
 * Used by StageInfoSystem for displaying stage info on the UI.
 */
struct RunStateComponent
{
	RunState State = RunState::Invalid;
	int CurrentStage = 0;
};