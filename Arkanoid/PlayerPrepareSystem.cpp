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
#include "EntityAdmin.h"
#include "InputState.h"
#include "PlayerReadyEvent.h"
#include <cassert>

namespace
{
	constexpr const char* PrepareMessageText = "PRESS SPACE TO START";
	constexpr float PrepareMessageDisplayDuration = 0.75f;

	bool ShouldCleanupPrepareMessage(const SystemContext& Context)
	{
		return Context.Input.IsKeyJustPressed(SDLK_SPACE);
	}

	void ChangeMessageVisibility(const SystemContext& Context, const Entity& Entity)
	{
		if (!Context.EntityAdmin.HasComponent<RenderComponent>(Entity))
			return;

		auto& RenderComp = Context.EntityAdmin.AccessComponent<RenderComponent>(Entity);
		RenderComp.Visible = !RenderComp.Visible;
	}

	void CleanupPrepareMessage(const SystemContext& Context)
	{
		Context.EntityAdmin.GetGroup<PlayerPrepareComponent>().ForEach([&Context](const Entity& Entity, const PlayerPrepareComponent& PlayerPrepare)
		{
			Context.EntityAdmin.DestroyEntity(Entity);
		});
	}

	void AddPrepareMessage(const SystemContext& Context)
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
}

void PlayerPrepareSystem::Initialize(const SystemContext& Context)
{
	const void* const Id = reinterpret_cast<const void*>(&Initialize);

	Context.EventBus.Subscribe<ChangeRunStateEvent>(Id, [](const SystemContext& Context, const ChangeRunStateEvent& Event)
	{
		if (Event.NewState != RunState::PlayerPrepare)
			return;
		
		AddPrepareMessage(Context);
	});
}

void PlayerPrepareSystem::Update(SystemQuery<Writes<PlayerPrepareComponent>, Reads<>>& Query, const SystemContext& Context, float DeltaTime)
{
	if (ShouldCleanupPrepareMessage(Context))
	{
		CleanupPrepareMessage(Context);
		Context.EventBus.Notify(Context, PlayerReadyEvent{});
		return;
	}

	Query.ForEach([&Context, DeltaTime](const Entity& Entity, PlayerPrepareComponent& PlayerPrepare)
	{
		PlayerPrepare.DisplayTimer -= DeltaTime;
		if (PlayerPrepare.DisplayTimer > 0.f)
			return;

		PlayerPrepare.DisplayTimer = PrepareMessageDisplayDuration;
		ChangeMessageVisibility(Context, Entity);
	});
}