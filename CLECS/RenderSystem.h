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

// RenderSystem renders all entities with Transform and Shape components
class RenderSystem : public System
{
public:
	void Update(const SystemContext& Context, float DeltaTime) const override;

private:
	void RenderCircle(SDL_Renderer* Renderer, const Vector2D<float>& Center, float Radius, bool Filled, const SDL_FColor& Color) const;
	void DecideColor(const SystemContext& Context, const Entity& Entity) const;
	std::vector<std::pair<Entity, RenderData>> GetRenderEntities(const SystemContext& Context) const;
	void RenderTexture(const SystemContext& Context, const Entity& Entity, const RenderData& RenderData) const;
	void RenderShape(const SystemContext& Context, const Entity& Entity, const RenderData& RenderData) const;
	void RenderText(const SystemContext& Context, const Entity& Entity, const RenderData& RenderData) const;
};