#include "InputState.h"

void InputState::ProcessEvent(const SDL_Event& Event)
{
	switch (Event.type)
	{
	case SDL_EVENT_KEY_DOWN:
		if (!Event.key.repeat)
		{
			HeldKeys.insert(Event.key.key);
			JustPressedKeys.insert(Event.key.key);
		}
		break;

	case SDL_EVENT_KEY_UP:
		HeldKeys.erase(Event.key.key);
		JustReleasedKeys.insert(Event.key.key);
		break;

	case SDL_EVENT_MOUSE_BUTTON_DOWN:
		HeldMouseButtons.insert(Event.button.button);
		JustPressedMouseButtons.insert(Event.button.button);
		break;

	case SDL_EVENT_MOUSE_BUTTON_UP:
		HeldMouseButtons.erase(Event.button.button);
		JustReleasedMouseButtons.insert(Event.button.button);
		break;

	case SDL_EVENT_MOUSE_MOTION:
		MousePosition.X = Event.motion.x;
		MousePosition.Y = Event.motion.y;
		MouseDelta.X = Event.motion.xrel;
		MouseDelta.Y = Event.motion.yrel;
		break;

	case SDL_EVENT_WINDOW_FOCUS_LOST:
		HeldKeys.clear();
		HeldMouseButtons.clear();
		break;

	default:
		break;
	}
}

void InputState::BeginFrame()
{
	// Clear only per-frame state
	// HeldKeys and HeldMouseButtons persist across frames
	JustPressedKeys.clear();
	JustReleasedKeys.clear();
	JustPressedMouseButtons.clear();
	JustReleasedMouseButtons.clear();
	MouseDelta.X = 0.f;
	MouseDelta.Y = 0.f;
}

bool InputState::IsKeyPressed(SDL_Keycode Key) const
{
	return HeldKeys.contains(Key);
}

bool InputState::IsKeyJustPressed(SDL_Keycode Key) const
{
	return JustPressedKeys.contains(Key);
}

bool InputState::IsKeyJustReleased(SDL_Keycode Key) const
{
	return JustReleasedKeys.contains(Key);
}

bool InputState::IsMouseButtonPressed(Uint8 Button) const
{
	return HeldMouseButtons.contains(Button);
}

bool InputState::IsMouseButtonJustPressed(Uint8 Button) const
{
	return JustPressedMouseButtons.contains(Button);
}

bool InputState::IsMouseButtonJustReleased(Uint8 Button) const
{
	return JustReleasedMouseButtons.contains(Button);
}