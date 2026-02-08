#pragma once
#include <SDL3/SDL.h>
#include "MathTypes.h"
#include <unordered_set>

/* InputState manages keyboard and mouse input states.
 * It tracks pressed keys, mouse buttons, and mouse position.
 * Updated by World and made available to Systems via SystemUpdateContext.
 */
class InputState
{
public:
	InputState() = default;

	void ProcessEvent(const SDL_Event& Event);
	void BeginFrame();

	// Keyboard input
	bool IsKeyHeld(SDL_Keycode Key) const;
	bool IsKeyJustPressed(SDL_Keycode Key) const;
	bool IsKeyDown(SDL_Keycode Key) const;
	bool IsKeyJustReleased(SDL_Keycode Key) const;

	// Mouse input
	bool IsMouseButtonHeld(Uint8 Button) const;
	bool IsMouseButtonJustPressed(Uint8 Button) const;
	bool IsMouseButtonDown(Uint8 Button) const;
	bool IsMouseButtonJustReleased(Uint8 Button) const;

	Vector2D<float> GetMousePosition() const { return MousePosition; }
	Vector2D<float> GetMouseDelta() const { return MouseDelta; }

private:
	// Persistent keyboard state
	std::unordered_set<SDL_Keycode> HeldKeys;
	
	// Per-frame keyboard state
	std::unordered_set<SDL_Keycode> JustPressedKeys;
	std::unordered_set<SDL_Keycode> JustReleasedKeys;

	// Persistent mouse button state
	std::unordered_set<Uint8> HeldMouseButtons;
	
	// Per-frame mouse button statecleared each frame)
	std::unordered_set<Uint8> JustPressedMouseButtons;
	std::unordered_set<Uint8> JustReleasedMouseButtons;

	// Mouse position and delta
	Vector2D<float> MousePosition{ 0.f, 0.f };
	Vector2D<float> MouseDelta{ 0.f, 0.f };
};