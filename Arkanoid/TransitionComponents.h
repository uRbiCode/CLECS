#pragma once
#include <string>

// Carries over a message to be displayed in the summary transition screen after a run is completed.
struct SummaryTransitionComponent
{
	std::string Message;
};

// Signals that the game should transition to the upgrades screen after a stage is cleared.
struct UpgradesTransitionComponent
{
};