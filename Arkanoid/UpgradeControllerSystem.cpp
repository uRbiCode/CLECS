#include "UpgradeControllerSystem.h"
#include "SystemContext.h"
#include "EventBus.h"
#include "ChangeRunStateEvent.h"
#include "UpgradeComponent.h"
#include "EntityAdmin.h"
#include "UpgradeEvent.h"
#include "ClickableComponent.h"
#include "SDLUtils.h"
#include "TransformComponent.h"
#include "ShapeComponents.h"
#include "RenderConstants.h"
#include "RenderComponent.h"
#include "Constants.h"
#include "TextComponent.h"
#include "ClickableUsedEvent.h"
#include "UpgradeUtils.h"
#include <numeric>
#include <random>
#include <algorithm>
#include <cassert>
#include <vector>

namespace
{
	constexpr int MaxUpgradesToPresent = 3;
	constexpr const char* UpgradeTitleText = "Pick an Upgrade";

	std::string BuildUpgradeButtonText(const UpgradeDefinition& Upgrade)
	{
		return Upgrade.Name + "\n\n" + Upgrade.Description;
	}

	std::vector<UpgradeDefinition> SampleUpgrades(const SystemContext& Context, const std::vector<UpgradeDefinition>& AvailableUpgrades)
	{
		if (AvailableUpgrades.size() <= MaxUpgradesToPresent)
			return AvailableUpgrades;

		auto SampledUpgrades = AvailableUpgrades;

		std::random_device rd;
		std::mt19937 g(rd());

		std::shuffle(SampledUpgrades.begin(), SampledUpgrades.end(), g);
		SampledUpgrades.resize(MaxUpgradesToPresent);
		return SampledUpgrades;
	}

	void CleanupUpgradeSelection(const SystemContext& Context)
	{
		Context.EntityAdmin.GetGroup<UpgradeComponent>().ForEach([&Context](const Entity& Entity, const UpgradeComponent& UpgradeComp)
		{
			Context.EntityAdmin.DestroyEntity(Entity);
		});

		const void* const Id = reinterpret_cast<const void*>(&UpgradeControllerSystem::Initialize);
		Context.EventBus.Unsubscribe<ClickableUsedEvent>(Id);
	}

	void AddUpgradeTitle(const SystemContext& Context)
	{
		const auto LogicalPresentation = SDLUtils::GetRendererLogicalPresentation(&Context.Renderer);
		const Vector2D<float> RectSize = { LogicalPresentation.X * 1.f, LogicalPresentation.Y * 0.1f };

		auto& Admin = Context.EntityAdmin;
		const auto UpgradeTitleEntity = Admin.CreateEntity();
		Admin.AddComponent<TransformComponent>(UpgradeTitleEntity, Vector2D<float>{ LogicalPresentation.X * 0.5f, LogicalPresentation.Y * 0.25f });
		Admin.AddComponent<RectComponent>(UpgradeTitleEntity, SDL_FRect{ -RectSize.X * 0.5f, -RectSize.Y * 0.5f, RectSize.X, RectSize.Y });
		Admin.AddComponent<ColorComponent>(UpgradeTitleEntity, SDL_FColor{ 0.f, 0.f, 0.f, 0.f });
		Admin.AddComponent<TextComponent>(UpgradeTitleEntity, UpgradeTitleText, Constants::FontFilePath, 60);
		Admin.AddComponent<RenderComponent>(UpgradeTitleEntity, RenderConstants::UILayer);

		// Add mock UpgradeComponent to ease cleanup.
		Admin.AddComponent<UpgradeComponent>(UpgradeTitleEntity);
	}

	void PresentUpgradesToPlayer(const SystemContext& Context, std::vector<UpgradeDefinition>&& Upgrades)
	{
		auto& Admin = Context.EntityAdmin;
		const auto LogicalPresentation = SDLUtils::GetRendererLogicalPresentation(&Context.Renderer);

		const Vector2D<float> ButtonSize = { LogicalPresentation.X * 0.25f, LogicalPresentation.Y * 0.4f };

		const float UpgradeCount = static_cast<float>(Upgrades.size());
		const float TotalWidth = LogicalPresentation.X * 0.8f;
		const float TotalButtonWidth = ButtonSize.X * UpgradeCount;
		const float AvailableSpacing = TotalWidth - TotalButtonWidth;
		const float Spacing = AvailableSpacing / (UpgradeCount + 1.f);
		const float StartX = LogicalPresentation.X * 0.1f;

		for (size_t i = 0; i < Upgrades.size(); ++i)
		{
			const float XPosition = StartX + Spacing * (static_cast<float>(i) + 1.f) + ButtonSize.X * (static_cast<float>(i) + 0.5f);

			const auto UpgradeButtonEntity = Admin.CreateEntity();
			Admin.AddComponent<ClickableComponent>(UpgradeButtonEntity, ClickableTag::Upgrade);
			Admin.AddComponent<TransformComponent>(UpgradeButtonEntity, Vector2D<float>{ XPosition, LogicalPresentation.Y * 0.55f });
			Admin.AddComponent<RectComponent>(UpgradeButtonEntity, SDL_FRect{ -ButtonSize.X * 0.5f, -ButtonSize.Y * 0.5f, ButtonSize.X, ButtonSize.Y });
			Admin.AddComponent<ShapeFillComponent>(UpgradeButtonEntity, false);
			Admin.AddComponent<RenderComponent>(UpgradeButtonEntity, RenderConstants::UILayer);
			Admin.AddComponent<TextComponent>(UpgradeButtonEntity, std::move(BuildUpgradeButtonText(Upgrades[i])), Constants::FontFilePath, 12);
			Admin.AddComponent<UpgradeComponent>(UpgradeButtonEntity, Upgrades[i]);
		}
	}

