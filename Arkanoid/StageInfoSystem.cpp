#include "StageInfoSystem.h"
#include "SystemContext.h"
#include "EventBus.h"
#include "GameStateEvents.h"
#include "EntityAdmin.h"
#include "RunStateComponent.h"
#include "TransformComponent.h"
#include "SDLUtils.h"
#include "ShapeComponents.h"
#include "RenderComponent.h"
#include "TextComponent.h"
#include "RenderConstants.h"
#include "Constants.h"
#include "ChangeRunStateEvent.h"
#include <cassert>

namespace
{
	constexpr const char* StageText = "STAGE ";

	std::string BuildStageText(const SystemContext& Context)
	{
		int StageNumber = 0;
		const auto RunStateGroup = Context.EntityAdmin.GetGroup<RunStateComponent>();
		assert(RunStateGroup.Size() == 1 && "Expected exactly one RunStateComponent in the world");

		if (!RunStateGroup.Empty())
		{
			StageNumber = Context.EntityAdmin.GetComponent<RunStateComponent>(RunStateGroup[0]).CurrentStage;
		}

		return StageText + std::to_string(StageNumber);
	}
}

void StageInfoSystem::Initialize(const SystemContext& Context) const
{
	Context.EventBus.Subscribe<GameStateBeginEvent>(this, [this](const SystemContext& Context, const GameStateBeginEvent& Event)
	{
		if (Event.BeginningState != GameState::Run)
			return;

		InitializeStageInfo(Context);
	});

	Context.EventBus.Subscribe<ChangeRunStateEvent>(this, [this](const SystemContext& Context, const ChangeRunStateEvent& Event)
	{
		if (Event.NewState != RunState::Stage)
			return;

		RefreshStageInfo(Context);
	});
}

void StageInfoSystem::InitializeStageInfo(const SystemContext& Context) const
{
	auto& Admin = Context.EntityAdmin;
	const auto RunStateGroup = Admin.GetGroup<RunStateComponent>();
	assert(RunStateGroup.Size() == 1 && "Expected exactly one RunStateComponent in the world");
	if (RunStateGroup.Empty())
		return;

	const auto LogicalPresentation = SDLUtils::GetRendererLogicalPresentation(&Context.Renderer);
	const Vector2D<float> RectSize = { LogicalPresentation.X * 0.25f, LogicalPresentation.Y * 0.1f };

	Admin.AddComponent<TransformComponent>(RunStateGroup[0], Vector2D<float>{ LogicalPresentation.X * 0.8f, LogicalPresentation.Y * 0.95f });
	Admin.AddComponent<RectComponent>(RunStateGroup[0], SDL_FRect{-RectSize.X * 0.5f, -RectSize.Y * 0.5f, RectSize.X, RectSize.Y});
	Admin.AddComponent<ColorComponent>(RunStateGroup[0], SDL_FColor{0.f, 0.f, 0.f, 0.f});
	Admin.AddComponent<TextComponent>(RunStateGroup[0], std::move(BuildStageText(Context)), Constants::FontFilePath, 24);
	Admin.AddComponent<RenderComponent>(RunStateGroup[0], RenderConstants::UILayer);
}

void StageInfoSystem::RefreshStageInfo(const SystemContext& Context) const
{
	auto& Admin = Context.EntityAdmin;
	const auto RunStateGroup = Admin.GetGroup<RunStateComponent>();
	if (RunStateGroup.Empty() || !Admin.HasComponent<TextComponent>(RunStateGroup[0]))
		return;

	auto& TextComp = Admin.AccessComponent<TextComponent>(RunStateGroup[0]);
	TextComp.Text = std::move(BuildStageText(Context));
}