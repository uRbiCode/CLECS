#pragma once
#include <string>
#include <cstdint>

enum class ClickableTag : uint8_t
{
	PlayButton = 0,
	QuitButton,
	MainMenuButton,
	Upgrade,
	Invalid
};

struct ClickableComponent
{
	ClickableTag Tag = ClickableTag::Invalid;
};