#pragma once
#include <string>

enum class ClickableTag : uint8_t
{
	PlayButton = 0,
	QuitButton,
	MainMenuButton,
	Invalid
};

struct ClickableComponent
{
	ClickableTag Tag = ClickableTag::Invalid;
};