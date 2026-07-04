#pragma once

struct SystemContext;

namespace TransitionUtils
{
	void TravelToMainMenu(SystemContext& Context);
	void TravelToTutorial(SystemContext& Context);
	void TravelToRun(SystemContext& Context);
}