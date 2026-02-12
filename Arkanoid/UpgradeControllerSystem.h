#pragma once
#include "System.h"
#include <vector>

struct UpgradeDefinition;
struct Entity;

// Manages presenting and awarding player with upgrades after each (but last) stage victory
class UpgradeControllerSystem : public System
{
public:
	void Initialize(const SystemContext& Context) const override;

private:
	void InitializeUpgradeSelection(const SystemContext& Context) const;
	std::vector<UpgradeDefinition> SampleUpgrades(const SystemContext& Context, const std::vector<UpgradeDefinition>& AvailableUpgrades) const;
	void AddUpgradeTitle(const SystemContext& Context) const;
	void PresentUpgradesToPlayer(const SystemContext& Context, const std::vector<UpgradeDefinition>& Upgrades) const;
	void OnUpgradeSelected(const SystemContext& Context, const Entity& Entity) const;
	void CleanupUpgradeSelection(const SystemContext& Context) const;
};