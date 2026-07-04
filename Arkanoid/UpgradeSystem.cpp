#include "UpgradeSystem.h"
#include "SystemContext.h"
#include "EventBus.h"
#include "ClickableComponent.h"
#include "SDLUtils.h"
#include "ShapeComponents.h"
#include "RenderConstants.h"
#include "TextComponent.h"
#include <numeric>
#include <random>
#include <algorithm>
#include <cassert>
#include <vector>

#include "UpgradeLoader.h"
#include <SDL3/SDL_log.h>
#include "AddEntitiesCommand.h"
#include "CommandRunner.h"
#include <Query.h>
#include "HealthComponent.h"
#include <RenderComponents.h>
#include "TransitionComponents.h"
#include <PositionComponent.h>
#include "GlobalConstants.h"

namespace
{
	constexpr int UpgradesToPresent = 3;

	std::string BuildUpgradeButtonText(const UpgradeDescriptionComponent& Upgrade)
	{
		return Upgrade.Name + "\n\n" + Upgrade.Description;
	}

	std::vector<std::pair<Entity, UpgradeDescriptionComponent>> SampleAvailableUpgradeEntities(SystemContext& Context)
	{
		std::vector<std::pair<Entity, UpgradeDescriptionComponent>> AvailableUpgradeEntitites;
		const Query<WritesList<>, ReadsList<AvailableUpgradeComponent, UpgradeDescriptionComponent>, ExcludeList<>> AvailableUpgradesQuery(Context.QueryContext);
		AvailableUpgradeEntitites.reserve(AvailableUpgradesQuery.Size());
		AvailableUpgradesQuery.ForEach([&](Entity Entity, const AvailableUpgradeComponent& AvailableUpgrade, const UpgradeDescriptionComponent& UpgradeDescription)
		{
			AvailableUpgradeEntitites.push_back({ Entity, UpgradeDescription });
		});

		std::vector<std::pair<Entity, UpgradeDescriptionComponent>> SampledEntities;
		SampledEntities.reserve(UpgradesToPresent);
		std::ranges::sample(AvailableUpgradeEntitites, std::back_inserter(SampledEntities), UpgradesToPresent, std::mt19937{ std::random_device{}() });
		return SampledEntities;
	}

	void PresentUpgradesChoice(SystemContext& Context, std::vector<std::pair<Entity, UpgradeDescriptionComponent>>&& Upgrades)
	{
		if (Upgrades.empty())
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "No available upgrades to present to the player.");
			return;
		}

		const auto LogicalPresentation = SDLUtils::GetRendererLogicalPresentation(&Context.Renderer);
		const Vector2D<float> ButtonSize = { LogicalPresentation.X * 0.25f, LogicalPresentation.Y * 0.4f };
		const float UpgradeCount = static_cast<float>(Upgrades.size());
		const float TotalWidth = LogicalPresentation.X * 0.8f;
		const float TotalButtonWidth = ButtonSize.X * UpgradeCount;
		const float AvailableSpacing = TotalWidth - TotalButtonWidth;
		const float Spacing = AvailableSpacing / (UpgradeCount + 1.f);
		const float StartX = LogicalPresentation.X * 0.1f;

		AddComponentsCommand<PositionComponent, RectComponent, UIRenderComponent, TextComponent, ClickableComponent> AddUpgradeButtonsCommand(Upgrades.size());
		for (size_t i = 0; i < Upgrades.size(); ++i)
		{
			const float XPosition = StartX + Spacing * (static_cast<float>(i) + 1.f) + ButtonSize.X * (static_cast<float>(i) + 0.5f);
			AddUpgradeButtonsCommand.WithEntry(Upgrades[i].first,
				PositionComponent{ {XPosition, LogicalPresentation.Y * 0.55f} },
				RectComponent{ SDL_FRect{ -ButtonSize.X * 0.5f, -ButtonSize.Y * 0.5f, ButtonSize.X, ButtonSize.Y } },
				UIRenderComponent{ SDL_FColor{ 1.f, 1.f, 1.f, 1.f } },
				TextComponent{ BuildUpgradeButtonText(Upgrades[i].second), GlobalConstants::FontFilePath, 12},
				ClickableComponent{ ClickableTag::Upgrade });
		}
	}
}

