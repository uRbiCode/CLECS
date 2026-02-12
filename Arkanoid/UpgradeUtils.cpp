#include "UpgradeUtils.h"
#include "SystemContext.h"
#include "EntityAdmin.h"
#include "StageDataComponent.h"
#include "UpgradeComponent.h"

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
				return;

			ModifiedData.PositionSize.Size.X *= Upgrade.PaddleWidthMultiplier.value();
		}
	});

	return ModifiedData;
}