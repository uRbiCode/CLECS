#pragma once
#include "RunStateComponent.h"

struct ChangeRunStateEvent
{
	RunState NewState = RunState::Invalid;
};