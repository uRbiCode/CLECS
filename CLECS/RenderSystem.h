#pragma once
#include "SystemQuery.h"
#include "TransformComponent.h"
#include "RenderComponent.h"

/* RenderSystem is responsible for rendering entities that have both TransformComponent and RenderComponent attached.
 * It determines their visual representation based on owned components, and renders them using SDL.
 * It supports rendering shapes, textures, and text, and it also handles layering and visibility.
 */
namespace RenderSystem
{
	void Update(SystemQuery<Writes<>,Reads<TransformComponent, RenderComponent>>& Query, const SystemContext& Context, float DeltaTime);
}