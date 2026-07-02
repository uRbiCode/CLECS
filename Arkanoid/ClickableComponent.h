#pragma once
#include <cstdint>

enum class ClickableTag : uint8_t
{
	PlayButton,
	QuitButton,
	MainMenuButton,
	TutorialButton,
	Upgrade,
	Invalid
};

/* Used by PlayerInputSystem to recognize which button was clicked.
 */
struct ClickableComponent
{
	ClickableTag Tag = ClickableTag::Invalid;
};