#pragma once
#include <string>

struct SystemContext;

namespace TransitionUtils
{
	void TravelToMainMenu(SystemContext& Context);
	void TravelToTutorial(SystemContext& Context);
	void TravelToRun(SystemContext& Context);
	void TravelToSummary(SystemContext& Context, const std::string& Message);
	void TravelToUpgrades(SystemContext& Context);
	void CleanupRunStage(SystemContext& Context);
}