#include "TransitionSystem.h"
#include "SystemContext.h"
#include "Query.h"
#include "ClickableComponent.h"
#include "ComponentUtils.h"
#include "TransitionUtils.h"
#include "RenderComponents.h"

namespace
{
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

	void UpdateClickableTransitions(SystemContext& Context, float DeltaTime)
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
}

void TransitionSystem::UpdateTransition(SystemContext& Context, float DeltaTime)
{
	UpdateClickableTransitions(Context, DeltaTime);
}