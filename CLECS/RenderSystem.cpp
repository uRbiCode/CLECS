#include "RenderSystem.h"
#include "SystemContext.h"
#include "EntityAdmin.h"
#include "SDL3/SDL.h"
#include "ShapeComponents.h"
#include "RenderComponent.h"
#include "TransformComponent.h"
#include "TextureComponent.h"
#include <optional>

using RenderData = std::pair<TransformComponent, RenderComponent>;

namespace
{
	void RenderCircle(SDL_Renderer* Renderer, const Vector2D<float>& Center, float Radius, bool Filled, const SDL_FColor& Color)
	{
		constexpr int Segments = 32;  
		constexpr float AngleStep = (2.f * SDL_PI_F) / Segments;
		
		if (Filled) 
		{
			// Draw filled circle using triangles from center
			for (int i = 0; i < Segments; ++i) 
			{
				const float Angle1 = i * AngleStep;
				const float Angle2 = (i + 1) * AngleStep;
				
				const SDL_Vertex Vertices[3] = {
					{ { Center.X, Center.Y }, { Color.r, Color.g, Color.b, Color.a }, { 0, 0 } },  // Center
					{ { Center.X + std::cosf(Angle1) * Radius, Center.Y + std::sinf(Angle1) * Radius }, { Color.r, Color.g, Color.b, Color.a }, { 0, 0 } },
					{ { Center.X + std::cosf(Angle2) * Radius, Center.Y + std::sinf(Angle2) * Radius }, { Color.r, Color.g, Color.b, Color.a }, { 0, 0 } }
				};
				SDL_RenderGeometry(Renderer, nullptr, Vertices, 3, nullptr, 0);
			}
		} 
		else 
		{
			// Draw circle outline using line segments
			for (int i = 0; i <= Segments; ++i) 
			{
				const float Angle1 = i * AngleStep;
				const float Angle2 = (i + 1) * AngleStep;
				
				const float X1 = Center.X + std::cosf(Angle1) * Radius;
				const float Y1 = Center.Y + std::sinf(Angle1) * Radius;
				const float X2 = Center.X + std::cosf(Angle2) * Radius;
				const float Y2 = Center.Y + std::sinf(Angle2) * Radius;
				
				SDL_RenderLine(Renderer, X1, Y1, X2, Y2);
			}
		}
	}

	Vector2D<float> RotatePoint(const Vector2D<float>& Point, const Vector2D<float>& Center, float Angle)
	{
		const float CosA = std::cosf(Angle);
		const float SinA = std::sinf(Angle);
		
		const Vector2D<float> Delta = { Point.X - Center.X, Point.Y - Center.Y };
		
		return {
			Center.X + Delta.X * CosA - Delta.Y * SinA,
			Center.Y + Delta.X * SinA + Delta.Y * CosA
		};
	}

	void DecideColor(const SystemContext& Context, const Entity& Entity)
	{
		auto& Renderer = Context.Renderer;
		auto& Admin = Context.EntityAdmin;
		if (Admin.HasComponent<ColorComponent>(Entity))
		{
			const auto& Color = Admin.GetComponent<ColorComponent>(Entity);
			SDL_SetRenderDrawColorFloat(&Renderer, Color.Color.r, Color.Color.g, Color.Color.b, Color.Color.a);
		}
		else
		{
			SDL_SetRenderDrawColor(&Renderer, 255, 255, 255, 0);
		}
	}

	std::vector<std::pair<Entity, RenderData>> GetRenderEntities(const SystemContext& Context)
	{
		auto& Admin = Context.EntityAdmin;
		auto RenderGroup = Admin.GetGroup<TransformComponent, RenderComponent>();
		std::vector<std::pair<Entity, RenderData>> SortedEntities;
		SortedEntities.reserve(RenderGroup.Size());
		for (const auto& Entity : RenderGroup)
		{
			const auto& Render = Admin.GetComponent<RenderComponent>(Entity);
			if (!Render.Visible)
				continue;

			const auto& Transform = Admin.GetComponent<TransformComponent>(Entity);
			SortedEntities.emplace_back(Entity, std::make_pair(Transform, Render));
		}
		std::sort(SortedEntities.begin(), SortedEntities.end(), [](const auto& A, const auto& B)
		{
			return A.second.second.Layer < B.second.second.Layer;
		});
		return SortedEntities;
	}

	bool IsFilled(const SystemContext& Context, const Entity& Entity)
	{
		auto& Admin = Context.EntityAdmin;
		if (Admin.HasComponent<ShapeFillComponent>(Entity))
		{
			return Admin.GetComponent<ShapeFillComponent>(Entity).Filled;
		}
		return true;
	}

	SDL_FRect CalcRenderRect(const TransformComponent& Transform, const RectComponent& Rect)
	{
		return SDL_FRect{
			Transform.Position.X + Rect.Rect.x * Transform.Scale.X,
			Transform.Position.Y + Rect.Rect.y * Transform.Scale.Y,
			Rect.Rect.w * Transform.Scale.X,
			Rect.Rect.h * Transform.Scale.Y
		};
	}

	SDL_FRect CalcRenderRectFromCircle(const TransformComponent& Transform, const CircleComponent& Circle)
	{
		const float Diameter = Circle.Radius * 2.f;
		return SDL_FRect{
			Transform.Position.X - Circle.Radius,
			Transform.Position.Y - Circle.Radius,
			Diameter,
			Diameter
		};
	}

