#pragma once
#include "System.h"
#include "MathTypes.h"
#include <vector>

struct SDL_Renderer;
struct SDL_FColor;
struct Entity;
struct TransformComponent;
struct RenderComponent;
struct SystemContext;

using RenderData = std::pair<TransformComponent, RenderComponent>;

/* RenderSystem is responsible for rendering entities that have both TransformComponent and RenderComponent attached.
 * It determines their visual representation based on owned components, and renders them using SDL.
 * It supports rendering shapes, textures, and text, and it also handles layering and visibility.
 */
class RenderSystem : public System
{
public:
	void Update(const SystemContext& Context, float DeltaTime) const override;

private:
	// Renderer management.
	void DecideColor(const SystemContext& Context, const Entity& Entity) const;

	// Render methods.
	void RenderCircle(SDL_Renderer* Renderer, const Vector2D<float>& Center, float Radius, bool Filled, const SDL_FColor& Color) const;
	void RenderTexture(const SystemContext& Context, const Entity& Entity, const RenderData& RenderData) const;
	void RenderShape(const SystemContext& Context, const Entity& Entity, const RenderData& RenderData) const;
	void RenderText(const SystemContext& Context, const Entity& Entity, const RenderData& RenderData) const;
};