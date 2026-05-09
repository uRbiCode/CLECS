#pragma once

struct SystemContext;

/* Responsible for presenting and awarding player with upgrades after each (but last) stage victory.
 * Also manages available and owned upgrade components so that player won't ever take the same upgrade twice.
 */
namespace UpgradeControllerSystem
{
	void Initialize(const SystemContext& Context);
}