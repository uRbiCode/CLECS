#pragma once

struct SystemContext;

namespace TransitionSystem
{
	void UpdateDynamicTransitions(SystemContext& Context, float DeltaTime);
	void UpdateClickableTransitions(SystemContext& Context, float DeltaTime);
	void UpdateTransitionsFromRun(SystemContext& Context, float DeltaTime);
}