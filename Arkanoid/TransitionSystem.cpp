#include "TransitionSystem.h"
#include "SystemContext.h"
#include "Query.h"
#include "ClickableComponent.h"
#include "ComponentUtils.h"
#include "TransitionUtils.h"
#include "RenderComponents.h"
#include "RunUtils.h"
#include "StageDataLoader.h"
#include "CollisionUtils.h"
#include "HealthComponent.h"
#include "TransitionComponents.h"
#include "PositionComponent.h"
#include "PlayerMoveSpeedComponent.h"
#include "ShapeComponents.h"

namespace
{
	namespace Constants
	{
		constexpr const char* VictoryText = "VICTORY";
		constexpr const char* DefeatText = "DEFEAT";
	}

	bool IsStageCleared(SystemContext& Context)
	{
		const PaddleQuery Paddle(Context.QueryContext);
		const BricksQuery Bricks(Context.QueryContext);
		return Paddle.Size() > 1 && Bricks.Size() < 1;
	}

	bool IsRunLost(SystemContext& Context)
	{
		const TriggerQuery Trigger(Context.QueryContext);
		bool IsTriggerDead = false;
		Trigger.ForEach([&](Entity Entity, const PositionComponent& Position, const RectComponent& Rect, const HealthComponent& Health)
		{
			IsTriggerDead &= Health.CurrentHealth < 1;
		});
		return IsTriggerDead;
	}

	void OnQuitButtonClicked()
	{
		SDL_Event Event{};
		Event.type = SDL_EVENT_QUIT;
		SDL_PushEvent(&Event);
	}

	void TransitionToRun(SystemContext& Context)
	{
		ComponentUtils::RemoveAllEntitiesWithComponent<UIRenderComponent>(Context);
		TransitionUtils::TravelToRun(Context);
	}

	void OnTutorialButtonClicked(SystemContext& Context)
	{
		ComponentUtils::RemoveAllEntitiesWithComponent<UIRenderComponent>(Context);
		TransitionUtils::TravelToTutorial (Context);
	}

	void OnMainMenuButtonClicked(SystemContext& Context)
	{
		ComponentUtils::RemoveAllEntitiesWithComponent<UIRenderComponent>(Context);
		TransitionUtils::TravelToMainMenu(Context);
	}
}

void TransitionSystem::UpdateDynamicTransitions(SystemContext& Context, float DeltaTime)
{
	const Query<WritesList<>, ReadsList<SummaryTransitionComponent>, ExcludeList<>> SummaryTransitionQuery(Context.QueryContext);
	if (SummaryTransitionQuery.Size() > 0)
	{
		RemoveEntitiesCommand RemoveSummaryTransitionCommand(SummaryTransitionQuery.Size());
		SummaryTransitionQuery.ForEach([&](Entity Entity, const SummaryTransitionComponent& SummaryTransition)
		{
			TransitionUtils::CleanupRunStage(Context);
			TransitionUtils::TravelToSummary(Context, SummaryTransition.Message);
			RemoveSummaryTransitionCommand.WithEntry(Entity);
		});

		Context.Commands.Submit(std::move(RemoveSummaryTransitionCommand));

		return;
	}

	const Query<WritesList<>, ReadsList<UpgradesTransitionComponent>, ExcludeList<>> UpgradesTransitionQuery(Context.QueryContext);
	if (UpgradesTransitionQuery.Size() > 0)
	{
		RemoveEntitiesCommand RemoveUpgradesTransitionCommand(UpgradesTransitionQuery.Size());
		UpgradesTransitionQuery.ForEach([&](Entity Entity, const UpgradesTransitionComponent& UpgradesTransition)
		{
			TransitionUtils::CleanupRunStage(Context);
			TransitionUtils::TravelToUpgrades(Context);
			RemoveUpgradesTransitionCommand.WithEntry(Entity);
		});
		Context.Commands.Submit(std::move(RemoveUpgradesTransitionCommand));
		return;
	}
}

void TransitionSystem::UpdateClickableTransitions(SystemContext& Context, float DeltaTime)
{
	const Query<WritesList<>, ReadsList<ClickableUsedComponent, ClickableComponent>, ExcludeList<>> TransitionQuery(Context.QueryContext);
	TransitionQuery.ForEach([&](Entity Entity, const ClickableUsedComponent& ClickableUsed, const ClickableComponent& Clickable)
	{
		switch (Clickable.Tag)
		{
			case ClickableTag::PlayButton:
				TransitionToRun(Context);
				break;
			case ClickableTag::QuitButton:
				OnQuitButtonClicked();
				break;
			case ClickableTag::MainMenuButton:
				OnMainMenuButtonClicked(Context);
				break;
			case ClickableTag::TutorialButton:
				OnTutorialButtonClicked(Context);
				break;
			case ClickableTag::Upgrade:
				TransitionToRun(Context);
				break;
			case ClickableTag::Invalid:
			default:
				SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "RunControllerSystem::AdvanceToNextStage -> No more stages found. Victory!");
				break;
		}
	});
}

void TransitionSystem::UpdateTransitionsFromRun(SystemContext& Context, float DeltaTime)
{
	auto SignalSummaryTransition = [](SystemContext& Context, const char* SummaryText)
	{
		AddEntitiesCommand<SummaryTransitionComponent> AddSummaryTransitionCommand(1);
		AddSummaryTransitionCommand.WithEntry(SummaryTransitionComponent{ SummaryText });
		Context.Commands.Submit(std::move(AddSummaryTransitionCommand));
	};

	if (IsStageCleared(Context))
	{
		if (StageDataLoader::IsStageDataAvailable(RunUtils::GetCurrentStageNumber(Context) + 1))
		{
			AddEntitiesCommand<UpgradesTransitionComponent> AddUpgradesTransitionCommand(1);
			AddUpgradesTransitionCommand.WithEntry(UpgradesTransitionComponent{});
			Context.Commands.Submit(std::move(AddUpgradesTransitionCommand));
		}
		else
		{
			SignalSummaryTransition(Context, Constants::VictoryText);
		}
	}

	else if (IsRunLost(Context))
	{
		SignalSummaryTransition(Context, Constants::DefeatText);
	}
}