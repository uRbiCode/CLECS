#include "UIBlinkSystem.h"
#include "SystemContext.h"
#include "UIBlinkComponent.h"
#include "Query.h"
#include "RenderComponents.h"
#include "CommandRunner.h"
#include "BeginStageComponent.h"
#include "ComponentUtils.h"

namespace
{
	void UpdateInput(SystemContext& Context, float DeltaTime)
	{
		const Query<WritesList<>, ReadsList<BeginStageComponent>, ExcludeList<>> InputQuery(Context.QueryContext);
		if (InputQuery.Size() < 1)
			return;

		ComponentUtils::RemoveAllEntitiesWithComponent<UIBlinkComponent>(Context);
	}

	void RegularUpdate(SystemContext& Context, float DeltaTime)
	{
		const Query<WritesList<UIBlinkComponent>, ReadsList<>, ExcludeList<>> BlinkQuery(Context.QueryContext);
		BlinkQuery.ForEach([&](Entity Entity, UIBlinkComponent& Blink)
		{
			Blink.Timer -= DeltaTime;
		});

		const Query<WritesList<UIBlinkComponent>, ReadsList<UIRenderComponent>, ExcludeList<>> RemoveBlinkQuery(Context.QueryContext);
		if (RemoveBlinkQuery.Size() > 0)
		{
			RemoveComponentsCommand<UIRenderComponent> RemoveBlinkCommand(RemoveBlinkQuery.Size());
			RemoveBlinkQuery.ForEach([&](Entity Entity, UIBlinkComponent& Blink, const UIRenderComponent& Render)
			{
				if (Blink.Timer > 0.f)
					return;

				Blink.Timer = Blink.StartingTimer;
				RemoveBlinkCommand.WithEntry(Entity);
			});

			if (!RemoveBlinkCommand.GetEntries().empty())
			{
				Context.Commands.Submit(std::move(RemoveBlinkCommand));
			}
		}

		const Query<WritesList<UIBlinkComponent>, ReadsList<>, ExcludeList<UIRenderComponent>> AddBlinkQuery(Context.QueryContext);
		if (AddBlinkQuery.Size() > 0)
		{
			AddComponentsCommand<UIRenderComponent> AddBlinkCommand(AddBlinkQuery.Size());
			AddBlinkQuery.ForEach([&](Entity Entity, UIBlinkComponent& Blink)
			{
				if (Blink.Timer > 0.f)
					return;

				Blink.Timer = Blink.StartingTimer;
				AddBlinkCommand.WithEntry(Entity, UIRenderComponent{});
			});

			if (!AddBlinkCommand.AccessEntries().empty())
			{
				Context.Commands.Submit(std::move(AddBlinkCommand));
			}
		}
	}
}

void UIBlinkSystem::Update(SystemContext& Context, float DeltaTime)
{
	UpdateInput(Context, DeltaTime);
	RegularUpdate(Context, DeltaTime);
}