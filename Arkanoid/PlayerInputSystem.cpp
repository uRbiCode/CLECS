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
	std::optional<ClickableTag> GetClickableTagFromEvent(const SystemContext& Context, const MouseClickEvent& Event)
	{
		if (Event.Button != SDL_BUTTON_LEFT)
			return std::nullopt;

		std::optional<ClickableTag> ClickedTag = std::nullopt;

		const auto LogicalEventPosition = SDLUtils::TranslateCoordinatesFromWindowToLogical(&Context.Renderer, &Context.Window, Event.Position);
		Context.EntityAdmin.GetGroup<ClickableComponent, TransformComponent, RectComponent>().ForEach([&ClickedTag, &Context, &LogicalEventPosition](const Entity& Entity, const ClickableComponent& Clickable, const TransformComponent& Transform, const RectComponent& Rect)
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
				ClickedTag = Clickable.Tag;
			}
		});

		return ClickedTag;
	}
}

void PlayerInputSystem::Initialize(const SystemContext& Context) const
{
	Context.EventBus.Subscribe<MouseClickEvent>(this, [this](const SystemContext& Context, const MouseClickEvent& Event)
	{
		OnMouseClick(Context, Event);
	});
}

void PlayerInputSystem::Update(const SystemContext& Context, float DeltaTime) const
{
	auto& Admin = Context.EntityAdmin;
	const auto& Input = Context.Input;

	auto PlayerGroup = Admin.GetGroup<VelocityComponent, PlayerControllerComponent>();
	PlayerGroup.ForEach([&Input](const Entity& Entity, VelocityComponent& Velocity, const PlayerControllerComponent& Controller)
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

void PlayerInputSystem::OnMouseClick(const SystemContext& Context, const MouseClickEvent& Event) const
{
	const auto ClickedTag = GetClickableTagFromEvent(Context, Event);
	if (!ClickedTag.has_value())
		return;

	const auto ClickedTagValue = ClickedTag.value();
	if (ClickedTagValue == ClickableTag::Invalid)
	{
		SDL_LogWarn(SDL_LOG_CATEGORY_INPUT, "PlayerInputSystem::OnMouseClick -> Clickable has invalid tag");
		return;
	}

	Context.EventBus.Notify(Context, ClickableUsedEvent{ ClickedTag.value() });
}