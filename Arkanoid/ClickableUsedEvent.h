#pragma once
#include "ClickableComponent.h"

struct Entity;

/* Send by PlayerInputSystem whenever a clickable Entity was, well, clicked.
 * Systems then know if they should invoke specific logic in response.
 */
struct ClickableUsedEvent
{
	const Entity& ClickedEntity;
	ClickableTag UsedClickableTag = ClickableTag::Invalid;
};