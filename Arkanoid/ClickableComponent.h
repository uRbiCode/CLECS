#pragma once
#include <string>

enum class ClickableTag
{
	PlayButton = 0,
	QuitButton,
	Invalid
};

struct ClickableComponent
{
	ClickableTag Tag = ClickableTag::Invalid;
};