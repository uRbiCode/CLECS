#pragma once

struct SystemContext;

/* RenderSystem is responsible for rendering entities that have both TransformComponent and RenderComponent attached.
 * It determines their visual representation based on owned components, and renders them using SDL.
 * It supports rendering shapes, textures, and text, and it also handles layering and visibility.
 */
namespace RenderSystem
{
	void Update(SystemContext& Context, float DeltaTime);
}