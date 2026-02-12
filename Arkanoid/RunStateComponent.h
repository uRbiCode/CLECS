#pragma once
#include <cstdint>

enum class RunState : uint8_t
{
	Invalid = 0,
	Stage,
	UpgradeSelection
};

struct RunStateComponent
{
	RunState State = RunState::Invalid;
	int CurrentStage = 0;
};