void UpgradeSystem::SpawnUpgradeEntities(SystemContext& Context)
{
	const size_t UpgradeCount = UpgradeLoader::GetUpgradeCount();
	AddEntitiesCommand<AvailableUpgradeComponent, UpgradeDescriptionComponent> AddAvailableUpgradesCommand(UpgradeCount);
	for (size_t i = 0; i < UpgradeCount; ++i)
	{
		AddAvailableUpgradesCommand.WithEntry(AvailableUpgradeComponent{}, UpgradeDescriptionComponent{});
	}

	Context.Commands.Submit(std::move(AddAvailableUpgradesCommand));
}

void UpgradeSystem::InitializeUpgrades(SystemContext& Context)
{
	const Query<WritesList<UpgradeDescriptionComponent>, ReadsList<AvailableUpgradeComponent>, ExcludeList<>> UpgradesQuery(Context.QueryContext);
	AddComponentsCommand<PaddleWidthMultiplierUpgradeComponent> PaddleWidthComponentsCommand(0);
	AddComponentsCommand<BallSpeedMultiplierUpgradeComponent> BallSpeedComponentsCommand(0);
	AddComponentsCommand<BallSizeMultiplierUpgradeComponent> BallSizeComponentsCommand(0);
	AddComponentsCommand<HealUpgradeComponent> HealComponentsCommand(0);

	const std::vector<UpgradeDefinition> AllUpgrades = UpgradeLoader::LoadUpgradeDefinitions();
	size_t UpgradeIndex = 0;

	UpgradesQuery.ForEach([&](Entity Entity, UpgradeDescriptionComponent& UpgradeDescription, const AvailableUpgradeComponent& AvailableUpgrade)
	{
		const UpgradeDefinition& UpgradeToSpawn = AllUpgrades[UpgradeIndex];

		UpgradeDescription.Name = UpgradeToSpawn.UpgradeDescription.Name;
		UpgradeDescription.Description = UpgradeToSpawn.UpgradeDescription.Description;

		if (UpgradeToSpawn.PaddleWidthMultiplierUpgrade.has_value())
		{
			PaddleWidthComponentsCommand.WithEntry(Entity, UpgradeToSpawn.PaddleWidthMultiplierUpgrade.value());
		}
		if (UpgradeToSpawn.BallSpeedMultiplierUpgrade.has_value())
		{
			BallSpeedComponentsCommand.WithEntry(Entity, UpgradeToSpawn.BallSpeedMultiplierUpgrade.value());
		}
		if (UpgradeToSpawn.BallSizeMultiplierUpgrade.has_value())
		{
			BallSizeComponentsCommand.WithEntry(Entity, UpgradeToSpawn.BallSizeMultiplierUpgrade.value());
		}
		if (UpgradeToSpawn.HealUpgrade.has_value())
		{
			HealComponentsCommand.WithEntry(Entity, UpgradeToSpawn.HealUpgrade.value());
		}
		++UpgradeIndex;
	});

	if (PaddleWidthComponentsCommand.AccessEntries().size() > 0)
	{
		Context.Commands.Submit(std::move(PaddleWidthComponentsCommand));
	}
	if (BallSpeedComponentsCommand.AccessEntries().size() > 0)
	{
		Context.Commands.Submit(std::move(BallSpeedComponentsCommand));
	}
	if (BallSizeComponentsCommand.AccessEntries().size() > 0)
	{
		Context.Commands.Submit(std::move(BallSizeComponentsCommand));
	}
	if (HealComponentsCommand.AccessEntries().size() > 0)
	{
		Context.Commands.Submit(std::move(HealComponentsCommand));
	}
}

