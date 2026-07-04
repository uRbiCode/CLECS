#pragma once

struct SystemContext;

/* Responsible for presenting and awarding player with upgrades after each (but last) stage victory.
 * Also manages available and owned upgrade components so that player won't ever take the same upgrade twice.
 */
namespace UpgradeSystem
{
	void SpawnUpgradeEntities(SystemContext& Context);
	void InitializeUpgrades(SystemContext& Context);
	void UpdateOwnedUpgrades(SystemContext& Context, float DeltaTime);
	void UpdateImmediateUpgrades(SystemContext& Context, float DeltaTime);
	void ResetUpgrades(SystemContext& Context, float DeltaTime);
}