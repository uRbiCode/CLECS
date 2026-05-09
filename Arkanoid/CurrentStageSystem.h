#pragma once

struct SystemContext;

/* Manages currently ongoing run stage.
 * Sends StageEndEvent when stage is completed, and resets stage when player loses health.
 * Also speeds up ball with each collision.
 */
namespace CurrentStageSystem
{
	void Initialize(const SystemContext& Context);
}