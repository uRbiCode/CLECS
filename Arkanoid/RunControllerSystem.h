#pragma once

struct SystemContext;

/* Top-level controller for Run. Manages internal state machine of the ongoing run.
 * Determines when and with what resolution end the run.
 */
namespace RunControllerSystem
{
	void Initialize(const SystemContext& Context);
}