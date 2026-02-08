#include "RenderSystem.h"
#include "SystemUpdateContext.h"
#include "EntityManager.h"
#include "Components.h"
#include "SDL3/SDL.h"

namespace
{
	void RenderCircle(SDL_Renderer* Renderer, const Vector2D<float>& Center, const float Radius, const bool Filled, const SDL_FColor& Color)
	{
		// Number of line segments to approximate circle
		constexpr int Segments = 32;  
		constexpr float AngleStep = (2.f * SDL_PI_F) / Segments;
		
		if (Filled) 
		{
			// Draw filled circle using triangles from center
			for (int i = 0; i < Segments; i++) 
			{
				const float Angle1 = i * AngleStep;
				const float Angle2 = (i + 1) * AngleStep;
				
				const SDL_Vertex Vertices[3] = {
					{ { Center.X, Center.Y }, { Color.r, Color.g, Color.b, Color.a }, { 0, 0 } },  // Center
					{ { Center.X + SDL_cosf(Angle1) * Radius, Center.Y + SDL_sinf(Angle1) * Radius }, { Color.r, Color.g, Color.b, Color.a }, { 0, 0 } },
					{ { Center.X + SDL_cosf(Angle2) * Radius, Center.Y + SDL_sinf(Angle2) * Radius }, { Color.r, Color.g, Color.b, Color.a }, { 0, 0 } }
				};
				SDL_RenderGeometry(Renderer, nullptr, Vertices, 3, nullptr, 0);
			}
		} 
		else 
		{
			// Draw circle outline using line segments
			for (int i = 0; i <= Segments; i++) 
			{
				const float Angle1 = i * AngleStep;
				const float Angle2 = (i + 1) * AngleStep;
				
				const float X1 = Center.X + SDL_cosf(Angle1) * Radius;
				const float Y1 = Center.Y + SDL_sinf(Angle1) * Radius;
				const float X2 = Center.X + SDL_cosf(Angle2) * Radius;
				const float Y2 = Center.Y + SDL_sinf(Angle2) * Radius;
				
				SDL_RenderLine(Renderer, X1, Y1, X2, Y2);
			}
		}
	}
}

void RenderSystem::Update(const SystemUpdateContext& UpdateContext)
{
	auto& Renderer = UpdateContext.Renderer;
	auto& Manager = UpdateContext.EntityManager;

	// Clear the screen with a dark gray background
	SDL_SetRenderDrawColor(&Renderer, 30, 30, 30, 255);
	SDL_RenderClear(&Renderer);

	// Get all entities with both Transform and Shape components
	auto Group = Manager.GetGroup<TransformComponent, ShapeComponent>();

	// Use ForEach for efficient iteration - components are passed directly with no lookups
	Group.ForEach([&Renderer](Entity CurrentEntity, TransformComponent& Transform, ShapeComponent& Shape)
		{
			if (!Shape.Visible)
				return;

			// Set render draw color from shape color
			SDL_SetRenderDrawColorFloat(&Renderer, Shape.Color.r, Shape.Color.g, Shape.Color.b, Shape.Color.a);

			// Build destination rectangle with transform applied
			SDL_FRect RenderRect{};
			RenderRect.x = Transform.Position.X + Shape.Rect.x * Transform.Scale.X;
			RenderRect.y = Transform.Position.Y + Shape.Rect.y * Transform.Scale.Y;
			RenderRect.w = Shape.Rect.w * Transform.Scale.X;
			RenderRect.h = Shape.Rect.h * Transform.Scale.Y;

			switch (Shape.Type)
			{
			case ShapeComponent::ShapeType::Rectangle:
				if (Shape.Filled)
				{
					SDL_RenderFillRect(&Renderer, &RenderRect);
				}
				else
				{
					SDL_RenderRect(&Renderer, &RenderRect);
				}
				break;

			case ShapeComponent::ShapeType::Circle:
			{
				const Vector2D<float> Center { RenderRect.x + RenderRect.w * 0.5f, RenderRect.y + RenderRect.h * 0.5f };
				const float Radius = (RenderRect.w > RenderRect.h ? RenderRect.w : RenderRect.h) * 0.5f;

				RenderCircle(&Renderer, Center, Radius, Shape.Filled, Shape.Color);
				break;
			}

			case ShapeComponent::ShapeType::Line:
			{
				const float X1 = Transform.Position.X + Shape.Rect.x * Transform.Scale.X;
				const float Y1 = Transform.Position.Y + Shape.Rect.y * Transform.Scale.Y;
				const float X2 = X1 + Shape.Rect.w * Transform.Scale.X;
				const float Y2 = Y1 + Shape.Rect.h * Transform.Scale.Y;

				SDL_RenderLine(&Renderer, X1, Y1, X2, Y2);
				break;
			}
			}
		});

	// Present the rendered frame
	SDL_RenderPresent(&Renderer);
}