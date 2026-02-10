#include "RenderSystem.h"
#include "SystemContext.h"
#include "EntityAdmin.h"
#include "SDL3/SDL.h"
#include "ShapeComponents.h"
#include "RenderComponent.h"
#include "TransformComponent.h"

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
			SDL_SetRenderDrawColor(&Renderer, 255, 255, 255, 255);
		}
	}

	using RenderData = std::pair<TransformComponent, RenderComponent>;
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

		if (Admin.HasComponent<RectComponent>(Entity))
		{
			const auto& Rect = Admin.GetComponent<RectComponent>(Entity);
			const SDL_FRect RenderRect{
				Transform.Position.X + Rect.Rect.x * Transform.Scale.X,
				Transform.Position.Y + Rect.Rect.y * Transform.Scale.Y,
				Rect.Rect.w * Transform.Scale.X,
				Rect.Rect.h * Transform.Scale.Y
			};
			if (IsFilled(Context, Entity))
			{
				SDL_RenderFillRect(&Renderer, &RenderRect);
			}
			else
			{
				SDL_RenderRect(&Renderer, &RenderRect);
			}

			continue;
		}

		if (Admin.HasComponent<CircleComponent>(Entity))
		{
			const auto& Circle = Admin.GetComponent<CircleComponent>(Entity);
			const Vector2D<float> Center = { Transform.Position.X, Transform.Position.Y };
			RenderCircle(&Renderer, Center, Circle.Radius * std::max(Transform.Scale.X, Transform.Scale.Y), IsFilled(Context, Entity), { 1.f, 1.f, 1.f, 1.f });

			continue;
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

			continue;
		}

		SDL_LogWarn(SDL_LOG_CATEGORY_RENDER, "Entity %d has RenderComponent but no shape component!", Entity.GetId());
	}

	SDL_RenderPresent(&Renderer);
}