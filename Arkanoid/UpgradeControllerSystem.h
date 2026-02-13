#pragma once
#include "System.h"
#include <vector>

struct UpgradeDefinition;
struct Entity;

/* Responsible for presenting and awarding player with upgrades after each (but last) stage victory.
 * Also manages available and owned upgades components so that player won't ever take the same upgrade twice.
 */
class UpgradeControllerSystem : public System
{
public:
	void Initialize(const SystemContext& Context) const override;

private:
	// Event responses.
	void OnUpgradeSelected(const SystemContext& Context, const Entity& Entity) const;

	// UI management.
	void InitializeUpgradeSelection(const SystemContext& Context) const;
	void AddUpgradeTitle(const SystemContext& Context) const;
	void PresentUpgradesToPlayer(const SystemContext& Context, const std::vector<UpgradeDefinition>& Upgrades) const;
	void CleanupUpgradeSelection(const SystemContext& Context) const;

	// Upgrade management.
	std::vector<UpgradeDefinition> SampleUpgrades(const SystemContext& Context, const std::vector<UpgradeDefinition>& AvailableUpgrades) const;
};