void UpgradeSystem::UpdateOwnedUpgrades(SystemContext& Context, float DeltaTime)
{
	const Query<WritesList<>, ReadsList<ClickableUsedComponent, AvailableUpgradeComponent>, ExcludeList<>> UpgradeSelectionQuery(Context.QueryContext);
	if (UpgradeSelectionQuery.Size() < 1)
		return;

	RemoveComponentsCommand<AvailableUpgradeComponent> RemoveAvailableUpgradeCommand(UpgradeSelectionQuery.Size());
	UpgradeSelectionQuery.ForEach([&](Entity Entity, const ClickableUsedComponent& ClickableUsed, const AvailableUpgradeComponent& AvailableUpgrade)
	{
		RemoveAvailableUpgradeCommand.WithEntry(Entity);
	});

	Context.Commands.Submit(std::move(RemoveAvailableUpgradeCommand));

	const Query<WritesList<>, ReadsList<UIRenderComponent, UpgradeDescriptionComponent>, ExcludeList<>> VisibleUpgrades(Context.QueryContext);
	RemoveComponentsCommand<PositionComponent, RectComponent, UIRenderComponent, TextComponent, ClickableComponent> RemoveUpgradeComponentsCommand(VisibleUpgrades.Size());
	VisibleUpgrades.ForEach([&](Entity Entity, const UIRenderComponent& UIRender, const UpgradeDescriptionComponent& UpgradeDescription)
	{
		RemoveUpgradeComponentsCommand.WithEntry(Entity);
	});
	Context.Commands.Submit(std::move(RemoveUpgradeComponentsCommand));
}

void UpgradeSystem::UpdateImmediateUpgrades(SystemContext& Context, float DeltaTime)
{
	int TotalHealAmount = 0;

	const Query<WritesList<>, ReadsList<ClickableUsedComponent, HealUpgradeComponent>, ExcludeList<>> HealQuery(Context.QueryContext);
	HealQuery.ForEach([&](Entity Entity, const ClickableUsedComponent& ClickableUsed, const HealUpgradeComponent& HealUpgrade)
	{
		TotalHealAmount += HealUpgrade.Heal;
	});

	const Query<WritesList<HealthComponent>, ReadsList<BackgroundRenderComponent>, ExcludeList<>> PersistentHealthQuery(Context.QueryContext);
	PersistentHealthQuery.ForEach([&](Entity Entity, HealthComponent& Health, const BackgroundRenderComponent& BackgroundRender)
	{
		Health.CurrentHealth += TotalHealAmount;
	});
}

void UpgradeSystem::ResetUpgrades(SystemContext& Context, float DeltaTime)
{
	const Query<WritesList<>, ReadsList<SummaryTransitionComponent>, ExcludeList<>> SummaryTransitionQuery(Context.QueryContext);
	if (SummaryTransitionQuery.Size() < 1)
		return;

	const Query<WritesList<>, ReadsList<UpgradeDescriptionComponent>, ExcludeList<AvailableUpgradeComponent>> OwnedUpgradesQuery(Context.QueryContext);
	if (OwnedUpgradesQuery.Size() < 1)
		return;

	AddComponentsCommand<AvailableUpgradeComponent> AddAvailableUpgradeCommand(OwnedUpgradesQuery.Size());
	OwnedUpgradesQuery.ForEach([&](Entity Entity, const UpgradeDescriptionComponent& UpgradeDescription)
	{
		AddAvailableUpgradeCommand.WithEntry(Entity, AvailableUpgradeComponent{});
	});

	Context.Commands.Submit(std::move(AddAvailableUpgradeCommand));
}

void UpgradeSystem::UpdateUpgradesChoice(SystemContext& Context, float DeltaTime)
{
	const Query<WritesList<>, ReadsList<UpgradesTransitionComponent>, ExcludeList<>> UpgradesTransitionQuery(Context.QueryContext);
	UpgradesTransitionQuery.ForEach([&](Entity Entity, const UpgradesTransitionComponent& UpgradesTransition)
	{
		PresentUpgradesChoice(Context, SampleAvailableUpgradeEntities(Context));
	});
}