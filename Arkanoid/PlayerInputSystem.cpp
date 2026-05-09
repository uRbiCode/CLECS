#include "PlayerInputSystem.h"
#include "SystemContext.h"
#include "EntityAdmin.h"
#include "InputState.h"
#include "SDL3/SDL.h"
#include "PlayerControllerComponent.h"
#include "VelocityComponent.h"
#include "EventBus.h"
#include "MouseClickEvent.h"
#include "ClickableComponent.h"
#include "TransformComponent.h"
#include "ShapeComponents.h"
#include "ClickableUsedEvent.h"
#include "SDLUtils.h"
#include <optional>

namespace
{
	std::optional<std::pair<Entity, ClickableTag>> GetClickableFromEvent(const SystemContext& Context, const MouseClickEvent& Event)
	{
		if (Event.Button != SDL_BUTTON_LEFT)
			return std::nullopt;

		std::optional<std::pair<Entity, ClickableTag>> ClickedPair = std::nullopt;
		const auto LogicalEventPosition = SDLUtils::TranslateCoordinatesFromWindowToLogical(&Context.Renderer, &Context.Window, Event.Position);

		Context.EntityAdmin.GetGroup<ClickableComponent, TransformComponent, RectComponent>().ForEach([&ClickedPair, &Context, &LogicalEventPosition](const Entity& Entity, const ClickableComponent& Clickable, const TransformComponent& Transform, const RectComponent& Rect)
		{
			const auto Left = Transform.Position.X + Rect.Rect.x;
			const auto Right = Left + Rect.Rect.w;
			const auto Top = Transform.Position.Y + Rect.Rect.y;
			const auto Bottom = Top + Rect.Rect.h;

			if (LogicalEventPosition.X >= Left 
				&& LogicalEventPosition.X <= Right 
				&& LogicalEventPosition.Y >= Top 
				&& LogicalEventPosition.Y <= Bottom)
			{
				ClickedPair = std::make_pair(Entity, Clickable.Tag);
			}
		});

		return ClickedPair;
	}

	void OnMouseClick(const SystemContext& Context, const MouseClickEvent& Event)
	{
		const auto ClickedPair = GetClickableFromEvent(Context, Event);
		if (!ClickedPair.has_value())
			return;

		const auto& [ClickedEntity, ClickedTagValue] = ClickedPair.value();
		if (ClickedTagValue == ClickableTag::Invalid)
		{
			SDL_LogWarn(SDL_LOG_CATEGORY_INPUT, "PlayerInputSystem::OnMouseClick -> Clickable has invalid tag");
			return;
		}

		Context.EventBus.Notify(Context, ClickableUsedEvent{ ClickedEntity, ClickedTagValue });
	}
}

void PlayerInputSystem::Initialize(const SystemContext& Context)
{
	const void* const Id = reinterpret_cast<const void*>(&Initialize);

	Context.EventBus.Subscribe<MouseClickEvent>(Id, [](const SystemContext& Context, const MouseClickEvent& Event)
	{
		OnMouseClick(Context, Event);
	});
}

void PlayerInputSystem::Update(const SystemContext& Context, float DeltaTime)
{
	const auto& Input = Context.Input;
	Context.EntityAdmin.GetGroup<VelocityComponent, PlayerControllerComponent>().ForEach([&Input](const Entity& Entity, VelocityComponent& Velocity, const PlayerControllerComponent& Controller)
	{
		Velocity.Velocity = { 0.f, 0.f };

		if (Input.IsKeyDown(SDLK_A) || Input.IsKeyDown(SDLK_LEFT))
		{
			Velocity.Velocity.X -= Controller.MoveSpeed;
		}
		if (Input.IsKeyDown(SDLK_D) || Input.IsKeyDown(SDLK_RIGHT))
		{
			Velocity.Velocity.X += Controller.MoveSpeed;
		}
	});
}