#pragma once

struct SystemContext;

/* Responsible for controlling the Tutorial game state.
 * Works similarly to SummaryControllerSystem, but manages a different state and differs slightly within initialization and cleanup logic.
 */
namespace TutorialControllerSystem
{
	void Initialize(const SystemContext& Context);
}