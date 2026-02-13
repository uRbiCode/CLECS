#pragma once
#include "RunStateComponent.h"

/* Sent whenever state of the ongoing run changes.
 * Used to trigger Upgrade selection and stage UI update.
 */
struct ChangeRunStateEvent
{
	RunState NewState = RunState::Invalid;
};