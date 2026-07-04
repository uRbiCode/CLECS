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

namespace
{
	constexpr int MaxUpgradesToPresent = 3;
	constexpr const char* UpgradeTitleText = "Pick an Upgrade";

	//std::string BuildUpgradeButtonText(const UpgradeDefinition& Upgrade)
	//{
	//	return Upgrade.Name + "\n\n" + Upgrade.Description;
	//}

	//std::vector<UpgradeDefinition> SampleUpgrades(const SystemContext& Context, const std::vector<UpgradeDefinition>& AvailableUpgrades)
	//{
	//	if (AvailableUpgrades.size() <= MaxUpgradesToPresent)
	//		return AvailableUpgrades;

	//	auto SampledUpgrades = AvailableUpgrades;

	//	std::random_device rd;
	//	std::mt19937 g(rd());

	//	std::shuffle(SampledUpgrades.begin(), SampledUpgrades.end(), g);
	//	SampledUpgrades.resize(MaxUpgradesToPresent);
	//	return SampledUpgrades;
	//}

	//void AddUpgradeTitle(const SystemContext& Context)
	//{
	//	const auto LogicalPresentation = SDLUtils::GetRendererLogicalPresentation(&Context.Renderer);
	//	const Vector2D<float> RectSize = { LogicalPresentation.X * 1.f, LogicalPresentation.Y * 0.1f };

	//	auto& Admin = Context.EntityAdmin;
	//	const auto UpgradeTitleEntity = Admin.CreateEntity();
	//	Admin.AddComponent<TransformComponent>(UpgradeTitleEntity, Vector2D<float>{ LogicalPresentation.X * 0.5f, LogicalPresentation.Y * 0.25f });
	//	Admin.AddComponent<RectComponent>(UpgradeTitleEntity, SDL_FRect{ -RectSize.X * 0.5f, -RectSize.Y * 0.5f, RectSize.X, RectSize.Y });
	//	Admin.AddComponent<ColorComponent>(UpgradeTitleEntity, SDL_FColor{ 0.f, 0.f, 0.f, 0.f });
	//	Admin.AddComponent<TextComponent>(UpgradeTitleEntity, UpgradeTitleText, Constants::FontFilePath, 60);
	//	Admin.AddComponent<RenderComponent>(UpgradeTitleEntity, RenderConstants::UILayer);

	//	// Add mock UpgradeComponent to ease cleanup.
	//	Admin.AddComponent<UpgradeComponent>(UpgradeTitleEntity);
	//}

	//void PresentUpgradesToPlayer(const SystemContext& Context, std::vector<UpgradeDefinition>&& Upgrades)
	//{
	//	auto& Admin = Context.EntityAdmin;
	//	const auto LogicalPresentation = SDLUtils::GetRendererLogicalPresentation(&Context.Renderer);

	//	const Vector2D<float> ButtonSize = { LogicalPresentation.X * 0.25f, LogicalPresentation.Y * 0.4f };

	//	const float UpgradeCount = static_cast<float>(Upgrades.size());
	//	const float TotalWidth = LogicalPresentation.X * 0.8f;
	//	const float TotalButtonWidth = ButtonSize.X * UpgradeCount;
	//	const float AvailableSpacing = TotalWidth - TotalButtonWidth;
	//	const float Spacing = AvailableSpacing / (UpgradeCount + 1.f);
	//	const float StartX = LogicalPresentation.X * 0.1f;

	//	for (size_t i = 0; i < Upgrades.size(); ++i)
	//	{
	//		const float XPosition = StartX + Spacing * (static_cast<float>(i) + 1.f) + ButtonSize.X * (static_cast<float>(i) + 0.5f);

	//		const auto UpgradeButtonEntity = Admin.CreateEntity();
	//		Admin.AddComponent<ClickableComponent>(UpgradeButtonEntity, ClickableTag::Upgrade);
	//		Admin.AddComponent<TransformComponent>(UpgradeButtonEntity, Vector2D<float>{ XPosition, LogicalPresentation.Y * 0.55f });
	//		Admin.AddComponent<RectComponent>(UpgradeButtonEntity, SDL_FRect{ -ButtonSize.X * 0.5f, -ButtonSize.Y * 0.5f, ButtonSize.X, ButtonSize.Y });
	//		Admin.AddComponent<ShapeFillComponent>(UpgradeButtonEntity, false);
	//		Admin.AddComponent<RenderComponent>(UpgradeButtonEntity, RenderConstants::UILayer);
	//		Admin.AddComponent<TextComponent>(UpgradeButtonEntity, std::move(BuildUpgradeButtonText(Upgrades[i])), Constants::FontFilePath, 12);
	//		Admin.AddComponent<UpgradeComponent>(UpgradeButtonEntity, Upgrades[i]);
	//	}
	//}
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
}

void UpgradeSystem::UpdateImmediateUpgrades(SystemContext& Context, float DeltaTime)
{
	const Query<WritesList<>, ReadsList<ClickableUsedComponent, HealUpgradeComponent>, ExcludeList<>> HealQuery(Context.QueryContext);
	if (HealQuery.Size() < 1)
		return;

	int TotalHealAmount = 0;
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

}