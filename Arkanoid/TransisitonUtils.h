#pragma once

struct SystemContext;

namespace TransitionUtils
{
	void InitializeMainMenu(SystemContext& Context);
	void PlayBackgroundMusic(SystemContext& Context);
	void InitializeTutorial(SystemContext& Context);
}