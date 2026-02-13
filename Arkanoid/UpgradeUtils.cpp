#include "UpgradeUtils.h"
#include "SystemContext.h"
#include "EntityAdmin.h"
#include "StageDataComponent.h"
#include "UpgradeComponent.h"
#include "RunStateComponent.h"
#include "HealthChangedEvent.h"
#include "EventBus.h"
#include "HealthUtils.h"

PlayerData UpgradeUtils::GetModifiedPlayerData(const SystemContext& Context, const PlayerData& OriginalData)
{
	auto OwnedUpgradesGroup = Context.EntityAdmin.GetGroup<OwnedUpgradesComponent>();
	if (OwnedUpgradesGroup.Empty())
		return OriginalData;

	auto ModifiedData = OriginalData;
	OwnedUpgradesGroup.ForEach([&ModifiedData](const Entity& Entity, const OwnedUpgradesComponent& OwnedUpgrades)
	{
		for (const auto& Upgrade : OwnedUpgrades.OwnedUpgrades)
		{
			if (!Upgrade.PaddleWidthMultiplier.has_value())
				continue;

			ModifiedData.PositionSize.Size.X *= Upgrade.PaddleWidthMultiplier.value();
		}
	});

	return ModifiedData;
}

BallData UpgradeUtils::GetModifiedBallData(const SystemContext& Context, const BallData& OriginalData)
{
	auto OwnedUpgradesGroup = Context.EntityAdmin.GetGroup<OwnedUpgradesComponent>();
	if (OwnedUpgradesGroup.Empty())
		return OriginalData;

	auto ModifiedData = OriginalData;
	OwnedUpgradesGroup.ForEach([&ModifiedData](const Entity& Entity, const OwnedUpgradesComponent& OwnedUpgrades)
	{
		for (const auto& Upgrade : OwnedUpgrades.OwnedUpgrades)
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
	});

	return ModifiedData;
}

void UpgradeUtils::ApplyHealUpgrade(const SystemContext& Context, const Upgrade& Upgrade)
{
	auto RunstateGroup = Context.EntityAdmin.GetGroup<RunStateComponent>();
	if (RunstateGroup.Empty())
		return;

	RunstateGroup.ForEach([&Context, &Upgrade](const Entity& Entity, const RunStateComponent& RunState)
	{
		if (!Upgrade.Heal.has_value())
			return;

		HealthUtils::ApplyHealthChange(Context, Entity, Upgrade.Heal.value());
	});
}