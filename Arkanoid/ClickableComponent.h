#pragma once
#include <string>
#include <cstdint>

enum class ClickableTag : uint8_t
{
	PlayButton = 0,
	QuitButton,
	MainMenuButton,
	TutorialButton,
	Upgrade,
	Invalid
};

/* Used by PlayerInputSystem to recognize which "button" was clicked.
 * Each Tag is forwarded via Events to other systems so that they can react.
 */
struct ClickableComponent
{
	ClickableTag Tag = ClickableTag::Invalid;
};