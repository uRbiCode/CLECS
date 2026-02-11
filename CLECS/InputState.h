#pragma once
#include <SDL3/SDL.h>
#include "MathTypes.h"
#include <unordered_set>

// Manages keyboard and mouse input states.
class InputState
{
public:
	InputState() = default;

	void ProcessEvent(const SDL_Event& Event);
	void BeginFrame();

	bool IsKeyHeld(SDL_Keycode Key) const;
	bool IsKeyJustPressed(SDL_Keycode Key) const;
	bool IsKeyDown(SDL_Keycode Key) const;
	bool IsKeyJustReleased(SDL_Keycode Key) const;

	bool IsMouseButtonHeld(Uint8 Button) const;
	bool IsMouseButtonJustPressed(Uint8 Button) const;
	bool IsMouseButtonDown(Uint8 Button) const;
	bool IsMouseButtonJustReleased(Uint8 Button) const;

	Vector2D<float> GetMousePosition() const { return MousePosition; }
	Vector2D<float> GetMouseDelta() const { return MouseDelta; }

private:
	std::unordered_set<SDL_Keycode> HeldKeys;
	
	std::unordered_set<SDL_Keycode> JustPressedKeys;
	std::unordered_set<SDL_Keycode> JustReleasedKeys;

	std::unordered_set<Uint8> HeldMouseButtons;
	
	std::unordered_set<Uint8> JustPressedMouseButtons;
	std::unordered_set<Uint8> JustReleasedMouseButtons;

	Vector2D<float> MousePosition{ 0.f, 0.f };
	Vector2D<float> MouseDelta{ 0.f, 0.f };

	friend class World;
};