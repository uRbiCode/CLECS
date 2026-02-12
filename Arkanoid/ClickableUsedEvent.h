#pragma once
#include "ClickableComponent.h"

struct ClickableUsedEvent
{
	Entity ClickedEntity = {};
	ClickableTag UsedClickableTag = ClickableTag::Invalid;
};