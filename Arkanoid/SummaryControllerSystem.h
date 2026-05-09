#pragma once

struct SystemContext;

/* Responsible for controlling the Summary game state.
 * Summary is either Victory or Defeat. From this system's perspective, they differ only by displayed text.
 */
namespace SummaryControllerSystem
{
	void Initialize(const SystemContext& Context);
}