	void OnUpgradeSelected(const SystemContext& Context, const Entity& Entity)
	{
		if (!Context.EntityAdmin.HasComponent<UpgradeComponent>(Entity))
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UpgradeControllerSystem::OnUpgradeSelected -> Clicked entity does not have UpgradeComponent");
			Context.EventBus.Notify(Context, UpgradeSelectedEvent{});
			CleanupUpgradeSelection(Context);
			return;
		}

		const auto& SelectedUpgradeDefinition = Context.EntityAdmin.GetComponent<UpgradeComponent>(Entity).UpgradeDefinition;
		const auto OwnedUpgradesGroup = Context.EntityAdmin.GetGroup<OwnedUpgradesComponent>();
		assert(OwnedUpgradesGroup.Size() == 1 && "Expected exactly one OwnedUpgradesComponent in the world");
		if (OwnedUpgradesGroup.Empty())
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "UpgradeControllerSystem::OnUpgradeSelected -> No OwnedUpgradesComponent found");
			Context.EventBus.Notify(Context, UpgradeSelectedEvent{});
			CleanupUpgradeSelection(Context);
			return;
		}

		std::erase(Context.EntityAdmin.AccessComponent<AvailableUpgradesComponent>(Context.EntityAdmin.GetGroup<AvailableUpgradesComponent>()[0]).AvailableUpgrades, SelectedUpgradeDefinition);
		Context.EntityAdmin.AccessComponent<OwnedUpgradesComponent>(OwnedUpgradesGroup[0]).OwnedUpgrades.push_back(SelectedUpgradeDefinition.Upgrade);
		Context.EventBus.Notify(Context, UpgradeSelectedEvent{});
		UpgradeUtils::ApplyHealUpgrade(Context, SelectedUpgradeDefinition.Upgrade);
		CleanupUpgradeSelection(Context);
	}

	void InitializeUpgradeSelection(const SystemContext& Context)
	{
		const auto AvailableUpgradesGroup = Context.EntityAdmin.GetGroup<AvailableUpgradesComponent>();
		assert(AvailableUpgradesGroup.Size() == 1 && "Expected exactly one AvailableUpgradesComponent in the world");
		if (AvailableUpgradesGroup.Empty())
		{
			SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "UpgradeControllerSystem::InitializeUpgradeSelection -> No AvailableUpgradesComponent found. Skipping upgrade selection.");
			Context.EventBus.Notify(Context, UpgradeSelectedEvent{});
			return;
		}

		const auto& AvailableUpgrades = Context.EntityAdmin.AccessComponent<AvailableUpgradesComponent>(AvailableUpgradesGroup[0]).AvailableUpgrades;
		if (AvailableUpgrades.empty())
		{
			SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "UpgradeControllerSystem::InitializeUpgradeSelection -> No available upgrades found. Skipping upgrade selection.");
			Context.EventBus.Notify(Context, UpgradeSelectedEvent{});
			return;
		}

		AddUpgradeTitle(Context);
		PresentUpgradesToPlayer(Context, SampleUpgrades(Context, AvailableUpgrades));

		const void* const Id = reinterpret_cast<const void*>(&UpgradeControllerSystem::Initialize);
		Context.EventBus.Subscribe<ClickableUsedEvent>(Id, [](const SystemContext& Context, const ClickableUsedEvent& Event)
		{
			if (Event.UsedClickableTag != ClickableTag::Upgrade)
				return;

			OnUpgradeSelected(Context, Event.ClickedEntity);
		});
	}
}

void UpgradeControllerSystem::Initialize(const SystemContext& Context)
{
	const void* const Id = reinterpret_cast<const void*>(&Initialize);

	Context.EventBus.Subscribe<ChangeRunStateEvent>(Id, [](const SystemContext& Context, const ChangeRunStateEvent& Event)
	{
		if (Event.NewState != RunState::UpgradeSelection)
			return;

		InitializeUpgradeSelection(Context);
	});
}