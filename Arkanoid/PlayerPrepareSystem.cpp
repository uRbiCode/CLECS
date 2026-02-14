#include "PlayerPrepareSystem.h"
#include "SystemContext.h"
#include "EventBus.h"
#include "ChangeRunStateEvent.h"
#include "SDLUtils.h"
#include "TransformComponent.h"
#include "ShapeComponents.h"
#include "RenderComponent.h"
#include "TextComponent.h"
#include "Constants.h"
#include "RenderConstants.h"
#include "PlayerPrepareComponent.h"
#include "EntityAdmin.h"
#include "InputState.h"
#include "PlayerReadyEvent.h"
#include <cassert>

namespace
{
	constexpr const char* PrepareMessageText = "PRESS SPACE TO START";
	constexpr const float PrepareMessageDisplayDuration = 0.75f;
}

void PlayerPrepareSystem::Initialize(const SystemContext& Context) const
{
	Context.EventBus.Subscribe<ChangeRunStateEvent>(this, [this](const SystemContext& Context, const ChangeRunStateEvent& Event)
	{
		if (Event.NewState != RunState::PlayerPrepare)
			return;
		
		AddPrepareMessage(Context);
	});
}

void PlayerPrepareSystem::Update(const SystemContext& Context, float DeltaTime) const
{
	const auto PrepareMessageGroup = Context.EntityAdmin.GetGroup<PlayerPrepareComponent>();
	if (PrepareMessageGroup.Empty())
		return;

	if (ShouldCleanupPrepareMessage(Context))
	{
		CleanupPrepareMessage(Context);
		Context.EventBus.Notify(Context, PlayerReadyEvent{});
		return;
	}

	auto& PlayerPrepareComp = Context.EntityAdmin.AccessComponent<PlayerPrepareComponent>(PrepareMessageGroup[0]);
	PlayerPrepareComp.DisplayTimer -= DeltaTime;
	if (PlayerPrepareComp.DisplayTimer > 0.f)
		return;

	PlayerPrepareComp.DisplayTimer = PrepareMessageDisplayDuration;
	ChangeMessageVisibility(Context, PrepareMessageGroup[0]);
}

void PlayerPrepareSystem::AddPrepareMessage(const SystemContext& Context) const
{
	const auto LogicalPresentation = SDLUtils::GetRendererLogicalPresentation(&Context.Renderer);
	const Vector2D<float> RectSize = { LogicalPresentation.X * 1.f, LogicalPresentation.Y * 0.1f };
	auto& Admin = Context.EntityAdmin;
	const auto PrepareMessageEntity = Admin.CreateEntity();
	Admin.AddComponent<TransformComponent>(PrepareMessageEntity, Vector2D<float>{ LogicalPresentation.X * 0.5f, LogicalPresentation.Y * 0.75f });
	Admin.AddComponent<RectComponent>(PrepareMessageEntity, SDL_FRect{ -RectSize.X * 0.5f, -RectSize.Y * 0.5f, RectSize.X, RectSize.Y });
	Admin.AddComponent<ColorComponent>(PrepareMessageEntity, SDL_FColor{0.f, 0.f, 0.f, 0.f});
	Admin.AddComponent<TextComponent>(PrepareMessageEntity, PrepareMessageText, Constants::FontFilePath, 50);
	Admin.AddComponent<RenderComponent>(PrepareMessageEntity, RenderConstants::UILayer);
	Admin.AddComponent<PlayerPrepareComponent>(PrepareMessageEntity, PlayerPrepareComponent{ PrepareMessageDisplayDuration });
}

void PlayerPrepareSystem::CleanupPrepareMessage(const SystemContext& Context) const
{
	Context.EntityAdmin.GetGroup<PlayerPrepareComponent>().ForEach([&Context](const Entity& Entity, const PlayerPrepareComponent& PlayerPrepare)
	{
		Context.EntityAdmin.DestroyEntity(Entity);
	});
}

void PlayerPrepareSystem::ChangeMessageVisibility(const SystemContext& Context, const Entity& Entity) const
{
	if (!Context.EntityAdmin.HasComponent<RenderComponent>(Entity))
		return;

	auto& RenderComp = Context.EntityAdmin.AccessComponent<RenderComponent>(Entity);
	RenderComp.Visible = !RenderComp.Visible;
}

bool PlayerPrepareSystem::ShouldCleanupPrepareMessage(const SystemContext& Context) const
{
	return Context.Input.IsKeyJustPressed(SDLK_SPACE);
}