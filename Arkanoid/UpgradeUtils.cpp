#include "UpgradeUtils.h"
#include "SystemContext.h"
#include "EntityAdmin.h"
#include "StageDataComponent.h"
#include "UpgradeComponent.h"
#include "RunStateComponent.h"
#include "EventBus.h"
#include "HealthUtils.h"
#include <cassert>

PlayerData UpgradeUtils::GetModifiedPlayerData(const SystemContext& Context, const PlayerData& OriginalData)
{
	const auto OwnedUpgradesGroup = Context.EntityAdmin.GetGroup<OwnedUpgradesComponent>();
	assert(OwnedUpgradesGroup.Size() == 1 && "Expected exactly one OwnedUpgradesComponent in the world");
	if (OwnedUpgradesGroup.Empty())
		return OriginalData;

	auto ModifiedData = OriginalData;
	const auto& OwnedUpgrades = Context.EntityAdmin.AccessComponent<OwnedUpgradesComponent>(OwnedUpgradesGroup[0]).OwnedUpgrades;
	for (const auto& Upgrade : OwnedUpgrades)
	{
		if (!Upgrade.PaddleWidthMultiplier.has_value())
			continue;

		ModifiedData.PositionSize.Size.X *= Upgrade.PaddleWidthMultiplier.value();
	}

	return ModifiedData;
}

BallData UpgradeUtils::GetModifiedBallData(const SystemContext& Context, const BallData& OriginalData)
{
	const auto OwnedUpgradesGroup = Context.EntityAdmin.GetGroup<OwnedUpgradesComponent>();
	assert(OwnedUpgradesGroup.Size() == 1 && "Expected exactly one OwnedUpgradesComponent in the world");
	if (OwnedUpgradesGroup.Empty())
		return OriginalData;

	auto ModifiedData = OriginalData;
	const auto& OwnedUpgrades = Context.EntityAdmin.AccessComponent<OwnedUpgradesComponent>(OwnedUpgradesGroup[0]).OwnedUpgrades;
	for (const auto& Upgrade : OwnedUpgrades)
	{
		if (Upgrade.BallSpeedMultiplier.has_value())
		{
			ModifiedData.Velocity.X *= Upgrade.BallSpeedMultiplier.value();
			ModifiedData.Velocity.Y *= Upgrade.BallSpeedMultiplier.value();
		}

		if (Upgrade.BallSizeMultiplier.has_value())
		{
			ModifiedData.Radius *= Upgrade.BallSizeMultiplier.value();
		}
	}

	return ModifiedData;
}

void UpgradeUtils::ApplyHealUpgrade(const SystemContext& Context, const Upgrade& Upgrade)
{
	const auto RunstateGroup = Context.EntityAdmin.GetGroup<RunStateComponent>();
	assert(RunstateGroup.Size() == 1 && "Expected exactly one RunStateComponent in the world");
	if (RunstateGroup.Empty())
		return;

	HealthUtils::ApplyHealthChange(Context, RunstateGroup[0], Upgrade.Heal.value_or(0));
}

float UpgradeUtils::GetBallVelocityModifier(const SystemContext& Context)
{
	float Modifier = 1.f;
	const auto OwnedUpgradesGroup = Context.EntityAdmin.GetGroup<OwnedUpgradesComponent>();
	assert(OwnedUpgradesGroup.Size() == 1 && "Expected exactly one OwnedUpgradesComponent in the world");
	if (OwnedUpgradesGroup.Empty())
		return Modifier;

	const auto& OwnedUpgrades = Context.EntityAdmin.AccessComponent<OwnedUpgradesComponent>(OwnedUpgradesGroup[0]).OwnedUpgrades;
	for (const auto& Upgrade : OwnedUpgrades)
	{
		if (!Upgrade.BallSpeedMultiplier.has_value())
			continue;

		Modifier *= Upgrade.BallSpeedMultiplier.value();
	};

	return Modifier;
}