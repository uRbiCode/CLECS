#pragma once

struct SystemContext;

/* RenderSystem is responsible for displaying entities on the screen.
 * To be renderable, an entity must have a PositionComponent and one of RenderComponents.
 * Rendering does pass for each layer: Background, Game, and UI.
 *
 * RenderOrder:
 * 1. Render Background Layer
 * 1.1 Render Rects
 * 1.2 Render Filled Rects
 * 1.3 Render Circles
 * 1.4 Render Filled Circles
 * 1.5 Render Rect Textures
 * 1.6 Render Circle Textures
 * 1.7 Render Rect Wrapped Text
 * 1.8 Render Circle Wrapped Text
 * 1.9 Render Unwrapped Text
 *
 * 2. Render Game Layer
 * 3. Render UI Layer
 */
namespace RenderSystem
{
	void Update(const SystemContext& Context, float DeltaTime);
}