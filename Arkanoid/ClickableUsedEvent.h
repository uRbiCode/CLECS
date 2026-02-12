#pragma once
#include "ClickableComponent.h"
#include "Entity.h"

struct ClickableUsedEvent
{
	Entity ClickedEntity = {};
	ClickableTag UsedClickableTag = ClickableTag::Invalid;
};