	void RenderTexture(const SystemContext& Context, const Entity& Entity, const RenderData& RenderData)
	{
		auto& Admin = Context.EntityAdmin;
		auto& Renderer = Context.Renderer;

		const auto& [Transform, Render] = RenderData;
		const auto& TexComponent = Admin.GetComponent<TextureComponent>(Entity);
		if (TexComponent.Texture == nullptr)
			return;

		if (TexComponent.SourceRect.w == 0.f || TexComponent.SourceRect.h == 0.f)
		{
			SDL_LogWarn(SDL_LOG_CATEGORY_RENDER, "RenderSystem::RenderTexture -> TextureComponent for Entity %u has zero width or height in SourceRect", Entity.GetId());
			return;
		}
		
		std::optional<SDL_FRect> RenderRect;
		if (Admin.HasComponent<CircleComponent>(Entity))
		{
			const auto& CircleComp = Admin.GetComponent<CircleComponent>(Entity);
			RenderRect = CalcRenderRectFromCircle(Transform, CircleComp);
		}
		else if (Admin.HasComponent<RectComponent>(Entity))
		{
			const auto& RectComp = Admin.GetComponent<RectComponent>(Entity);
			RenderRect = CalcRenderRect(Transform, RectComp);
		}

		if (!RenderRect.has_value())
		{
			SDL_LogWarn(SDL_LOG_CATEGORY_RENDER, "RenderSystem::RenderTexture -> Entity %u does not have a valid shape component for calculating render rect", Entity.GetId());
			return;
		}
	
		if (Admin.HasComponent<ColorComponent>(Entity))
		{
			const auto& Color = Admin.GetComponent<ColorComponent>(Entity);
			SDL_SetTextureColorModFloat(TexComponent.Texture, Color.Color.r, Color.Color.g, Color.Color.b);
			SDL_SetTextureAlphaModFloat(TexComponent.Texture, Color.Color.a);
		}
		
		const auto& RenderRectValue = RenderRect.value();
		if (Transform.Rotation != 0.f)
		{
			const SDL_FPoint Center = {RenderRectValue.w * 0.5f, RenderRectValue.h * 0.5f};
			SDL_RenderTextureRotated(&Renderer, TexComponent.Texture, &TexComponent.SourceRect, &RenderRectValue, 
									 Transform.Rotation * (180.0 / SDL_PI_D), &Center, SDL_FLIP_NONE);
		}
		else
		{
			SDL_RenderTexture(&Renderer, TexComponent.Texture, &TexComponent.SourceRect, &RenderRectValue);
		}
	}
}

void RenderSystem::Update(const SystemContext& Context, float DeltaTime) const
{
	auto& Renderer = Context.Renderer;
	auto& Admin = Context.EntityAdmin;

	SDL_SetRenderDrawColor(&Renderer, 0, 0, 0, 255);
	SDL_RenderClear(&Renderer);

	const auto RenderEntities = GetRenderEntities(Context);

	for (const auto& [Entity, RenderData] : RenderEntities)
	{
		const auto& [Transform, Render] = RenderData;
		DecideColor(Context, Entity);

		if (Admin.HasComponent<TextureComponent>(Entity))
		{
			RenderTexture(Context, Entity, RenderData);
			continue;
		}

		if (Admin.HasComponent<RectComponent>(Entity))
		{
			const auto& Rect = Admin.GetComponent<RectComponent>(Entity);
			const auto RenderRect = CalcRenderRect(Transform, Rect);

			if (IsFilled(Context, Entity))
			{
				SDL_RenderFillRect(&Renderer, &RenderRect);
			}
			else
			{
				SDL_RenderRect(&Renderer, &RenderRect);
			}
		}

		if (Admin.HasComponent<CircleComponent>(Entity))
		{
			const auto& Circle = Admin.GetComponent<CircleComponent>(Entity);
			const Vector2D<float> Center = { Transform.Position.X, Transform.Position.Y };
			RenderCircle(&Renderer, Center, Circle.Radius * std::max(Transform.Scale.X, Transform.Scale.Y), 
			             IsFilled(Context, Entity), { 1.f, 1.f, 1.f, 1.f });
		}

		if (Admin.HasComponent<LineComponent>(Entity))
		{
			const auto& Line = Admin.GetComponent<LineComponent>(Entity);
			Vector2D<float> Point1 = {
				Transform.Position.X + Line.Start.X * Transform.Scale.X,
				Transform.Position.Y + Line.Start.Y * Transform.Scale.Y
			};
			Vector2D<float> Point2 = {
				Transform.Position.X + Line.End.X * Transform.Scale.X,
				Transform.Position.Y + Line.End.Y * Transform.Scale.Y
			};
			if (Transform.Rotation != 0.f)
			{
				Point1 = RotatePoint(Point1, Transform.Position, Transform.Rotation);
				Point2 = RotatePoint(Point2, Transform.Position, Transform.Rotation);
			}
			SDL_RenderLine(&Renderer, Point1.X, Point1.Y, Point2.X, Point2.Y);
		}
	}

	SDL_RenderPresent(&Renderer);
}